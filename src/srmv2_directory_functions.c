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
#include <errno.h>
#include <regex.h>
#include <assert.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"

// Utility functions
int srmv2_check_srm_root(const char* surl);

int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output);
int srmv2_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output);
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input);

// Asynchronous srm ls call
void srm_ls_output_destroy(struct srm_ls_output *output)
{
    if (output == NULL)
    {
        return;
    }

    srm_srm2__TReturnStatus_delete(output->retstatus);
    output->retstatus = NULL;

    free(output->token);
    output->token = NULL;

    srm_srmv2_mdfilestatus_delete(output->statuses, output->statuses_num);
    output->statuses = NULL;
}

int srmv2_ls_async_internal(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output,struct srm_internal_context *internal_context)
{
	int ret;
	enum xsd__boolean trueoption = true_;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "Ls";
	struct srm2__srmLsRequest req;
	struct srm2__srmLsResponse_ rep;
	struct srm2__ArrayOfTMetaDataPathDetail *repfs = NULL;

    /* Basic sanity checks */
    if (input->offset && *input->offset < 0)
    {
        errno = EINVAL;
        srm_errmsg (context, "[SRM][srmv2_ls_async_internal][] Negative offset value is illegal.");
        return -1;
    }

    if (input->count < 0)
    {
        errno = EINVAL;
        srm_errmsg (context, "[SRM][srmv2_ls_async_internal][] Negative count value is illegal.");
        return -1;
    }

	memset(output,0,sizeof(*output));
	memset (&req, 0, sizeof(req));

	if ((req.arrayOfSURLs = soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.fullDetailedList = &trueoption;
	req.numOfLevels = &(input->numlevels);

	if (input->offset && *input->offset > 0) 
    {
		req.offset = input->offset;
	}

    if (input->count > 0)
    {
        req.count = &(input->count);
    }

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **)input->surls;

	do
	{
		// Gsoap call soap_call_srm2__srmLs
		ret = call_function.call_srm2__srmLs(soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmLsResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmLsResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);



	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if (copy_string(&output->token,rep.srmLsResponse->requestToken))
			{
				internal_context->current_status = srm_call_status_FAILURE;
				srm_soap_free(soap);
				return -1;
			}
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			// Copy file structure to another pointer for easier manipulation
			repfs = rep.srmLsResponse->details;
			// Check if file structure ok
			if (!repfs || repfs->__sizepathDetailArray <= 0 || !repfs->pathDetailArray)
			{
				// file list empty error
				internal_context->current_status  = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				internal_context->current_status = srm_call_status_SUCCESS;
				// Everything is fine copy file structure and check if copy went ok
				ret = copy_mdfilestatuses(output->retstatus, &output->statuses,repfs);

				if (ret == -1)
				{
					errno = srm_call_err(context,output->retstatus,srmfunc);
					internal_context->current_status  = srm_call_status_FAILURE;
				}else
				{
                    output->statuses_num = repfs->__sizepathDetailArray;
					
                    if (ret == 1 && input->offset && output->retstatus->statusCode == SRM_USCORETOO_USCOREMANY_USCORERESULTS &&
							repfs->pathDetailArray[0] != NULL && repfs->pathDetailArray[0]->arrayOfSubPaths != NULL)
					{
						// offset is only supported for a single directory listing
						*input->offset += repfs->pathDetailArray[0]->arrayOfSubPaths->__sizepathDetailArray;
					}else if (input->offset)
					{
						*input->offset = 0;
					}
				}
			}
			break;
		default:
			errno = srm_call_err(context,output->retstatus,srmfunc);
			ret = -1;
			break;
	}

	srm_soap_free(soap);
	return ret;
}

int srmv2_status_of_ls_request_async_internal(struct srm_context *context,
		struct srm_ls_input *input,
		struct srm_ls_output *output,
		struct srm_internal_context *internal_context)
{
	const char srmfunc[] = "StatusOfLsRequest";
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmStatusOfLsRequestResponse_ srep;
	struct srm2__srmStatusOfLsRequestRequest sreq;
	struct srm2__ArrayOfTMetaDataPathDetail *repfs = NULL;
	int ret;

	// wait for files ready

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;
	output->retstatus = NULL;
	output->statuses = NULL;

	internal_context->current_status = srm_call_status_FAILURE;

	do
	{
		ret = call_function.call_srm2__srmStatusOfLsRequest (soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfLsRequestResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,srep.srmStatusOfLsRequestResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus );

		repfs = srep.srmStatusOfLsRequestResponse->details;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	// If success
	switch (internal_context->current_status)
	{
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			// Check if file structure ok
			if (!repfs || repfs->__sizepathDetailArray <= 0 || !repfs->pathDetailArray)
			{
				// file list empty error
				errno = srm_call_err(context,output->retstatus,srmfunc);
				internal_context->current_status  = srm_call_status_FAILURE;
				ret = -1;
			}else
			{
				internal_context->current_status = srm_call_status_SUCCESS;
				// Everything is fine copy file structure and check if copy went ok
				ret = copy_mdfilestatuses(output->retstatus, &output->statuses,repfs );
				if (ret == -1)
				{
					errno = srm_call_err(context,output->retstatus,srmfunc);
					internal_context->current_status  = srm_call_status_FAILURE;
				}else
				{
                    output->statuses_num = repfs->__sizepathDetailArray;
					if (ret == 1 && input->offset && output->retstatus->statusCode == SRM_USCORETOO_USCOREMANY_USCORERESULTS &&
							repfs->pathDetailArray[0] != NULL && repfs->pathDetailArray[0]->arrayOfSubPaths != NULL)
					{
						// offset is only supported for a single directory listing
						*input->offset += repfs->pathDetailArray[0]->arrayOfSubPaths->__sizepathDetailArray;
					}else if (input->offset)
					{
						*input->offset = 0;
					}
				}
			}
			break;
		case srm_call_status_QUEUED:
			break;
		default:
			errno = srm_call_err(context,output->retstatus,srmfunc);
			ret = -1;
			break;
	}

	srm_soap_free(soap);
	return (ret);
}



int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output)
{
	struct srm2__srmRmResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmRmRequest req;
	//struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "srmRm";
	struct srm_internal_context internal_context;
	int i,n,ret;

	back_off_logic_init(context,&internal_context);

	

	memset (&req, 0, sizeof(req));
	memset(output,0,sizeof(*output));


	// NOTE: only one file in the array
	if ((req.arrayOfSURLs = soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	// issue "srmRm" request
	do
	{
		// Gsoap call
		ret = call_function.call_srm2__srmRm (soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmRmResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmRmResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,output->retstatus);

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	repfs = rep.srmRmResponse->arrayOfFileStatuses;

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray) {
		errno = srm_call_err(context,output->retstatus,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((output->statuses = (struct srmv2_filestatus*) calloc (n, sizeof (struct srmv2_filestatus))) == NULL) {
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	for (i = 0; i < n; ++i) {
		if (!repfs->statusArray[i])
			continue;
		if (repfs->statusArray[i]->surl)
			(output->statuses)[i].surl = strdup (repfs->statusArray[i]->surl);
		if (repfs->statusArray[i]->status)
		{
			(output->statuses)[i].status = statuscode2errno(repfs->statusArray[i]->status->statusCode);
			srm_print_explanation(&((output->statuses)[i].explanation), repfs->statusArray[i]->status,srmfunc);
		}
	}

	srm_soap_free(soap);
    errno = 0;
	return (n);
}
//srmv2_rmdir (const char *surl, const char *srm_endpoint, int recursive,
//struct srmv2_filestatus **statuses, char *errbuf, int errbufsz, int timeout)
int srmv2_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output)
{
	int flags;
	int ret;
	struct srm2__srmRmdirResponse_ rep;
	struct srm2__srmRmdirRequest req;
	enum xsd__boolean trueoption = true_;
	struct srm_internal_context internal_context;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "srmRmdir";

	back_off_logic_init(context,&internal_context);

	

	memset (&req, 0, sizeof(req));
	memset(output,0,sizeof(*output));

	req.SURL = (char *) input->surl;
	if (input->recursive)
	{
		req.recursive = &trueoption;
	}

	// issue "srmRmdir" request
	do
	{
		// Gsoap call
		ret = call_function.call_srm2__srmRmdir(soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmRmdirResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmRmdirResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,output->retstatus);

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if ((output->statuses = (struct srmv2_filestatus*) calloc (1, sizeof (struct srmv2_filestatus))) == NULL) {
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}
	output->statuses[0].surl = strdup (input->surl);
	output->statuses[0].status = statuscode2errno (output->retstatus->statusCode);
	if (output->statuses[0].status)
	{
		srm_print_explanation(&((output->statuses)[0].explanation),output->retstatus,srmfunc);
	}
	srm_soap_free(soap);
	errno = 0;
	return (1); //deleted one folder
}

/* tries to create all directories in 'dest_file' */
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input)
{
	char* file = NULL;
    int ret = -1;
	int flags;
	int sav_errno = 0;
	char *p, *endp;
	struct srm2__srmMkdirResponse_ rep;
	struct srm2__srmMkdirRequest req;
	struct srm2__TReturnStatus *repstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "Mkdir";

	
	memset (&req, 0, sizeof (struct srm2__srmMkdirRequest));
	memset (&rep, 0, sizeof (struct srm2__srmMkdirResponse_));
    file = srm_util_normalize_surl(input->dir_name);
    p = endp = strrchr (file, '/');

	// 1st cycle, trying to create directories ascendingly, until success
	do {
        /* Do not try to create the root directory... */
        if (srmv2_check_srm_root(file)) {
            break;
        }   

		*p = 0;
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir (soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
            goto CLEANUP_AND_RETURN; 
		}

		repstatp = rep.srmMkdirResponse->returnStatus;
		sav_errno = statuscode2errno (repstatp->statusCode);

		if (sav_errno != 0 && sav_errno != EEXIST && sav_errno != ENOENT)
		{
			srm_print_error_status_additional(context,repstatp,srmfunc,input->dir_name);
			errno = sav_errno;
            goto CLEANUP_AND_RETURN; 
		}
	} while (sav_errno == ENOENT && (p = strrchr (file, '/')) != NULL);

	if (p == NULL) {
		// should never happen, failure must appear in soap call
		srm_errmsg (context, "[SRM][srmv2_makedirp][EINVAL] %s: Invalid SURL", input->dir_name);
		errno = EINVAL;
	    goto CLEANUP_AND_RETURN; 
	}

	// 2nd cycle, creating directories descendingly as of the one created by previous cycle
	*p = '/';
	sav_errno = 0;
	while (sav_errno == 0 && p < endp && (p = strchr (p + 1, 0)) != NULL)
	{
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir(soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			errno = ECOMM;
            goto CLEANUP_AND_RETURN;
		}

		repstatp = NULL;
		if (rep.srmMkdirResponse == NULL || (repstatp = rep.srmMkdirResponse->returnStatus) == NULL
				|| statuscode2errno (repstatp->statusCode) != 0)

		{
			errno = srm_call_err(context,repstatp,srmfunc);
            goto CLEANUP_AND_RETURN; 
		}

		*p = '/';
	}

    errno = 0;
    ret = 0;

CLEANUP_AND_RETURN:
    free(file);
    srm_soap_free(soap);
    return ret;
}

int srmv2_extend_file_lifetime(struct srm_context *context,
		struct srm_extendfilelifetime_input *input,
		struct srm_extendfilelifetime_output *output)
{
	int ret;
	struct srm2__ArrayOfTSURLLifetimeReturnStatus *repfs;
	struct srm2__srmExtendFileLifeTimeResponse_ rep;
	struct srm2__srmExtendFileLifeTimeRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	int i = 0;
	const char srmfunc[] = "ExtendFileLifeTime";

	

	/* issue "extendfilelifetime" request */

	memset (&req, 0, sizeof(req));
	memset(output,0,sizeof(*output));

	if ((req.arrayOfSURLs = soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.authorizationID = NULL;
	req.requestToken = input->reqtoken;
	req.newFileLifeTime = NULL;
	req.newPinLifeTime = &input->pintime;
	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = input->surls;

	if ((ret = call_function.call_srm2__srmExtendFileLifeTime (soap, context->srm_endpoint, srmfunc, &req, &rep)))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}
	if ((rep.srmExtendFileLifeTimeResponse == NULL)||(ret!=0)||
			copy_returnstatus(&output->retstatus,rep.srmExtendFileLifeTimeResponse->returnStatus))
	{
		errno = srm_call_err(context,output->retstatus,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	/* return file statuses */
	repfs = rep.srmExtendFileLifeTimeResponse->arrayOfFileStatuses;

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,output->retstatus,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	errno = 0;
	ret = copy_pinfilestatuses_extendlifetime(output->retstatus,
							&output->filestatuses,
							repfs,
							srmfunc);

	srm_soap_free(soap);
	return (ret);
}

int srmv2_check_srm_root(const char* surl)
{
    int ret = 0;
    static regex_t re;
    static is_compiled = 0;
    static const char* regexp = "^srm\://[^/]*/$";
    #define SRMV1_CHECK_SRM_ROOT_NMATCH 1
    regmatch_t match[SRMV1_CHECK_SRM_ROOT_NMATCH];

    if (surl == NULL) {
        return 0;
    }

    if (!is_compiled) {
        int comp_res = regcomp(&re, regexp, REG_ICASE);
        assert(comp_res == 0);
        is_compiled = 1;
    }
   
    if (0 == regexec(&re, surl, SRMV1_CHECK_SRM_ROOT_NMATCH, match, 0)) {
        ret = 1;
    }

    return ret;
    #undef SRMV1_CHECK_SRM_ROOT_NMATCH
}

