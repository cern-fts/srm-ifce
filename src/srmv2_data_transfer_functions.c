#include <errno.h>
#include <assert.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int copy_pinfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs);

srm_call_status srmv2_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context);

srm_call_status srmv2_status_of_put_request(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context);

srm_call_status srmv2_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context);

srm_call_status srmv2_status_of_get_request(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context);


int srmv2_prepeare_to_put(struct srm_context *context,struct srm_preparetoput_input *input, struct srmv2_pinfilestatus **filestatuses)
{
	srm_call_status current_status;
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	if (context->timeout > 0)
	{
		internal_context.end_time = (time(NULL) + context->timeout);
	}

	internal_context.attempt = 1;

	// request
	current_status = srmv2_prepare_to_put_async(context,input,filestatuses,&internal_context);


	// if request was queued start polling statusOfLsRequest
	if (current_status == srm_call_status_QUEUED)
	{
		current_status = srmv2_status_of_put_request(context,input,filestatuses,&internal_context);
		if (current_status != srm_call_status_SUCCESS)
		{
			current_status = srmv2_abort_request(context,&internal_context);
			return -1;
		}

		// status of request
	}

	if (current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return 0;
}

//srm_preparetoput_output

srm_call_status srmv2_status_of_put_request(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	srm_call_status current_status;
	int ret = 0;
	struct soap soap;
	struct srm2__srmStatusOfPutRequestResponse_ srep;
	struct srm2__srmStatusOfPutRequestRequest sreq;
	const char srmfunc[] = "StatusOfPutRequest";

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = internal_context->token;

	do
	{
		ret = call_function.call_srm2__srmStatusOfPutRequest(&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfPutRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		internal_context->retstatus = srep.srmStatusOfPutRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		current_status = back_off_logic(context,srmfunc,internal_context);

		repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;

	}while ((current_status == srm_call_status_INTERNAL_ERROR)||(current_status == srm_call_status_QUEUED));

	if (internal_context->retstatus->statusCode == SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED) {
		srm_errmsg (context,"[SRM][%s][%s] %s: Space lifetime expired",srmfunc, statuscode2errmsg(internal_context->retstatus->statusCode), context->srm_endpoint);
		errno = statuscode2errno(internal_context->retstatus->statusCode);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (current_status != srm_call_status_SUCCESS || !repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(internal_context->retstatus,
								filestatuses,
								repfs);
	}
	srm_soap_deinit(&soap);
	return ret;
}



srm_call_status srmv2_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context)
{
	srm_call_status current_status;
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
			current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		internal_context->retstatus = rep.srmPrepareToPutResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		current_status = back_off_logic(context,srmfunc,internal_context);

		repfs = rep.srmPrepareToPutResponse->arrayOfFileStatuses;

	}while (current_status == srm_call_status_INTERNAL_ERROR);

	if (current_status  == srm_call_status_QUEUED)
	{
		internal_context->token = rep.srmPrepareToPutResponse->requestToken;
	}


	if (current_status != srm_call_status_SUCCESS || !repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}


	if (current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(internal_context->retstatus,
								filestatuses,
								repfs);
	}

	srm_soap_deinit(&soap);
	return ret;
}

srm_call_status srmv2_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context)
{
	srm_call_status current_status;
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
			current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		internal_context->retstatus = rep.srmPrepareToGetResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		current_status = back_off_logic(context,srmfunc,internal_context);

		repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;

	}while (current_status == srm_call_status_INTERNAL_ERROR);

	/* wait for files ready */

	if (current_status == srm_call_status_QUEUED)
	{
		internal_context->token = rep.srmPrepareToGetResponse->requestToken;
	}

	if (current_status != srm_call_status_SUCCESS || !repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	if (current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(internal_context->retstatus,
								filestatuses,
								repfs);
	}

	srm_soap_deinit(&soap);
	return ret;
}

srm_call_status srmv2_status_of_get_request(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srmv2_pinfilestatus **filestatuses,
		struct srm_internal_context *internal_context)
{
	struct srm2__ArrayOfTPutRequestFileStatus *repfs;
	srm_call_status current_status;
	int ret = 0;
	struct soap soap;
	struct srm2__srmStatusOfGetRequestResponse_ srep;
	struct srm2__srmStatusOfGetRequestRequest sreq;
	const char srmfunc[] = "StatusOfPutRequest";

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = internal_context->token;

