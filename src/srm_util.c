/*
 * Copyright (c) Members of the EGEE Collaboration. 2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Todor Manev  IT-GT CERN
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "srmv2Stub.h"
#include "srm_soap.h"
#include "srm_util.h"
#include "gfal_srm_ifce_internal.h"

#ifdef CMAKE_BUILD
#define namespaces_srmv2 namespaces
#endif 

const char *err_msg_begin = "SE";

const char *srmv2_errmsg[] = {
	"SRM_SUCCESS",
	"SRM_FAILURE",
	"SRM_AUTHENTICATION_FAILURE",
	"SRM_AUTHORIZATION_FAILURE",
	"SRM_INVALID_REQUEST",
	"SRM_INVALID_PATH",
	"SRM_FILE_LIFETIME_EXPIRED",
	"SRM_SPACE_LIFETIME_EXPIRED",
	"SRM_EXCEED_ALLOCATION",
	"SRM_NO_USER_SPACE",
	"SRM_NO_FREE_SPACE",
	"SRM_DUPLICATION_ERROR",
	"SRM_NON_EMPTY_DIRECTORY",
	"SRM_TOO_MANY_RESULTS",
	"SRM_INTERNAL_ERROR",
	"SRM_FATAL_INTERNAL_ERROR",
	"SRM_NOT_SUPPORTED",
	"SRM_REQUEST_QUEUED",
	"SRM_REQUEST_INPROGRESS",
	"SRM_REQUEST_SUSPENDED",
	"SRM_ABORTED",
	"SRM_RELEASED",
	"SRM_FILE_PINNED",
	"SRM_FILE_IN_CACHE",
	"SRM_SPACE_AVAILABLE",
	"SRM_LOWER_SPACE_GRANTED",
	"SRM_DONE",
	"SRM_PARTIAL_SUCCESS",
	"SRM_REQUEST_TIMED_OUT",
	"SRM_LAST_COPY",
	"SRM_FILE_BUSY",
	"SRM_FILE_LOST",
	"SRM_FILE_UNAVAILABLE",
	"SRM_CUSTOM_STATUS"
};
static int srm_timeout_connect = 60;
static int srm_timeout_sendreceive = 180;

int srm_get_timeout_connect ()
{
	return (srm_timeout_connect);
}

void srm_set_timeout_connect (int value)
{

	if (value >= 0)
		srm_timeout_connect = value;
}

int srm_get_timeout_sendreceive ()
{
	return (srm_timeout_sendreceive);
}

void srm_set_timeout_sendreceive (int value)
{
	if (value >= 0)
		srm_timeout_sendreceive = value;
}

void srm_context_init(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose)
{
    GFAL_SRM_IFCE_ASSERT(context);
	context->errbuf = errbuf;
	context->errbufsz = errbufsz;
	context->version = VERSION_2_2;
	context->srm_endpoint = srm_endpoint;
	context->timeout  = 60;
	context->verbose = verbose;
    context->timeout_conn = srm_get_timeout_connect ();
    context->timeout_ops = srm_get_timeout_sendreceive();
}

void back_off_logic_init(struct srm_context *context,struct srm_internal_context *internal_context)
{
    GFAL_SRM_IFCE_ASSERT(context);
    GFAL_SRM_IFCE_ASSERT(internal_context);

    if (context->timeout > 0)
	{
		internal_context->end_time = (time(NULL) + context->timeout);
	}
	internal_context->estimated_wait_time = -1;
	internal_context->attempt = 1;
    internal_context->random_seed = (unsigned int) time(NULL);
}
void set_estimated_wait_time(struct srm_internal_context *internal_context, int *my_time)
{
    if (my_time == NULL)
	{
		internal_context->estimated_wait_time = -1;
	}else
	{
        internal_context->estimated_wait_time = *my_time;
	}
}

void srm_soap_init(struct soap *soap)
{
	#ifdef GFAL_SECURE
	int flags;
	#endif
	soap_init (soap);
	soap->namespaces = namespaces_srmv2;

	#ifdef GFAL_SECURE
	flags = CGSI_OPT_DISABLE_NAME_CHECK;
	soap_register_plugin_arg (soap, client_cgsi_plugin, &flags);
	#endif

	soap->send_timeout =  srm_get_timeout_sendreceive ();
	soap->recv_timeout =  srm_get_timeout_sendreceive ();
	soap->connect_timeout = srm_get_timeout_connect ();
}

struct soap * srm_soap_init_new()
{
    return srm_soap_init_context_new(NULL);
}

struct soap * srm_soap_init_context_new(struct srm_context* c){
    #ifdef GFAL_SECURE
    int flags;
    #endif
    struct soap *soap_handle = soap_new();
    soap_init (soap_handle);
    soap_handle->namespaces = namespaces_srmv2;

    #ifdef GFAL_SECURE
    flags = CGSI_OPT_DISABLE_NAME_CHECK;
    soap_register_plugin_arg (soap_handle, client_cgsi_plugin, &flags);
    #endif

    if(c){
        soap_handle->recv_timeout= c->timeout_ops;
        soap_handle->send_timeout = c->timeout_ops;
        soap_handle->connect_timeout = c->timeout_conn;
    }else{
        soap_handle->send_timeout =  srm_get_timeout_sendreceive ();
        soap_handle->recv_timeout =  srm_get_timeout_sendreceive ();
        soap_handle->connect_timeout = srm_get_timeout_connect ();
    }
    return soap_handle;
}

void srm_soap_deinit(struct soap *soap)
{
	soap_end (soap);
	soap_done (soap);
}

void srm_soap_free(struct soap *soap)
{
	soap_end (soap);
	soap_done (soap);
	soap_free(soap);
}


const char * statuscode2errmsg (int statuscode)
{
	return (srmv2_errmsg[statuscode]);
}

int statuscode2errno (int statuscode)
{
	switch (statuscode) {
		case SRM_USCOREINVALID_USCOREPATH:
			return (ENOENT);
		case SRM_USCOREAUTHORIZATION_USCOREFAILURE:
			return (EACCES);
		case SRM_USCOREDUPLICATION_USCOREERROR:
			return (EEXIST);
		case SRM_USCORENO_USCOREFREE_USCORESPACE:
			return (ENOSPC);
		case SRM_USCOREINTERNAL_USCOREERROR:
			return (ECOMM);
		case SRM_USCORENOT_USCORESUPPORTED:
			return (EOPNOTSUPP);
		case SRM_USCORENON_USCOREEMPTY_USCOREDIRECTORY:
			return (ENOTEMPTY);
		case SRM_USCORESUCCESS:
		case SRM_USCOREFILE_USCOREPINNED:
		case SRM_USCORESPACE_USCOREAVAILABLE:
			return (0);
		default:
			return (EINVAL);
	}
}

void srm_errmsg (struct srm_context *context, const char *format, ...)
{
	va_list ap;
	char *actual_format;

	if ((context->errbuf == NULL && context->verbose == 0) || format == NULL || format[0] == 0)
		return;

	va_start (ap, format);
     (void)asprintf (&actual_format, "%s\n", format);
	if (actual_format == NULL) return;

	if (context->errbuf == NULL)
		vfprintf (stderr, actual_format, ap);
	else
		vsnprintf (context->errbuf, context->errbufsz, actual_format, ap);

	free (actual_format);
}
int srm_soap_call_err(struct srm_context *context,struct soap *soap,const char *srmfunc)
{
	if (soap->fault != NULL && soap->fault->faultstring != NULL)
	{
		srm_errmsg (context, "[%s][%s][] %s: %s",err_msg_begin,
				 srmfunc, context->srm_endpoint, soap->fault->faultstring);
	}else if (soap->error == SOAP_EOF)
	{
		srm_errmsg (context, "[%s][%s][] %s: Connection fails or timeout",
				err_msg_begin,srmfunc,context->srm_endpoint);
	}else
	{
		srm_errmsg (context, "[%s][%s][] %s: Unknown SOAP error (%d)",
				err_msg_begin,srmfunc,context->srm_endpoint, soap->error);
	}
	return ECOMM;
}
int srm_call_err(struct srm_context *context,struct srm2__TReturnStatus  *retstatus,
		const char *srmfunc)
{
	int result_errno;
	if (retstatus == NULL)
	{
		srm_errmsg (context, "[%s][%s][] %s: <empty response>",err_msg_begin,
				srmfunc,context->srm_endpoint);
		return ECOMM;
	}
	if (retstatus->statusCode != SRM_USCORESUCCESS &&
			retstatus->statusCode != SRM_USCOREPARTIAL_USCORESUCCESS &&
			retstatus->statusCode != SRM_USCOREDONE &&
			retstatus->statusCode != SRM_USCORETOO_USCOREMANY_USCORERESULTS)
	{
		result_errno = statuscode2errno (retstatus->statusCode);
		if (retstatus->explanation && retstatus->explanation[0])
		{
			srm_errmsg (context, "[%s][%s][%s] %s: %s",err_msg_begin,
					srmfunc, statuscode2errmsg (retstatus->statusCode),
					context->srm_endpoint, retstatus->explanation);
		}else
		{
			srm_errmsg (context, "[%s][%s][%s] %s: <none>",err_msg_begin,
					srmfunc, statuscode2errmsg (retstatus->statusCode),
					context->srm_endpoint);
		}
	}
	else
	{
		srm_errmsg (context, "[%s][%s][%s] %s: <empty response>",err_msg_begin,
				srmfunc, statuscode2errmsg (retstatus->statusCode),
				context->srm_endpoint);
		result_errno = ECOMM;
	}
	return result_errno;
}
// return error status
int srm_print_error_status(struct srm_context *context,struct srm2__TReturnStatus *status,const char *srmfunc)
{

	if (status->explanation && status->explanation[0])
	{
		srm_errmsg (context, "[%s][%s][%s] %s: %s",err_msg_begin,
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint, status->explanation);
	}else
	{	srm_errmsg (context, "[%s][%s][%s] %s: <none>",err_msg_begin,
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint);
	}
	return statuscode2errno (status->statusCode);
}
int srm_print_error_status_additional(struct srm_context *context,struct srm2__TReturnStatus *status,const char *srmfunc,char *additional_info)
{

	if (status->explanation && status->explanation[0])
	{
		srm_errmsg (context, "[%s][%s][%s] %s: %s: %s",err_msg_begin,
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint,additional_info, status->explanation);
	}else
	{	srm_errmsg (context, "[%s][%s][%s] %s: %s: <none>",err_msg_begin,
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint,additional_info);
	}
	return statuscode2errno (status->statusCode);
}
void srm_print_explanation(char **explanation,struct srm2__TReturnStatus *reqstatp,const char *srmfunc)
{
	if (reqstatp->explanation != NULL && reqstatp->explanation[0])
         (void)asprintf(explanation, "[%s][%s][%s] %s",err_msg_begin,
				srmfunc, statuscode2errmsg(
						reqstatp->statusCode), reqstatp->explanation);
	else
         (void)asprintf(explanation, "[%s][%s][%s] <none>",err_msg_begin,
				srmfunc, statuscode2errmsg(
						reqstatp->statusCode));
}
// Returns -1 for timeout
// Returns  0 for wait finished
int wait_for_new_attempt(struct srm_internal_context *internal_context)  // Or Timeout
{
    const time_t last_chance_sec_before_end = 2; // 2 seconds before the end
    int random_limit;
    int random_wait;
    time_t wait_till_end=0;
	time_t after_sleep;
	after_sleep = time(NULL) ;

    if (internal_context->estimated_wait_time <= 0)
	{
		// Exponential logic
		if (internal_context->attempt<=10) // Maximum 10 attempts
		{
            random_limit = (time_t) (1<<(internal_context->attempt));
            random_wait = (rand_r(&(internal_context->random_seed)) % random_limit);
			internal_context->attempt++;
            after_sleep += (time_t) random_wait;
			if (after_sleep >= internal_context->end_time)
			{
                wait_till_end = internal_context->end_time - time(NULL) -  last_chance_sec_before_end; //try the last hope before the end
				if (wait_till_end>0) 
					call_function.call_sleep(wait_till_end); 
                else // deadline outdated, do not sleep and try a last time
                    return 0; 					// simply return in timeout
			}else
			{
				call_function.call_sleep(random_wait);
			}
		}else
		{
			// Timeout, attempts exceeded
			return -1;
		}
    }else{
            after_sleep += internal_context->estimated_wait_time ;
            if (after_sleep >= internal_context->end_time)
            {
                wait_till_end = internal_context->end_time - (time(NULL)+last_chance_sec_before_end);
                if (wait_till_end>0)
                {
                    call_function.call_sleep(wait_till_end);
                }else
                {
                    // Timeout
                    return -1;
                }
            }else
            {
                call_function.call_sleep(internal_context->estimated_wait_time );
            }
    }

	return 0;
}
// Return all statuses timeout, failure,internal error, queued
srm_call_status back_off_logic(struct srm_context *context,const char *srmfunc,
		struct srm_internal_context *internal_context,struct srm2__TReturnStatus  *retstatus)
{

	if (retstatus == NULL)
	{
		srm_errmsg (context, "[%s][%s][] %s: <empty response>",err_msg_begin,
				srmfunc, context->srm_endpoint);
		errno = ECOMM;
		// No response status
		return srm_call_status_FAILURE;
	}else
	{
		switch (retstatus->statusCode )
		{
			case SRM_USCOREINTERNAL_USCOREERROR:
				if (wait_for_new_attempt(internal_context) != 0)
				{
					srm_errmsg (context, "[%s][%s][ETIMEDOUT] %s: User timeout over",
							err_msg_begin,srmfunc, context->srm_endpoint);
					errno = ETIMEDOUT;
					// Timeout
					return srm_call_status_TIMEOUT;
				}
				return srm_call_status_INTERNAL_ERROR;
			case SRM_USCOREREQUEST_USCOREQUEUED:
			case SRM_USCOREREQUEST_USCOREINPROGRESS:
				if (wait_for_new_attempt(internal_context) != 0)
				{
					srm_errmsg (context, "[%s][%s][ETIMEDOUT] %s: User timeout over",
							err_msg_begin,srmfunc, context->srm_endpoint);
					errno = ETIMEDOUT;
					// Timeout
					return srm_call_status_TIMEOUT;
				}
				return srm_call_status_QUEUED;
			case SRM_USCORESUCCESS:
			case SRM_USCOREPARTIAL_USCORESUCCESS: // TODO
				return srm_call_status_SUCCESS;
			default:
				return srm_call_status_FAILURE;
		}
	}
	return srm_call_status_FAILURE;
}
void srm_spacemd_free (int nbtokens,struct srm_spacemd *smd)
{
	int i;

	if (smd == NULL)
		return;

	for (i = 0; i < nbtokens; ++i) {
		if (smd[i].spacetoken) free (smd[i].spacetoken);
		if (smd[i].owner) free (smd[i].owner);
	}

	free (smd);
}
int srm_set_protocol_in_transferParameters(
	struct srm_context *context,
    struct soap* soap,
    struct srm2__TTransferParameters* transferParameters,
    char** protocols)
{
    struct srm2__ArrayOfString ** array = NULL;
    assert(transferParameters);
    array = &(transferParameters->arrayOfTransferProtocols);
    *array = NULL;

    if (protocols) {
        *array = soap_malloc (soap, sizeof(struct srm2__ArrayOfString));

        if (*array == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return -1;
        }

        (*array)->__sizestringArray = srm_count_elements_of_string_array(protocols);
        (*array)->stringArray = protocols;
    }

    return 0;
}
char* srm_strip_string(const char* str, const char chr)
{
    char *res = 0;

    assert(str);

    if (str) {
        char *pos = strrchr(str, chr);
        int size = -1;

        if (pos) {
            /* +1: to include the last character as well */
            size = pos - str + 1;
        } else {
            size = strlen(str);
        }

        res = (char*) malloc(size + 1);
        strncpy(res, str, size);
        res[size] = 0;
    }

    return res;
}


