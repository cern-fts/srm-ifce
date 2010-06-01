#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "srm_soap.h"
#include "srm_types.h"
#include "srmv2H.h"
#include "srm_dependencies.h"

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

void back_off_logic_init(struct srm_context *context,struct srm_internal_context *internal_context)
{
	if (context->timeout > 0)
	{
		internal_context->end_time = (time(NULL) + context->timeout);
	}
	internal_context->estimated_wait_time = -1;
	internal_context->attempt = 1;
}
void set_estimated_wait_time(struct srm_internal_context *internal_context, int *time)
{
	if (time == NULL)
	{
		internal_context->estimated_wait_time = -1;
	}else
	{
		internal_context->estimated_wait_time = *time;
	}
}

void srm_soap_init(struct soap *soap)
{
	int flags;
	soap_init (soap);
	soap->namespaces = namespaces_srmv2;

	#ifdef GFAL_SECURE
	flags = CGSI_OPT_DISABLE_NAME_CHECK;
	soap_register_plugin_arg (soap, client_cgsi_plugin, &flags);
	#endif

	soap->send_timeout = 1000;//gfal_get_timeout_sendreceive ();
	soap->recv_timeout = 1000;//gfal_get_timeout_sendreceive ();
	soap->connect_timeout = 1000;//gfal_get_timeout_connect ();
}
void srm_soap_deinit(struct soap *soap)
{
	soap_end (soap);
	soap_done (soap);
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
	asprintf (&actual_format, "%s\n", format);
	if (actual_format == NULL) return;

	if (context->errbuf == NULL)
		vfprintf (stderr, actual_format, ap);
	else
		vsnprintf (context->errbuf, context->errbufsz, actual_format, ap);

	free (actual_format);
}
int srm_soup_call_err(struct srm_context *context,struct soap *soap,const char *srmfunc)
{
	if (soap->fault != NULL && soap->fault->faultstring != NULL)
	{
		srm_errmsg (context, "[SE][%s][] %s: %s",
				 srmfunc, context->srm_endpoint, soap->fault->faultstring);
	}else if (soap->error == SOAP_EOF)
	{
		srm_errmsg (context, "[SE][%s][] %s: Connection fails or timeout",
				 srmfunc,context->srm_endpoint);
	}else
	{
		srm_errmsg (context, "[SE][%s][] %s: Unknown SOAP error (%d)",
				 srmfunc,context->srm_endpoint, soap->error);
	}
	return ECOMM;
}
int srm_call_err(struct srm_context *context,struct srm2__TReturnStatus  *retstatus,
		const char *srmfunc)
{
	int result_errno;
	if (retstatus == NULL)
	{
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
			srm_errmsg (context, "[SE][%s][%s] %s: %s",
					srmfunc, statuscode2errmsg (retstatus->statusCode),
					context->srm_endpoint, retstatus->explanation);
		}else
		{
			srm_errmsg (context, "[SE][%s][%s] %s: <none>",
					srmfunc, statuscode2errmsg (retstatus->statusCode), context->srm_endpoint);
		}
	}
	else
	{
		srm_errmsg (context, "[SE][%s][%s] %s: <empty response>",
				srmfunc, statuscode2errmsg (retstatus->statusCode), context->srm_endpoint);
		result_errno = ECOMM;
	}
	return result_errno;
}
// return error status
int srm_print_error_status(struct srm_context *context,struct srm2__TReturnStatus *status,char *srmfunc)
{

	if (status->explanation && status->explanation[0])
	{
		srm_errmsg (context, "[SE][%s][%s] %s: %s",
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint, status->explanation);
	}else
	{	srm_errmsg (context, "[%s][%s][%s] %s: <none>",
				 srmfunc, statuscode2errmsg (status->statusCode),
				 context->srm_endpoint);
	}
	return statuscode2errno (status->statusCode);
}
void srm_print_explanation(char **explanation,struct srm2__TReturnStatus *reqstatp,const char *srmfunc)
{
	if (reqstatp->explanation != NULL && reqstatp->explanation[0])
		asprintf(explanation, "[SE][%s][%s] %s",
				srmfunc, statuscode2errmsg(
						reqstatp->statusCode), reqstatp->explanation);
	else
		asprintf(explanation, "[SE][%s][%s] <none>",
				srmfunc, statuscode2errmsg(
						reqstatp->statusCode));
}
// Returns -1 for timeout
// Returns  0 for wait finished
int wait_for_new_attempt(struct srm_internal_context *internal_context)  // Or Timeout
{
	int random_limit;
	int random_wait;
	time_t after_sleep;
	after_sleep = time(NULL) ;
	srand( time(NULL) ); // new seed

	if (internal_context->estimated_wait_time <= -1)
	{
		// Exponential logic
		if (internal_context->attempt<=10) // Maximum 10 attempts
		{
			random_limit = (1<<(internal_context->attempt));
			random_wait = (rand() % random_limit);
			internal_context->attempt++;
			after_sleep += random_wait;
			if (after_sleep > internal_context->end_time)
			{
				// Timeout
				return -1;
			}
			call_function.call_sleep(random_wait);
			//call_function.call_sleep(random_wait);
		}else
		{
			// Timeout, attempts exceeded
			return -1;
		}
	}else
	if (internal_context->estimated_wait_time == 0)
	{
		// Timeout
		return -1;
	}else
	{
		after_sleep += internal_context->estimated_wait_time ;
		if (after_sleep > internal_context->end_time)
		{
			// Timeout
			return -1;
		}
		call_function.call_sleep(internal_context->estimated_wait_time );
	}


	return 0;
}
// Return all statuses timeout, failure,internal error, queued
srm_call_status back_off_logic(struct srm_context *context,const char *srmfunc,
		struct srm_internal_context *internal_context,struct srm2__TReturnStatus  *retstatus)
{

