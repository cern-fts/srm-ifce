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
#include <assert.h>
#include "gfal_srm_ifce.h"
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"



int srmv2_status_of_put_request_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_prepare_to_put_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output,
		struct srm_internal_context *internal_context);

int srmv2_prepare_to_get_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_get_request_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);

int srmv2_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);



int srmv2_status_of_put_request_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	int ret = 0;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmStatusOfPutRequestResponse_ srep;
	struct srm2__srmStatusOfPutRequestRequest sreq;
	const char srmfunc[] = "StatusOfPutRequest";

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;
	output->retstatus = NULL;
	output->filestatuses = NULL;

	do
	{
		ret = call_function.call_srm2__srmStatusOfPutRequest(soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfPutRequestResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,srep.srmStatusOfPutRequestResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		set_estimated_wait_time(internal_context, srep.srmStatusOfPutRequestResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	if (output->retstatus->statusCode == SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED) {
		srm_errmsg (context,"[SRM][%s][%s] %s: Space lifetime expired",srmfunc, statuscode2errmsg(output->retstatus->statusCode), context->srm_endpoint);
		errno = statuscode2errno(output->retstatus->statusCode);
		srm_soap_free(soap);
		return (-1);
	}

	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				errno = 0;
				internal_context->current_status = srm_call_status_SUCCESS;
				ret = copy_pinfilestatuses_put(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
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



int srmv2_prepare_to_put_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output,
		struct srm_internal_context *internal_context)
{
	int i;
	int n;
	int ret = 0;
	struct srm2__srmPrepareToPutResponse_ rep;
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	struct srm2__srmPrepareToPutRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	char *targetspacetoken;
	const char srmfunc[] = "PrepareToPut";
	struct srm_getbestspacetokens_input spacetokeninput;
	SRM_LONG64 totalsize=0;
	struct srm_mkdir_input mkdirinput;


	memset (&req, 0, sizeof(req));
	memset (&rep, 0, sizeof(rep));
	memset(output,0,sizeof(*output));

	if ((req.arrayOfFileRequests =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfTPutFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (soap, input->nbfiles* sizeof(struct srm2__TPutFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	if (input->desiredpintime > 0)
	{
		req.desiredPinLifeTime = &input->desiredpintime;
	}
	if (context->timeout > 0)
	{
		req.desiredTotalRequestTime = &context->timeout ;
	}

	req.desiredFileStorageType = &s_types[PERMANENT];
	req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

	if (input->filesizes == NULL &&  input->nbfiles>0)
	{
		errno = EINVAL;
		srm_soap_free(soap);
		return (-1);
	}

	for (i = 0; i < input->nbfiles; i++) {
		if ((req.arrayOfFileRequests->requestArray[i] =
					soap_malloc (soap, sizeof(struct srm2__TPutFileRequest))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_free(soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TPutFileRequest));
		req.arrayOfFileRequests->requestArray[i]->targetSURL = (char *)input->surls[i];

		if ((req.arrayOfFileRequests->requestArray[i]->expectedFileSize = soap_malloc (soap, sizeof(ULONG64))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_free(soap);
			return (-1);
		}
		*(req.arrayOfFileRequests->requestArray[i]->expectedFileSize) = input->filesizes[i];
		totalsize += input->filesizes[i]; // compute total size to determine best space token
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;
	req.transferParameters->arrayOfTransferProtocols = NULL;

	ret = srm_set_protocol_in_transferParameters(context,soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_free(soap);
		return (-1);
	}


	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
	{

			spacetokeninput.neededsize = totalsize;
			spacetokeninput.spacetokendesc = input->spacetokendesc;
			targetspacetoken = srm_getbestspacetoken (context,&spacetokeninput);
			if (targetspacetoken != NULL)
			{
				req.targetSpaceToken = targetspacetoken;
			} else
			{
				srm_soap_free(soap);
				return (-1);
			}
	}

	// Create sub-directories of SURLs - NOT NEEDED for srmv2.2
	/*for (i = 0; i < input->nbfiles; ++i)
	{

		const char* dir = srm_strip_string(input->surls[i], '/');
		int res = 0;

		assert (dir);

		if (dir && strlen(dir) > 0)
		{
			mkdirinput.dir_name = dir;
			res = srmv2_mkdir (context,mkdirinput);
		}

		free(dir);

		if (res < 0)
		{
			srm_soap_deinit(&soap);
			return (-1);
		}
	}*/

	do
	{
		ret = call_function.call_srm2__srmPrepareToPut (soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPrepareToPutResponse== NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmPrepareToPutResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		set_estimated_wait_time(internal_context, rep.srmPrepareToPutResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = rep.srmPrepareToPutResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if (copy_string(&output->token,rep.srmPrepareToPutResponse->requestToken))
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			if (!repfs ||
                repfs->__sizestatusArray < 1 ||
                !repfs->statusArray ||
                internal_context->current_status == srm_call_status_FAILURE)
			{
			    internal_context->current_status = srm_call_status_FAILURE;

			    if (internal_context->current_status == srm_call_status_FAILURE)
			    {
                    errno = srm_call_err(context,output->retstatus,srmfunc);
                    ret = -1;
			    }
			    else
			    {
                    srm_call_err(context, NULL, srmfunc);
                    errno = ECOMM;
			    }
			}else{
				if ( output->token == NULL && copy_string(&output->token,rep.srmPrepareToPutResponse->requestToken))
				{
					internal_context->current_status = srm_call_status_FAILURE;
					errno = srm_call_err(context,output->retstatus,srmfunc);
					ret = -1;
				}else{				
					errno = 0;
					internal_context->current_status = srm_call_status_SUCCESS;
					ret = copy_pinfilestatuses_put(output->retstatus, &output->filestatuses,
												repfs,
												srmfunc);
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



int srmv2_prepare_to_get_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context)
{
	char *targetspacetoken;
	int i;
	int n;
	int ret = 0;
	struct srm2__srmPrepareToGetResponse_ rep;
	struct srm2__ArrayOfTGetRequestFileStatus *repfs;
	struct srm2__srmPrepareToGetRequest req;
	struct srm2__TReturnStatus *reqstatp;
	struct srm_getbestspacetokens_input spacetokeninput;
	int canary = 0;
    struct soap* soap = srm_soap_init_context_new(context);
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	const char srmfunc[] = "PrepareToGet";


	/* issue "get" request */
	memset (&req, 0, sizeof(req));
	memset(output,0,sizeof(*output));
	if ((req.arrayOfFileRequests =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}


	/* get first space token from user space token description */
	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
	{
		spacetokeninput.spacetokendesc = input->spacetokendesc;
		targetspacetoken = srm_getbestspacetoken (context,&spacetokeninput);
		if (targetspacetoken != NULL)
		{
			req.targetSpaceToken = targetspacetoken;
		} else
		{
			srm_soap_free(soap);
			return (-1);
		}
	}

	if (input->desiredpintime > 0)
	{
		req.desiredPinLifeTime = &input->desiredpintime;
	}
	if (context->timeout > 0)
	{
		req.desiredTotalRequestTime = &context->timeout;
	}

	req.desiredFileStorageType = &s_types[PERMANENT];
	req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

	for (i = 0; i < input->nbfiles; i++)
	{
		if ((req.arrayOfFileRequests->requestArray[i] =
					soap_malloc (soap, sizeof(struct srm2__TGetFileRequest))) == NULL)
		{
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_free(soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
		req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *)input->surls[i];
		req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;

	ret = srm_set_protocol_in_transferParameters(context,soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_free(soap);
		return (-1);
	}

	do
	{
		ret = call_function.call_srm2__srmPrepareToGet (soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPrepareToGetResponse== NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmPrepareToGetResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		set_estimated_wait_time(internal_context, rep.srmPrepareToGetResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if (copy_string(&output->token,rep.srmPrepareToGetResponse->requestToken))
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
				break;
			}
			if  (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				break;
			}else
			{
				errno = 0;
				ret = copy_pinfilestatuses_get(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
			}
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				errno = 0;
				internal_context->current_status = srm_call_status_SUCCESS;
				ret = copy_pinfilestatuses_get(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
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

int srmv2_status_of_get_request_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTGetRequestFileStatus *repfs;
	int ret = 0;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmStatusOfGetRequestResponse_ srep;
	struct srm2__srmStatusOfGetRequestRequest sreq;
    const char srmfunc[] = "StatusOfGetRequest";


	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;
	output->retstatus = NULL;
	output->filestatuses = NULL;

	do
	{
		ret = call_function.call_srm2__srmStatusOfGetRequest(soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfGetRequestResponse == NULL)||(ret!=0) ||
				copy_returnstatus(&output->retstatus,srep.srmStatusOfGetRequestResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		set_estimated_wait_time(internal_context, srep.srmStatusOfGetRequestResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if  (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				break;
			}else
			{
				errno = 0;
				ret = copy_pinfilestatuses_get(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
			}
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				errno = 0;
				internal_context->current_status = srm_call_status_SUCCESS;
				ret = copy_pinfilestatuses_get(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
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
int srmv2_put_done(struct srm_context *context,
		struct srm_putdone_input *input, struct srmv2_filestatus **statuses)
{
	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int ret=0;
	struct srm2__srmPutDoneResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmPutDoneRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "PutDone";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);


	memset (&req, 0, sizeof(req));
	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;


	do
	{
		ret = call_function.call_srm2__srmPutDone(soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPutDoneResponse == NULL)||(ret!=0))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		reqstatp = rep.srmPutDoneResponse->returnStatus;
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,reqstatp);

		repfs = rep.srmPutDoneResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);


	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,reqstatp,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}else
	{
		errno = 0;
		internal_context.current_status = srm_call_status_SUCCESS;
		ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
	}

	srm_soap_free(soap);
	return (ret);
}
int srmv2_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses)
{
	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int ret;
	struct srm2__srmReleaseFilesResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmReleaseFilesRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "ReleaseFiles";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);


	memset (&req, 0, sizeof(req));
	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	do
	{
		ret = call_function.call_srm2__srmReleaseFiles(soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmReleaseFilesResponse == NULL)||(ret!=0))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Copy response status

		//internal_context.retstatus = rep.srmReleaseFilesResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		reqstatp = rep.srmReleaseFilesResponse->returnStatus;
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,reqstatp);

		repfs = rep.srmReleaseFilesResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,reqstatp,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}else
	{
		errno = 0;
		internal_context.current_status = srm_call_status_SUCCESS;
		ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
	}

	srm_soap_free(soap);
	return (ret);
}


int srmv2_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context)
{
	srm_call_status current_status;
	int ret,i;
	struct srm_getbestspacetokens_input spacetokeninput;
	struct srm2__srmBringOnlineResponse_ rep;
	struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
	struct srm2__srmBringOnlineRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	char *targetspacetoken;
	int nbproto = 0;
	const char srmfunc[] = "BringOnline";



	// issue "bringonline" request
	memset(output,0,sizeof(*output));
	memset (&req, 0, sizeof(req));

	if ((req.arrayOfFileRequests =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	// get first space token from user space token description
	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
	{
		spacetokeninput.spacetokendesc = input->spacetokendesc;
		targetspacetoken = srm_getbestspacetoken (context,&spacetokeninput);
		if (targetspacetoken != NULL)
		{
			req.targetSpaceToken = targetspacetoken;
		} else
		{
			srm_soap_free(soap);
			return (-1);
		}
	}

	req.authorizationID = NULL;
	req.userRequestDescription = NULL;
	req.storageSystemInfo = NULL;
	req.desiredFileStorageType = &s_types[PERMANENT];
	req.targetFileRetentionPolicyInfo = NULL;
	req.deferredStartTime = NULL;

	req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

	for (i = 0; i < input->nbfiles; i++) {
		if ((req.arrayOfFileRequests->requestArray[i] =
				soap_malloc (soap, sizeof(struct srm2__TGetFileRequest))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_free(soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
		req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *) input->surls[i];
		req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;

	ret = srm_set_protocol_in_transferParameters(context,soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_free(soap);
		return (-1);
	}
	do
	{
		ret = call_function.call_srm2__srmBringOnline (soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmBringOnlineResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmBringOnlineResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}

		set_estimated_wait_time(internal_context, rep.srmBringOnlineResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus );

		repfs = rep.srmBringOnlineResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if (copy_string(&output->token,rep.srmBringOnlineResponse->requestToken))
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
				break;
			}
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				break;
			}else
			{
				errno = 0;
				ret = copy_pinfilestatuses_bringonline(output->retstatus,
												&output->filestatuses,
												repfs,
												srmfunc);
			}
			break;
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				internal_context->current_status = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				errno = 0;
				internal_context->current_status = srm_call_status_SUCCESS;
				ret = copy_pinfilestatuses_bringonline(output->retstatus,
											&output->filestatuses,
											repfs,
											srmfunc);
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
int srmv2_status_of_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context)
{
	int sav_errno = 0;
	int i = 0;
	int n;
	int ret;
	struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmStatusOfBringOnlineRequestResponse_ srep;
	struct srm2__srmStatusOfBringOnlineRequestRequest sreq;
	const char srmfunc[] = "StatusOfBringOnlineRequest";



	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = (char *) output->token;
	output->retstatus = NULL;
	output->filestatuses = NULL;


	do
	{
		ret = call_function.call_srm2__srmStatusOfBringOnlineRequest(soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfBringOnlineRequestResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,srep.srmStatusOfBringOnlineRequestResponse->returnStatus))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		set_estimated_wait_time(internal_context, srep.srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime);
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	switch (internal_context->current_status)
	{
		case srm_call_status_SUCCESS:
		case srm_call_status_FAILURE:
			// Check if file structure ok
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				internal_context->current_status  = srm_call_status_FAILURE;
				errno = srm_call_err(context,output->retstatus,srmfunc);
				ret = -1;
			}else
			{
				errno = 0;
				internal_context->current_status  = srm_call_status_SUCCESS;
				ret = copy_pinfilestatuses_bringonline(output->retstatus,
												&output->filestatuses,
												repfs,
												srmfunc);
			}
			break;
		case srm_call_status_QUEUED:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				break;
			}else
			{
				errno = 0;
				ret = copy_pinfilestatuses_bringonline(output->retstatus,
												&output->filestatuses,
												repfs,
												srmfunc);
			}
			break;
		default:
			errno = srm_call_err(context,output->retstatus,srmfunc);
			ret = -1;
			break;
	}

	srm_soap_free(soap);
	return (ret);

}
int srmv2_abort_files(struct srm_context *context,
		struct srm_abort_files_input *input,struct srmv2_filestatus **statuses)
{
	int ret;
	struct srm_internal_context internal_context;
	struct srm2__srmAbortFilesResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmAbortFilesRequest req;
	struct srm2__TReturnStatus *reqstatp;
    struct soap* soap = srm_soap_init_context_new(context);
	const char srmfunc[] = "AbortFiles";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	

	memset (&req, 0, sizeof(req));

	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	do
	{
		ret = call_function.call_srm2__srmAbortFiles (soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmAbortFilesResponse == NULL)||(ret!=0))
		{
			errno = srm_soap_call_err(context,soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_free(soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		reqstatp = rep.srmAbortFilesResponse->returnStatus;
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,reqstatp);

		repfs = rep.srmAbortFilesResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	switch (internal_context.current_status)
	{
		case  srm_call_status_SUCCESS:
		case  srm_call_status_FAILURE:
			if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
			{
				errno = srm_call_err(context,reqstatp,srmfunc);
				srm_soap_free(soap);
				return (-1);
			}else
			{
				errno = 0;
				internal_context.current_status = srm_call_status_SUCCESS;
				ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
			}
			break;
		default:
			ret = -1;
			break;
	}

	srm_soap_free(soap);
	return (ret);
}

int srmv2_abort_request(struct srm_context *context,char *token)
{
	const char srmfunc[] = "AbortRequest";
	struct srm2__srmAbortRequestRequest abortreq;
	struct srm2__srmAbortRequestResponse_ abortrep;
    struct soap* soap = srm_soap_init_context_new(context);
	int result;


	memset (&abortreq, 0, sizeof(abortreq));

	if (token == NULL)
	{
		// No token supplied
		errno = EINVAL;
		srm_soap_free(soap);
		return (-1);
	}else
	{
		abortreq.requestToken = token;

		result = call_function.call_srm2__srmAbortRequest (soap, context->srm_endpoint, srmfunc, &abortreq, &abortrep);

		if (result != 0)
		{
			// Soap call failure
			errno = srm_soap_call_err(context,soap,srmfunc);
		}else
		{
			if (abortrep.srmAbortRequestResponse == NULL ||
					abortrep.srmAbortRequestResponse->returnStatus == NULL ||
					abortrep.srmAbortRequestResponse->returnStatus->statusCode != SRM_USCORESUCCESS)
			{
				errno = EINVAL;
				srm_soap_free(soap);
				return (-1);
			}
		}
	}

	srm_soap_free(soap);

	return result;
}