int srm_count_elements_of_string_array(char** a)
{
    int ret = -1;
    GFAL_SRM_IFCE_ASSERT(a);
    for (ret = 0; a[ret] != 0; ++ret) ;
    return ret;
}

int copy_string(char **dest,char *src)
{
    GFAL_SRM_IFCE_ASSERT(dest);
	if (src)
	{
        *dest = strdup(src);

		if (*dest == NULL)
		{
			errno = ENOMEM;
			return (-1);
		}
	}
    else
	{
        *dest = NULL;
        errno = EINVAL;
		// queued but empty token
		return (-1);
	}
	return 0;
}
int copy_permissionfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLPermissionReturn *repfs,
						const char *srmfunc,
						int amode)
{
	int i,n;

	n = repfs->__sizesurlPermissionArray;

	if ((*statuses = (struct srmv2_filestatus *) calloc (n, sizeof(struct srmv2_filestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}
	for (i = 0; i < n; i++) {
		if (!repfs->surlPermissionArray[i])
			continue;
		memset (*statuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->surlPermissionArray[i]->surl)
			(*statuses)[i].surl = strdup (repfs->surlPermissionArray[i]->surl);
		if (repfs->surlPermissionArray[i]->status) {
			(*statuses)[i].status = statuscode2errno (repfs->surlPermissionArray[i]->status->statusCode);
			if (repfs->surlPermissionArray[i]->status->explanation && repfs->surlPermissionArray[i]->status->explanation[0])
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->surlPermissionArray[i]->status->statusCode),
						repfs->surlPermissionArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->surlPermissionArray[i]->status->statusCode),
						reqstatp->explanation);
			else
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->surlPermissionArray[i]->status->statusCode));
		} else
			(*statuses)[i].status = ENOMEM;
        if ((*statuses)[i].status == 0) {
			enum srm2__TPermissionMode perm = *(repfs->surlPermissionArray[i]->permission);

			if ((amode == R_OK && (perm == NONE || perm == X || perm == W || perm == WX)) ||
					(amode == W_OK && (perm == NONE || perm == X || perm == R || perm == RX)) ||
					(amode == X_OK && (perm == NONE || perm == W || perm == R || perm == RW)) ||
					(amode == (R_OK|W_OK) && perm != RW && perm != RWX) ||
					(amode == (R_OK|X_OK) && perm != RX && perm != RWX) ||
					(amode == (W_OK|X_OK) && perm != WX && perm != RWX) ||
					(amode == (R_OK|W_OK|X_OK) && perm != RWX))
				(*statuses)[i].status = EACCES;
		}
	}
	return n;
}
int copy_filestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLReturnStatus *repfs,
						const char *srmfunc)
{
	int i,n;

	n = repfs->__sizestatusArray;

	if ((*statuses = (struct srmv2_filestatus *) calloc (n, sizeof(struct srmv2_filestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*statuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->statusArray[i]->surl)
			(*statuses)[i].surl = strdup (repfs->statusArray[i]->surl);
		if (repfs->statusArray[i]->status) {
			(*statuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else
                 (void)asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
	}
	return n;
}
int copy_pinfilestatuses_extendlifetime(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTSURLLifetimeReturnStatus *repfs,
						const char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->surl)
		{
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->surl);
		}
		if (repfs->statusArray[i]->pinLifetime)
		{
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->pinLifetime);
		}
		if (repfs->statusArray[i]->status)
		{
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
							 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
	}
	return n;
}
int copy_pinfilestatuses_get(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTGetRequestFileStatus *repfs,
						const char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);
		if (repfs->statusArray[i]->transferURL)
			(*filestatuses)[i].turl = strdup (repfs->statusArray[i]->transferURL);
		   if (repfs->statusArray[i]->status) {
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
		if (repfs->statusArray[i]->estimatedWaitTime)
		{
			(*filestatuses)[i].estimated_wait_time = *(repfs->statusArray[i]->estimatedWaitTime);
		}else
		{
			(*filestatuses)[i].estimated_wait_time = -1;
		}
	}
	return n;
}