	if (retstatus == NULL)
	{
		srm_errmsg (context, "[SE][%s][] %s: <empty response>",
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
					srm_errmsg (context, "[SE][%s][ETIMEDOUT] %s: User timeout over",
							srmfunc, context->srm_endpoint);
					errno = ETIMEDOUT;
					// Timeout
					return srm_call_status_TIMEOUT;
				}
				return srm_call_status_INTERNAL_ERROR;
			case SRM_USCOREREQUEST_USCOREQUEUED:
			case SRM_USCOREREQUEST_USCOREINPROGRESS:
				if (wait_for_new_attempt(internal_context) != 0)
				{
					srm_errmsg (context, "[SE][%s][ETIMEDOUT] %s: User timeout over",
								srmfunc, context->srm_endpoint);
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
void srm_spacemd_free (int nbtokens, srm_spacemd *smd)
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
    assert(a);
    for (ret = 0; a[ret] != 0; ++ret) ;
    return ret;
}

int copy_string(char **dest,char *src)
{
	if (src)
	{
		if ((*dest	 = strdup (src)) == NULL)
		{
			errno = ENOMEM;
			return (-1);
		}
	}else
	{
		errno = EINVAL;
		// queued but empty token
		return (-1);
	}
	return 0;
}
int copy_filestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLReturnStatus *repfs,
						char *srmfunc)
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
				asprintf (&((*statuses)[i].explanation), "[SRM][%s][%s] %s",
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*statuses)[i].explanation), "[SRM][%s][%s] %s",
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else
				asprintf (&((*statuses)[i].explanation), "[SRM][%s][%s] <none>",
						srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
	}
	return n;
}
int copy_pinfilestatuses_get(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTGetRequestFileStatus *repfs,
						char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		//srm_soap_deinit(&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);
		if (repfs->statusArray[i]->transferURL)
			(*filestatuses)[i].turl = strdup (repfs->statusArray[i]->transferURL);
		   if (repfs->statusArray[i]->status) {
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] <none>",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
	}
	return n;
}

int copy_pinfilestatuses_bringonline(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs,
						char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		//srm_soap_deinit(&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);

		if (repfs->statusArray[i]->status)
		{
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] <none>",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
	}
	return n;
}

int copy_pinfilestatuses_put(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						char *srmfunc)
{
	int n,i;
	n = repfs->__sizestatusArray;
	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL)
	{
		//srm_soap_deinit(&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++)
	{
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->statusArray[i]->SURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->SURL);
		if (repfs->statusArray[i]->transferURL)
			(*filestatuses)[i].turl = strdup (repfs->statusArray[i]->transferURL);

		if (repfs->statusArray[i]->status)
		{
			(*filestatuses)[i].status = statuscode2errno (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] <none>",
						 srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinLifetime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinLifetime);
	}
	return n;
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
		memset (*statuses + i, 0, sizeof(struct srm_ls_output));
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
				asprintf (&((*statuses)[i].explanation), "[SE][%s][%s] %s",
						srmfunc, statuscode2errmsg(repfs->pathDetailArray[i]->status->statusCode),
						repfs->pathDetailArray[i]->status->explanation);
			}else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
			{
				asprintf (&((*statuses)[i].explanation), "[SE][%s][%s] %s",
						srmfunc, statuscode2errmsg(repfs->pathDetailArray[i]->status->statusCode),
						reqstatp->explanation);
			}else
			{
				asprintf (&((*statuses)[i].explanation), "[SE][%s][%s] <none>",
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
			(*statuses)[i].locality = *(repfs->pathDetailArray[i]->fileLocality); // TODO check this line
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
		*destination = (struct srm2__TReturnStatus *) malloc (sizeof (struct srm2__TReturnStatus));
		if ((*destination) != NULL)
		{
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
	}
	return 0;
}