	do
	{
		ret = call_function.call_srm2__srmStatusOfGetRequest(&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfGetRequestResponse == NULL)||(ret!=0))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			current_status = srm_call_status_FAILURE;
			break;
		}
		// Copy response status
		internal_context->retstatus = srep.srmStatusOfGetRequestResponse->returnStatus;
		// Check status and wait with back off logic if necessary(Internal_error)
		current_status = back_off_logic(context,srmfunc,internal_context);

		repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;

	}while ((current_status == srm_call_status_INTERNAL_ERROR)||(current_status == srm_call_status_QUEUED));



	if (current_status != srm_call_status_SUCCESS || !repfs || repfs->__sizestatusArray < input->nbfiles || !repfs->statusArray)
	{
		errno = srm_call_err(context,internal_context,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}


	if (current_status == srm_call_status_SUCCESS)
	{
		ret = copy_pinfilestatuses(internal_context->retstatus,
								filestatuses,
								repfs);
	}

	srm_soap_deinit(&soap);
	return ret;

}
/*
int srmv2_turlsfromsurls_get (
    int nbfiles, const char **surls, const char *srm_endpoint, int desiredpintime,
    const char *spacetokendesc,	char **protocols, char **reqtoken,
    struct srmv2_pinfilestatus **filestatuses, char *errbuf, int errbufsz,
    int timeout)
{



	while (reqstatp->statusCode == SRM_USCOREREQUEST_USCOREQUEUED ||
			reqstatp->statusCode == SRM_USCOREREQUEST_USCOREINPROGRESS ||
			reqstatp->statusCode == SRM_USCOREINTERNAL_USCOREERROR) {

		// if user timeout has passed, abort the request
		if (timeout > 0 && time(NULL) + sleep_time > endtime) {
			abortreq.requestToken = r_token;
			soap_call_srm2__srmAbortRequest (&soap, srm_endpoint, "AbortRequest", &abortreq, &abortrep);
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][ETIMEDOUT] %s: User timeout over",
					gfal_remote_type, srmfunc_status, srm_endpoint);
			sav_errno = ETIMEDOUT;

			soap_end (&soap);
			soap_done (&soap);
			errno = ETIMEDOUT;
			return (-1);
		}

		sleep (sleep_time);
		if (++nbretries < 10)
			sleep_time *= 2; // exponentially increase sleeping time, until ~40min

		if ((ret = soap_call_srm2__srmStatusOfGetRequest (&soap, srm_endpoint, srmfunc_status, &sreq, &srep))) {
			if (soap.fault != NULL && soap.fault->faultstring != NULL)
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: %s",
						gfal_remote_type, srmfunc_status, srm_endpoint, soap.fault->faultstring);
			else if (soap.error == SOAP_EOF)
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Connection fails or timeout",
						gfal_remote_type, srmfunc_status, srm_endpoint);
			else
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: Unknown SOAP error (%d)",
						gfal_remote_type, srmfunc_status, srm_endpoint, soap.error);

			soap_end (&soap);
			soap_done (&soap);
			errno = ECOMM;
			return (-1);
		}

		if (srep.srmStatusOfGetRequestResponse == NULL || (reqstatp = srep.srmStatusOfGetRequestResponse->returnStatus) == NULL) {
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: <empty response>",
					gfal_remote_type, srmfunc_status, srm_endpoint);
			soap_end (&soap);
			soap_done (&soap);
			errno = ECOMM;
			return (-1);
		}

		repfs = srep.srmStatusOfGetRequestResponse->arrayOfFileStatuses;
	}

	call_is_successfull =
	    reqstatp->statusCode == SRM_USCORESUCCESS ||
        reqstatp->statusCode == SRM_USCOREPARTIAL_USCORESUCCESS;

	if (!call_is_successfull || !repfs || repfs->__sizestatusArray < nbfiles ||
	   !repfs->statusArray)
	{
		if (!call_is_successfull) {
			sav_errno = statuscode2errno (reqstatp->statusCode);
			if (reqstatp->explanation && reqstatp->explanation[0])
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s",
						gfal_remote_type, srmfunc_status, statuscode2errmsg (reqstatp->statusCode),
						srm_endpoint, reqstatp->explanation);
			else
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <none>",
						gfal_remote_type, srmfunc_status, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
		} else {
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <empty response>",
					gfal_remote_type, srmfunc_status, statuscode2errmsg (reqstatp->statusCode), srm_endpoint);
			sav_errno = ECOMM;
		}

		soap_end (&soap);
		soap_done (&soap);
		errno = sav_errno;
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((*filestatuses = (struct srmv2_pinfilestatus *) calloc (n, sizeof (struct srmv2_pinfilestatus))) == NULL) {
		soap_end (&soap);
		soap_done (&soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < n; i++) {
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
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s",
						gfal_remote_type, srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>",
						gfal_remote_type, srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
		}
		if (repfs->statusArray[i]->remainingPinTime)
			(*filestatuses)[i].pinlifetime = *(repfs->statusArray[i]->remainingPinTime);
	}

	if (reqtoken && sreq.requestToken)
		if ((*reqtoken = strdup (sreq.requestToken)) == NULL) {
			soap_end (&soap);
			soap_done (&soap);
			errno = ENOMEM;
			return (-1);
		}

    errno = 0;
    ret = n;

FUNCTION_CLEANUP_AND_RETURN:
	soap_end (&soap);
	soap_done (&soap);

	return ret;
}
*/
int copy_pinfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs)
{
	const char srmfunc_status[] = "StatusOfGetRequest";
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
						 srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] %s",
						 srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode),
						reqstatp->explanation);
			else if ((*filestatuses)[i].status != 0)
				asprintf (&((*filestatuses)[i].explanation), "[SRM][%s][%s] <none>",
						 srmfunc_status, statuscode2errmsg (repfs->statusArray[i]->status->statusCode));
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