int copy_pinfilestatuses_bringonline(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs,
						const char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);

		if (repfs->statusArray[i]->status)
		{
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
		if (repfs->statusArray[i]->estimatedWaitTime)
		{
			(*filestatuses)[i].estimated_wait_time = *(repfs->statusArray[i]->estimatedWaitTime);
		}else
		{
			(*filestatuses)[i].estimated_wait_time = -1;
		}
	}
	return n;
}

int copy_pinfilestatuses_put(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						const char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->SURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->SURL);
		if (repfs->statusArray[i]->transferURL)
			(*filestatuses)[i].turl = strdup (repfs->statusArray[i]->transferURL);

		if (repfs->statusArray[i]->status)
		{
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
                 (void) asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinLifetime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinLifetime);
		if (repfs->statusArray[i]->estimatedWaitTime)
		{
			(*filestatuses)[i].estimated_wait_time = *(repfs->statusArray[i]->estimatedWaitTime);
		}else
		{
			(*filestatuses)[i].estimated_wait_time = -1;
		}
	}
	return n;
}
int copy_filepermissions(struct srm2__TReturnStatus *reqstatp,
		struct srm_filepermission **permissions,
		struct srm2__ArrayOfTPermissionReturn *repperm)
{
	int i, n, j , k;
	const char srmfunc[] = "GetPermission";

