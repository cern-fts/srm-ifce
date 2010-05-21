#include <errno.h>
#include <assert.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int copy_filestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLReturnStatus *repfs,
						char *srmfunc);

int copy_pinfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						char srmfunc);

int srmv2_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_put_request(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_get_request(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_bringonline (struct srm_context *context,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);

int srmv2_bringonline_async (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);


int srmv2_prepeare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srmv2_pinfilestatus **filestatuses)
{
	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	internal_context.current_status = srmv2_prepare_to_put_async(context,input,filestatuses,&internal_context);


	// if request was queued start polling statusOfLsRequest
	if (internal_context.current_status == srm_call_status_QUEUED)
	{
		if (srmv2_status_of_put_request(context,input,filestatuses,&internal_context))
		{
			srmv2_abort_request(context,&internal_context);
			return -1;
		}

		// status of request
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return 0;
}

//srm_preparetoput_output

int srmv2_status_of_put_request(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	int ret = 0;
	struct soap soap;
	struct srm2__srmStatusOfPutRequestResponse_ srep;
	struct srm2__srmStatusOfPutRequestRequest sreq;
	const char srmfunc[] = "StatusOfPutRequest";

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;

	do
	{
		ret = call_function.call_srm2__srmStatusOfPutRequest(&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfPutRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = srep.srmStatusOfPutRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;

	}while ((internal_context->current_status == srm_call_status_INTERNAL_ERROR)||(internal_context->current_status == srm_call_status_QUEUED));

	if (output->retstatus->statusCode == SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED) {
		srm_errmsg (context,"[SRM][%s][%s] %s: Space lifetime expired",srmfunc, statuscode2errmsg(output->retstatus->statusCode), context->srm_endpoint);
		errno = statuscode2errno(output->retstatus->statusCode);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (internal_context->current_status != srm_call_status_SUCCESS ||
			!repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (internal_context->current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}
	srm_soap_deinit(&soap);
	return ret;
}



int srmv2_prepare_to_put_async(struct srm_context *context,
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
	struct soap soap;
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	char *targetspacetoken;
	const char srmfunc[] = "PrepareToPut";
	struct srm_getbestspacetokens_input spacetokeninput;
	SRM_LONG64 totalsize;
	struct srm_mkdir_input mkdirinput;

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfFileRequests =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfTPutFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (&soap, input->nbfiles* sizeof(struct srm2__TPutFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (&soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (&soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
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

	for (i = 0; i < input->nbfiles; i++) {
		if ((req.arrayOfFileRequests->requestArray[i] =
					soap_malloc (&soap, sizeof(struct srm2__TPutFileRequest))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TPutFileRequest));
		req.arrayOfFileRequests->requestArray[i]->targetSURL = (char *)input->surls[i];

		if ((req.arrayOfFileRequests->requestArray[i]->expectedFileSize = soap_malloc (&soap, sizeof(ULONG64))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		*(req.arrayOfFileRequests->requestArray[i]->expectedFileSize) = input->filesizes[i];
		totalsize += input->filesizes[i]; // compute total size to determine best space token
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;
	req.transferParameters->arrayOfTransferProtocols = NULL;

	ret = srm_set_protocol_in_transferParameters(context,&soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_deinit(&soap);
		return (-1);
	}


	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
		{
			spacetokeninput.neededsize = totalsize;
			spacetokeninput.spacetokendesc = input->spacetokendesc;
			targetspacetoken = srmv2_getbestspacetoken (context,&spacetokeninput);
			if (targetspacetoken != NULL)
			{
				req.targetSpaceToken = targetspacetoken;
			} else
			{
				srm_soap_deinit(&soap);
				return (-1);
			}
	}

	// Create sub-directories of SURLs
	for (i = 0; i < input->nbfiles; ++i)
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
	}

	do
	{
		ret = call_function.call_srm2__srmPrepareToPut (&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPrepareToPutResponse== NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = rep.srmPrepareToPutResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = rep.srmPrepareToPutResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	if (copy_token(output->token,internal_context->current_status,rep.srmPrepareToPutResponse->requestToken))
	{
		srm_soap_deinit(&soap);
		return -1;
	}

	if (internal_context->current_status != srm_call_status_SUCCESS ||
			!repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}


	if (internal_context->current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}

	srm_soap_deinit(&soap);
	return ret;
}

int srmv2_prepeare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input, struct srmv2_pinfilestatus **filestatuses)
{
/*	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	internal_context->current_status = srmv2_prepare_to_get_async(context,input,filestatuses,&internal_context);


	// if request was queued start polling statusOfLsRequest
	if (internal_context->current_status == srm_call_status_QUEUED)
	{
		internal_context->current_status = srmv2_status_of_get_request(context,input,filestatuses,&internal_context);
		if (internal_context->current_status != srm_call_status_SUCCESS)
		{
			internal_context->current_status = srmv2_abort_request(context,&internal_context);
			return -1;
		}

		// status of request
	}

	if (internal_context->current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}*/
	return 0;
}
int srmv2_prepare_to_get_async(struct srm_context *context,
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
	struct soap soap;
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	const char srmfunc[] = "PrepareToGet";


	srm_soap_init(&soap);

	/* issue "get" request */
	memset (&req, 0, sizeof(req));

	if ((req.arrayOfFileRequests =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (&soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (&soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (&soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	/* get first space token from user space token description */
	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
	{
		spacetokeninput.spacetokendesc = input->spacetokendesc;
		targetspacetoken = srmv2_getbestspacetoken (context,&spacetokeninput);
		if (targetspacetoken != NULL)
		{
			req.targetSpaceToken = targetspacetoken;
		} else
		{
			srm_soap_deinit(&soap);
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
					soap_malloc (&soap, sizeof(struct srm2__TGetFileRequest))) == NULL)
		{
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
		req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *)input->surls[i];
		req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;

	ret = srm_set_protocol_in_transferParameters(context,&soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_deinit(&soap);
		return (-1);
	}

	do
	{
		ret = call_function.call_srm2__srmPrepareToGet (&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPrepareToGetResponse== NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = rep.srmPrepareToGetResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	/* wait for files ready */

	if (copy_token(output->token,internal_context->current_status,rep.srmPrepareToGetResponse->requestToken))
	{
		srm_soap_deinit(&soap);
		return -1;
	}

	if (internal_context->current_status != srm_call_status_SUCCESS || !repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (internal_context->current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}

	srm_soap_deinit(&soap);
	return ret;
}

int srmv2_status_of_get_request(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	int ret = 0;
	struct soap soap;
	struct srm2__srmStatusOfGetRequestResponse_ srep;
	struct srm2__srmStatusOfGetRequestRequest sreq;
	const char srmfunc[] = "StatusOfPutRequest";

	srm_soap_init(&soap);

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;

	do
	{
		ret = call_function.call_srm2__srmStatusOfGetRequest(&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfGetRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = srep.srmStatusOfGetRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	if (internal_context->current_status == srm_call_status_QUEUED)
	{
		srm_soap_deinit(&soap);
		return (0);
	}

	if (internal_context->current_status != srm_call_status_SUCCESS ||
			!repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}


	if (internal_context->current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}

	srm_soap_deinit(&soap);
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
	struct soap soap;
	const char srmfunc[] = "PutDone";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit (&soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;


	do
	{
		ret = call_function.call_srm2__srmPutDone(&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmPutDoneResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		//internal_context.retstatus = rep.srmPutDoneResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,rep.srmPutDoneResponse->returnStatus);

		repfs = rep.srmPutDoneResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);


	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,&internal_context,srmfunc);
		srm_soap_deinit (&soap);
		return (-1);
	}

	ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);


	srm_soap_deinit(&soap);
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
	struct soap soap;
	const char srmfunc[] = "ReleaseFiles";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit (&soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	do
	{
		ret = call_function.call_srm2__srmReleaseFiles(&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmReleaseFilesResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		//internal_context.retstatus = rep.srmReleaseFilesResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,rep.srmReleaseFilesResponse->returnStatus);

		repfs = rep.srmReleaseFilesResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,&internal_context,srmfunc);
		srm_soap_deinit (&soap);
		return (-1);
	}

	ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);

	srm_soap_deinit(&soap);
	return (ret);
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
int copy_pinfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						char srmfunc)
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
		if (repfs->statusArray[i]->remainingPinLifetime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinLifetime);
	}
	return n;
}

int srmv2_copy(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output **output)
{
	return 0;
}
int srmv2_abort_files(struct srm_context *context,
		struct srm_abortfiles_input *input,struct srmv2_filestatus **statuses)
{
	int ret;
	struct srm_internal_context internal_context;
	struct srm2__srmAbortFilesResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmAbortFilesRequest req;
	struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	const char srmfunc[] = "AbortFiles";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	req.requestToken = (char *) input->reqtoken;

	// NOTE: only one SURL in the array
	if ((req.arrayOfSURLs =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	do
	{
		ret = call_function.call_srm2__srmAbortFiles (&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmAbortFilesResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		//internal_context.retstatus = rep.srmAbortFilesResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,rep.srmAbortFilesResponse->returnStatus);

		repfs = rep.srmAbortFilesResponse->arrayOfFileStatuses;

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,&internal_context,srmfunc);
		srm_soap_deinit (&soap);
		return (-1);
	}

	ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);

	srm_soap_deinit(&soap);
	return (ret);
}
int srmv2_abort_request_new (struct srm_context *context,struct srm_abort_request_input *input)
{
	struct srm_internal_context internal_context;
	int ret;
	struct srm2__srmAbortRequestResponse_ rep;
	struct srm2__srmAbortRequestRequest req;
	struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	const char srmfunc[] = "AbortRequest";

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	req.requestToken = (char *) input->reqtoken;

	do
	{
		ret = call_function.call_srm2__srmAbortRequest (&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmAbortRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		//internal_context.retstatus = rep.srmAbortRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,rep.srmAbortRequestResponse->returnStatus);

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		errno = srm_call_err(context,&internal_context,srmfunc);
		srm_soap_deinit (&soap);
		return (-1);
	}

	srm_soap_deinit (&soap);
	return (ret);
}
int srmv2_abort_request(struct srm_context *context,struct srm_abort_request_input *input)
{
	const char srmfunc[] = "AbortRequest";
	struct srm2__srmAbortRequestRequest abortreq;
	struct srm2__srmAbortRequestResponse_ abortrep;
	struct soap soap;
	int result;
	srm_soap_init(&soap);


	if (input->reqtoken == NULL)
	{
		// No token supplied
		return (-1);
	}else
	{
		abortreq.requestToken = input->reqtoken;

		result = call_function.call_srm2__srmAbortRequest (&soap, context->srm_endpoint, srmfunc, &abortreq, &abortrep);

		if (result != 0)
		{
			// Soap call failure
			errno = srm_soup_call_err(context,&soap,srmfunc);
		}
	}

	srm_soap_deinit(&soap);

	return result;
}

int srmv2_bringonline_async (struct srm_context *context,
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
	struct soap soap;
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	char *targetspacetoken;
	int nbproto = 0;
	const char srmfunc[] = "BringOnline";

	srm_soap_init(&soap);

	// issue "bringonline" request
	memset (&req, 0, sizeof(req));

	if ((req.arrayOfFileRequests =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (&soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (&soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (&soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit (&soap);
		return (-1);
	}

	// get first space token from user space token description
	if (!input->spacetokendesc)
	{
		req.targetSpaceToken = NULL;
	} else
	{
		spacetokeninput.spacetokendesc = input->spacetokendesc;
		targetspacetoken = srmv2_getbestspacetoken (context,&spacetokeninput);
		srmv2_getbestspacetoken (context);
		if (targetspacetoken != NULL)
		{
			req.targetSpaceToken = targetspacetoken;
		} else
		{
			srm_soap_deinit(&soap);
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
				soap_malloc (&soap, sizeof(struct srm2__TGetFileRequest))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit (&soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
		req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *) input->surls[i];
		req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;

	ret = srm_set_protocol_in_transferParameters(context,&soap, req.transferParameters,input->protocols);

	if (ret)
	{
		srm_soap_deinit(&soap);
		return (-1);
	}
	do
	{
		ret = call_function.call_srm2__srmBringOnline (&soap, context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmBringOnlineResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = rep.srmBringOnlineResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus );

		repfs = rep.srmBringOnlineResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);

	/* wait for files ready */
	if (copy_token(internal_context,current_status,rep.srmBringOnlineResponse->requestToken))
	{
		srm_soap_deinit(&soap);
		return -1;
	}

	if (internal_context->current_status != srm_call_status_SUCCESS || internal_context->current_status != srm_call_status_QUEUED ||
			!repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (internal_context->current_status == srm_call_status_SUCCESS || internal_context->current_status == srm_call_status_QUEUED )
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}

	srm_soap_deinit(&soap);
	return ret;
}
int srmv2_status_of_bringonline (struct srm_context *context,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context)
{
	int sav_errno = 0;
	int i = 0;
	int n;
	int ret;
	struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
	struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	struct srm2__srmStatusOfBringOnlineRequestResponse_ srep;
	struct srm2__srmStatusOfBringOnlineRequestRequest sreq;
	const char srmfunc[] = "StatusOfBringOnlineRequest";

	srm_soap_init(&soap);

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = (char *) output->token;


	do
	{
		ret = call_function.call_srm2__srmStatusOfBringOnlineRequest(&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfBringOnlineRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		output->retstatus = srep.srmStatusOfBringOnlineRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

		repfs = srep.srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses;

	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (internal_context->current_status == srm_call_status_SUCCESS )
	{
		ret = copy_pinfilestatuses(output->retstatus,
								output->filestatuses,
								repfs,
								srmfunc);
	}


	srm_soap_deinit(&soap);
	return (ret);

}
int srmv2_bring_online(struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output)
{
	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	srmv2_bringonline_async(context,input,output,&internal_context);


	// if request was queued start polling statusOfLsRequest
	if (internal_context.current_status == srm_call_status_QUEUED)
	{
		if (srmv2_status_of_bringonline(context,output,&internal_context))
		{
			internal_context.current_status = srmv2_abort_request(context,&internal_context);
			return -1;
		}

		// status of request
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return 0;
}
/*
srmv2_prestagestatuse (const char *reqtoken, const char *srm_endpoint, struct srmv2_pinfilestatus **filestatuses,
		char *errbuf, int errbufsz, int timeout)
{
	int flags;
	int sav_errno = 0;
	int i = 0;
	int n;
	int ret;
	struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
	struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	struct srm2__srmStatusOfBringOnlineRequestResponse_ srep;
	struct srm2__srmStatusOfBringOnlineRequestRequest sreq;
	const char srmfunc[] = "StatusOfBringOnlineRequest";

	soap_init (&soap);
	soap.namespaces = namespaces_srmv2;

#ifdef GFAL_SECURE
	flags = CGSI_OPT_DISABLE_NAME_CHECK;
	soap_register_plugin_arg (&soap, client_cgsi_plugin, &flags);
#endif

	soap.send_timeout = gfal_get_timeout_sendreceive ();
	soap.recv_timeout = gfal_get_timeout_sendreceive ();
	soap.connect_timeout = gfal_get_timeout_connect ();

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = (char *) reqtoken;

	if ((ret = soap_call_srm2__srmStatusOfBringOnlineRequest (&soap, srm_endpoint, srmfunc, &sreq, &srep))) {
		if (soap.fault != NULL && soap.fault->faultstring != NULL)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: %s",
					gfal_remote_type, srmfunc, srm_endpoint, soap.fault->faultstring);
		else if (soap.error == SOAP_EOF)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Connection fails or timeout",
				   	gfal_remote_type, srmfunc, srm_endpoint);
		else
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Unknown SOAP error (%d)",
					gfal_remote_type, srmfunc, srm_endpoint, soap.error);

		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	if (srep.srmStatusOfBringOnlineRequestResponse == NULL ||
			(reqstatp = srep.srmStatusOfBringOnlineRequestResponse->returnStatus) == NULL) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: <empty response>",
				gfal_remote_type, srmfunc, srm_endpoint);
		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	// return file statuses
	repfs = srep.srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses;

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray) {
		if (reqstatp->statusCode != SRM_USCORESUCCESS &&
				reqstatp->statusCode != SRM_USCOREPARTIAL_USCORESUCCESS) {

			sav_errno = statuscode2errno (reqstatp->statusCode);
			if (reqstatp->explanation && reqstatp->explanation[0])
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode),
						srm_endpoint, reqstatp->explanation);
			else
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <none>",
						gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
		} else {
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <empty response>",
					gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
			sav_errno = ECOMM;
		}

		soap_end (&soap);
		soap_done (&soap);
		errno = sav_errno;
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof(struct srmv2_pinfilestatus))) == NULL) {
		soap_end (&soap);
		soap_done (&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++) {
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);
		if (repfs->statusArray[i]->status) {
			(*filestatuses)[i].status = filestatus2returncode (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
	}

	soap_end (&soap);
	soap_done (&soap);
    errno = 0;
	return (n);
}
*/
/*    int nbfiles, const char **surls, int desiredpintime,
    const char *spacetokendesc, char **protocols, char **reqtoken,
    struct srmv2_pinfilestatus **filestatuses)*/

/*srmv2_prestagee (
    int nbfiles, const char **surls, const char *srm_endpoint, int desiredpintime,
    const char *spacetokendesc, char **protocols, char **reqtoken,
    struct srmv2_pinfilestatus **filestatuses, char *errbuf, int errbufsz,
    int timeout)
{



    ret = gfal_srm_callers_v2.call_bring_online(&soap, srm_endpoint, srmfunc, &req, &rep);

    if (ret) {
		if (soap.fault != NULL && soap.fault->faultstring != NULL)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: %s",
					gfal_remote_type, srmfunc, srm_endpoint, soap.fault->faultstring);
		else if (soap.error == SOAP_EOF)
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Connection fails or timeout",
				   	gfal_remote_type, srmfunc, srm_endpoint);
		else
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Unknown SOAP error (%d)",
					gfal_remote_type, srmfunc, srm_endpoint, soap.error);

		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	if (rep.srmBringOnlineResponse == NULL || (reqstatp = rep.srmBringOnlineResponse->returnStatus) == NULL) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: <empty response>",
				gfal_remote_type, srmfunc, srm_endpoint);
		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	// return request token
	if (reqtoken && rep.srmBringOnlineResponse->requestToken)
		if ((*reqtoken = strdup (rep.srmBringOnlineResponse->requestToken)) == NULL) {
			soap_end (&soap);
			soap_done (&soap);
			errno = ENOMEM;
			return (-1);
		}

	// return file statuses
	repfs = rep.srmBringOnlineResponse->arrayOfFileStatuses;

	call_is_successfull =
	    reqstatp->statusCode == SRM_USCORESUCCESS ||
	    reqstatp->statusCode == SRM_USCOREPARTIAL_USCORESUCCESS ||
	    reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED;

	if (!call_is_successfull || !repfs || repfs->__sizestatusArray < 1 ||
	    !repfs->statusArray)
	{
		if (!call_is_successfull) {
			sav_errno = statuscode2errno (reqstatp->statusCode);
			if (reqstatp->explanation && reqstatp->explanation[0])
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode),
						srm_endpoint, reqstatp->explanation);
			else
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <none>",
						gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
		} else {
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <empty response>",
					gfal_remote_type, srmfunc, srmfunc, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
			sav_errno = ECOMM;
		}

		soap_end (&soap);
		soap_done (&soap);
		errno = sav_errno;
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof(struct srmv2_pinfilestatus))) == NULL) {
		soap_end (&soap);
		soap_done (&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++) {
		if (!repfs->statusArray[i])
			continue;
		memset (*filestatuses + i, 0, sizeof (struct srmv2_pinfilestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);
		if (repfs->statusArray[i]->status) {
			(*filestatuses)[i].status = filestatus2returncode (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",
						gfal_remote_type, srmfunc, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
	}

    errno = 0;
    ret = n;

FUNCTION_CLEANUP_AND_RETURN:
    soap_end (&soap);
    soap_done (&soap);

    return ret;
}*/