	if (reqstatp == NULL || repperm == NULL || permissions == NULL)
	{
		errno = EFAULT;
		return (-1);
	}

	n = repperm->__sizepermissionArray;

	if ((*permissions = (struct srm_filepermission *) calloc (n, sizeof (struct srm_filepermission))) == NULL)
	{
		return (-1);
	}

	for (i = 0; i < n; ++i)
	{
		if (!repperm->permissionArray[i])
			continue;
		memset (*permissions + i, 0, sizeof(struct srm_filepermission));
		if (repperm->permissionArray[i]->owner)
		{
			(*permissions)[i].owner = strdup (repperm->permissionArray[i]->owner);
		}
		if (repperm->permissionArray[i]->surl)
		{
			(*permissions)[i].surl = strdup (repperm->permissionArray[i]->surl);
		}
		if (repperm->permissionArray[i]->status)
		{
			(*permissions)[i].status = statuscode2errno(repperm->permissionArray[i]->status->statusCode);
			if (repperm->permissionArray[i]->status->explanation && repperm->permissionArray[i]->status->explanation[0])
			{
				asprintf (&((*permissions)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg(repperm->permissionArray[i]->status->statusCode),
						repperm->permissionArray[i]->status->explanation);
			}else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
			{
				asprintf (&((*permissions)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg(repperm->permissionArray[i]->status->statusCode),
						reqstatp->explanation);
			}else
			{
				asprintf (&((*permissions)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						srmfunc, statuscode2errmsg(repperm->permissionArray[i]->status->statusCode));
			}
			//continue;
		}
		if (repperm->permissionArray[i]->ownerPermission)
		{
			(*permissions)[i].owner_permission = *repperm->permissionArray[i]->ownerPermission;
		}
		if (repperm->permissionArray[i]->otherPermission)
		{
			(*permissions)[i].other_permission = *repperm->permissionArray[i]->otherPermission;
		}
		if (repperm->permissionArray[i]->arrayOfGroupPermissions &&
				repperm->permissionArray[i]->arrayOfGroupPermissions->__sizegroupPermissionArray>0)
		{
			k = repperm->permissionArray[i]->arrayOfGroupPermissions->__sizegroupPermissionArray;
			(*permissions)[i].group_permissions_count = k;
			if (( (*permissions)[i].group_permissions = (struct srm_permission *) calloc (k, sizeof (struct srm_permission))) == NULL)
				return (-1);
			for (j = 0; j < k; ++j)
			{
				if (repperm->permissionArray[i]->arrayOfGroupPermissions->groupPermissionArray[j] == NULL)
					continue;

				(*permissions)[i].group_permissions[j].name_id = strdup (repperm->permissionArray[i]->arrayOfGroupPermissions->groupPermissionArray[j]->groupID);
				(*permissions)[i].group_permissions[j].mode = repperm->permissionArray[i]->arrayOfGroupPermissions->groupPermissionArray[j]->mode;
			}
		}else
		{
			(*permissions)[i].group_permissions_count = 0;
			(*permissions)[i].group_permissions = NULL;
		}
		if (repperm->permissionArray[i]->arrayOfUserPermissions &&
				repperm->permissionArray[i]->arrayOfUserPermissions->__sizeuserPermissionArray>0)
		{
			k = repperm->permissionArray[i]->arrayOfUserPermissions->__sizeuserPermissionArray;
			(*permissions)[i].user_permissions_count = k;
			if (( (*permissions)[i].user_permissions = (struct srm_permission *) calloc (k, sizeof (struct srm_permission))) == NULL)
				return (-1);
			for (j = 0; j < k; ++j)
			{
				if (repperm->permissionArray[i]->arrayOfUserPermissions->userPermissionArray[j] == NULL)
					continue;

				(*permissions)[i].user_permissions[j].name_id = strdup (repperm->permissionArray[i]->arrayOfUserPermissions->userPermissionArray[j]->userID);
				(*permissions)[i].user_permissions[j].mode = repperm->permissionArray[i]->arrayOfUserPermissions->userPermissionArray[j]->mode;
			}
		}else
		{
			(*permissions)[i].user_permissions_count = 0;
			(*permissions)[i].user_permissions = NULL;
		}
	}

	errno = 0;
	return (n);
}

/**
 * Copy TLocality (ONLINE, NEARLINE,...) from srm soap to srmifce enum
 */
void copy_Locality(struct srm2__TMetaDataPathDetail *soap_file_meta_data, struct srmv2_mdfilestatus *statuses){
	enum srm2__TFileLocality loc = 	*(soap_file_meta_data->fileLocality);
	TFileLocality res_loc;
	switch(loc){
		case ONLINE_:
			res_loc = GFAL_LOCALITY_ONLINE_;
			break;
		case NEARLINE_:
			res_loc = GFAL_LOCALITY_NEARLINE_;
			break;
		case ONLINE_USCOREAND_USCORENEARLINE:
			res_loc = GFAL_LOCALITY_ONLINE_USCOREAND_USCORENEARLINE;
			break;
		case LOST:
			res_loc = GFAL_LOCALITY_LOST;
			break;
		case UNAVAILABLE:
			res_loc = GFAL_LOCALITY_UNAVAILABLE;
			break;			
		default:
			res_loc = GFAL_LOCALITY_UNKNOWN;
	}
	statuses->locality = res_loc;
}

int copy_mdfilestatuses(struct srm2__TReturnStatus *reqstatp,
		struct srmv2_mdfilestatus **statuses,
		struct srm2__ArrayOfTMetaDataPathDetail *repfs)
{
	int i, n, r;
	const char srmfunc[] = "Ls";

	if (reqstatp == NULL || repfs == NULL || statuses == NULL)
	{
		errno = EFAULT;
		return (-1);
	}

	n = repfs->__sizepathDetailArray;

	if ((*statuses = (struct srmv2_mdfilestatus *) calloc (n, sizeof (struct srmv2_mdfilestatus))) == NULL)
	{
		return (-1);
	}

	for (i = 0; i < n; ++i)
	{
		if (!repfs->pathDetailArray[i])
			continue;
		memset (*statuses + i, 0, sizeof(struct srmv2_mdfilestatus));
		if (repfs->pathDetailArray[i]->path)
		{
			(*statuses)[i].surl = strdup (repfs->pathDetailArray[i]->path);
		}
		if (repfs->pathDetailArray[i]->status)
		{
			(*statuses)[i].status = statuscode2errno(repfs->pathDetailArray[i]->status->statusCode);
		}
		if ((*statuses)[i].status) {
			if (repfs->pathDetailArray[i]->status->explanation && repfs->pathDetailArray[i]->status->explanation[0])
			{
                (void) asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg(repfs->pathDetailArray[i]->status->statusCode),
						repfs->pathDetailArray[i]->status->explanation);
			}else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
			{
                 (void) asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] %s",err_msg_begin,
						srmfunc, statuscode2errmsg(repfs->pathDetailArray[i]->status->statusCode),
						reqstatp->explanation);
			}else
			{
                 (void) asprintf (&((*statuses)[i].explanation), "[%s][%s][%s] <none>",err_msg_begin,
						srmfunc, statuscode2errmsg(repfs->pathDetailArray[i]->status->statusCode));
			}

			continue;
		}

		if (repfs->pathDetailArray[i]->size)
		{
			(*statuses)[i].stat.st_size = *(repfs->pathDetailArray[i]->size);
		}else
		{
			(*statuses)[i].stat.st_size = 0;
		}
		if (repfs->pathDetailArray[i]->fileLocality)
		{
			copy_Locality(repfs->pathDetailArray[i], &((*statuses)[i]) );
        }
		(*statuses)[i].stat.st_uid = 2;//TODO: create haseh placeholder for string<->uid/gid mapping
		(*statuses)[i].stat.st_gid = 2;
		(*statuses)[i].stat.st_nlink = 1;

		if (repfs->pathDetailArray[i]->otherPermission)
		{
			(*statuses)[i].stat.st_mode = *(repfs->pathDetailArray[i]->otherPermission);
		}
		if (repfs->pathDetailArray[i]->groupPermission)
		{
			(*statuses)[i].stat.st_mode |= repfs->pathDetailArray[i]->groupPermission->mode << 3;
		}
		if (repfs->pathDetailArray[i]->ownerPermission)
		{
			(*statuses)[i].stat.st_mode |= repfs->pathDetailArray[i]->ownerPermission->mode << 6;
		}
		if (repfs->pathDetailArray[i]->type)
		{
			switch (*(repfs->pathDetailArray[i]->type))
			{
				case FILE_:
					(*statuses)[i].stat.st_mode |= S_IFREG;
					break;
				case DIRECTORY:
					(*statuses)[i].stat.st_mode |= S_IFDIR;
					break;
				case LINK:
					(*statuses)[i].stat.st_mode |= S_IFLNK;
					break;
			}
		}
		if (repfs->pathDetailArray[i]->checkSumType && repfs->pathDetailArray[i]->checkSumValue)
		{
			(*statuses)[i].checksumtype = strdup (repfs->pathDetailArray[i]->checkSumType);
			(*statuses)[i].checksum = strdup (repfs->pathDetailArray[i]->checkSumValue);
		}
		if (repfs->pathDetailArray[i]->arrayOfSpaceTokens &&
                repfs->pathDetailArray[i]->arrayOfSpaceTokens->__sizestringArray > 0 &&
                repfs->pathDetailArray[i]->arrayOfSpaceTokens->stringArray)
		{
            int j;
            (*statuses)[i].nbspacetokens = repfs->pathDetailArray[i]->arrayOfSpaceTokens->__sizestringArray;

            if (((*statuses)[i].spacetokens = (char **) calloc ((*statuses)[i].nbspacetokens, sizeof (char *))) == NULL)
                return (-1);
            for (j = 0; j < (*statuses)[i].nbspacetokens; ++j)
            {
                if (repfs->pathDetailArray[i]->arrayOfSpaceTokens->stringArray[j] == NULL)
                    continue;
                (*statuses)[i].spacetokens[j] = strdup (repfs->pathDetailArray[i]->arrayOfSpaceTokens->stringArray[j]);
            }
        }

		if (repfs->pathDetailArray[i]->arrayOfSubPaths)
		{
			r = copy_mdfilestatuses (reqstatp, &((*statuses)[i].subpaths), repfs->pathDetailArray[i]->arrayOfSubPaths );

			if (r < 0)
				return (r);

			(*statuses)[i].nbsubpaths = r;
		}
	}

    errno = 0;
	return (n);
}


int copy_returnstatus(struct srm2__TReturnStatus **destination,struct srm2__TReturnStatus *returnStatus)
{
	if (returnStatus)
	{
        GFAL_SRM_IFCE_ASSERT(destination);
		*destination = (struct srm2__TReturnStatus *) malloc (sizeof (struct srm2__TReturnStatus));
		if ((*destination) != NULL)
		{
			(**destination).explanation = NULL;
			if (returnStatus->explanation)
			{
				if (((**destination).explanation = strdup(returnStatus->explanation)) == NULL)
				{
					errno = ENOMEM;
					return (-1);
				}
			}
			(**destination).statusCode = returnStatus->statusCode;
			return 0;
		}else
		{
			errno = ENOMEM;
			return (-1);
		}
	}else
	{
		return -1; // empty response
	}
	return 0;
}

char* srm_util_consolidate_multiple_characters(const char* s, const char c, const int start)
{
    char *tmp = 0;
    char *ret = 0;
    int i = 0;
    int tmp_i = 0;

    if (s == 0) {
        return 0;
    }    

    tmp = (char*) malloc (strlen(s) + 1);

    for (; s[i] != 0; ++i) {
        // Copy the characters unless we find c. If the index + 1 also stores c, do not copy,
        if (i < start || s[i] != c || s[i + 1] != c) {
            tmp[tmp_i] = s[i];
            ++tmp_i;
        } 
    } 
    
    tmp[tmp_i] = 0;
    // strdup the string, to shrink to the real size
    ret = strdup(tmp);    
    free(tmp);
    return ret;
}


char* srm_util_add_strings(const char* s1, const char* s2)
{
    char* ret = 0;
    unsigned int len_s1 = 0;

    assert(s1);
    assert(s2);
   
    if (!s1 || !s2) {
        return 0;
    }

    len_s1 = strlen(s1);
    ret = malloc(len_s1 + strlen(s2) + 1);
    assert(ret);
   
    if (ret) {
        strcpy(ret, s1);
        strcat(ret + len_s1, s2);
    }

    return ret;
}


char* srm_util_normalize_surl(const char* surl)
{
    char *consolidated_file = NULL;
    char *with_trailing_slash = NULL;
    /* We put a trailing "/" to the end of each directory, fo fix the algorithm in srmv2_makedirp
       (Savannah bug #52502) */
    with_trailing_slash = srm_util_add_strings(surl, "/");
    assert(with_trailing_slash);
    /* do not consolidate the "//" after the protocol part... (srm://...) */
    consolidated_file = srm_util_consolidate_multiple_characters(
        with_trailing_slash, '/', strlen("srm://") + 1);
    assert(consolidated_file);
    free(with_trailing_slash);

    return consolidated_file;
}

