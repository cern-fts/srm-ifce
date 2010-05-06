#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int srmv2_copy(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output **output)
{
	return 0;
}
/*int srmv2_preparetoget (
    int nbfiles, const char **surls, const char *spacetokendesc, char **protocols,
    char **reqtoken, struct srmv2_filestatus **filestatuses, char *errbuf,
    int errbufsz, int timeout)
{
	char **se_types = NULL;
	char **se_endpoints = NULL;
	char *srm_endpoint = NULL;
	struct srmv2_pinfilestatus *pinfilestatuses = NULL;
	int i, r;

	if (setypesandendpointsfromsurl (surls[0], &se_types, &se_endpoints, errbuf, errbufsz) < 0)
		return (-1);

	i = 0;
	while (se_types[i]) {
		if ((strcmp (se_types[i], "srm_v2")) == 0)
			srm_endpoint = se_endpoints[i];
		i++;
	}

	free (se_types);
	free (se_endpoints);

	if (! srm_endpoint) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[GFAL][][] %s: No matching SRMv2.2-compliant SE", surls[0]);
		errno = EINVAL;
		return (-1);
	}

	r = srmv2_preparetogete (nbfiles, surls,spacetokendesc, srm_endpoint, 0, protocols,
	    reqtoken, &pinfilestatuses, errbuf, errbufsz, timeout);

	free (srm_endpoint);

	if (r < 0 || pinfilestatuses == NULL)
		return (r);

	if ((*filestatuses = (struct srmv2_filestatus *) calloc (r, sizeof (struct srmv2_filestatus))) == NULL) {
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < r; ++i) {
		(*filestatuses)[i].surl = pinfilestatuses[i].surl;
		(*filestatuses)[i].turl = pinfilestatuses[i].turl;
		(*filestatuses)[i].status = pinfilestatuses[i].status;
		(*filestatuses)[i].explanation = pinfilestatuses[i].explanation;
	}

	free (pinfilestatuses);
    errno = 0;
	return (r);
}*/

/*int nbfiles, const char **surls, const char *srm_endpoint, int desiredpintime,
    const char *spacetokendesc, char **protocols, char **reqtoken,
    struct srmv2_pinfilestatus **filestatuses, char *errbuf, int errbufsz,
    int timeout)*/

/*
int srmv2_preparetogete (struct srm_context *context,
		struct srm_preparetoget_input *input,struct srm_preparetoget_output **output)

{
	int flags;
	int sav_errno = 0;
	int n;
	int ret;
	struct srm2__srmPrepareToGetResponse_ rep;
	struct srm2__ArrayOfTGetRequestFileStatus *repfs;
	struct srm2__srmPrepareToGetRequest req;
	struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
	int i = 0;
	char *targetspacetoken;
	const char srmfunc[] = "PrepareToGet";
	int call_is_successfull = 0;

	srm_soap_init(&soap);

	// issue "get" request

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfFileRequests =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
			(req.arrayOfFileRequests->requestArray =
			 soap_malloc (&soap, nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
			(req.transferParameters =
			 soap_malloc (&soap, sizeof(struct srm2__TTransferParameters))) == NULL ||
			(req.targetSpaceToken =
			 soap_malloc (&soap, sizeof(char *))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	// get first space token from user space token description
	if (!input->spacetokendesc) {
		req.targetSpaceToken = NULL;
	} else if ((targetspacetoken = srmv2_getbestspacetoken (spacetokendesc, srm_endpoint, 0, errbuf, errbufsz, timeout)) != NULL) {
		req.targetSpaceToken = input->t targetspacetoken;
	} else {
		sav_errno = errno;
		soap_end (&soap);
		soap_done (&soap);
		errno = sav_errno;
		return (-1);
	}

	if (desiredpintime > 0)
		req.desiredPinLifeTime = &desiredpintime;
	if (timeout > 0)
		req.desiredTotalRequestTime = &timeout;

	req.desiredFileStorageType = &s_types[PERMANENT];
	req.arrayOfFileRequests->__sizerequestArray = nbfiles;

	for (i = 0; i < nbfiles; i++) {
		if ((req.arrayOfFileRequests->requestArray[i] =
					soap_malloc (&soap, sizeof(struct srm2__TGetFileRequest))) == NULL) {
			gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[GFAL][soap_malloc][] error");
			errno = ENOMEM;
			soap_end (&soap);
			soap_done (&soap);
			return (-1);
		}
		memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
		req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *) surls[i];
		req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
	}

	req.transferParameters->accessPattern = NULL;
	req.transferParameters->connectionType = NULL;
	req.transferParameters->arrayOfClientNetworks = NULL;

	ret = gfal_set_protocol_in_transferParameters(
        &soap, req.transferParameters, protocols, errbuf, errbufsz);

    if (ret != GFAL_SRM_RETURN_OK)
        goto FUNCTION_CLEANUP_AND_RETURN;

    ret = gfal_srm_callers_v2.call_prepare_to_get(&soap, srm_endpoint, srmfunc, &req, &rep);

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

	// return request token
	if (reqtoken && rep.srmPrepareToGetResponse->requestToken)
		if ((*reqtoken = strdup (rep.srmPrepareToGetResponse->requestToken)) == NULL) {
			soap_end (&soap);
			soap_done (&soap);
			errno = ENOMEM;
			return (-1);
		}

	if (rep.srmPrepareToGetResponse == NULL || (reqstatp = rep.srmPrepareToGetResponse->returnStatus) == NULL) {
		gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][] %s: <empty response>",
				gfal_remote_type, srmfunc, srm_endpoint);
		soap_end (&soap);
		soap_done (&soap);
		errno = ECOMM;
		return (-1);
	}

	// return file statuses
	repfs = rep.srmPrepareToGetResponse->arrayOfFileStatuses;

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
						gfal_remote_type, srmfunc, statuscode2errmsg (reqstatp->statusCode),
						srm_endpoint);
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
		memset (*filestatuses + i, 0, sizeof (struct srmv2_filestatus));
		if (repfs->statusArray[i]->sourceSURL)
			(*filestatuses)[i].surl = strdup (repfs->statusArray[i]->sourceSURL);
		if (repfs->statusArray[i]->transferURL)
			(*filestatuses)[i].turl = strdup (repfs->statusArray[i]->transferURL);
		if (repfs->statusArray[i]->status) {
			(*filestatuses)[i].status = filestatus2returncode (repfs->statusArray[i]->status->statusCode);
			if (repfs->statusArray[i]->status->explanation && repfs->statusArray[i]->status->explanation[0])
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s", gfal_remote_type, srmfunc,
						statuscode2errmsg(repfs->statusArray[i]->status->statusCode), repfs->statusArray[i]->status->explanation);
			else if (reqstatp->explanation != NULL && reqstatp->explanation[0] && strncasecmp (reqstatp->explanation, "failed for all", 14))
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] %s", gfal_remote_type, srmfunc,
						statuscode2errmsg(repfs->statusArray[i]->status->statusCode), reqstatp->explanation);
			else
				asprintf (&((*filestatuses)[i].explanation), "[%s][%s][%s] <none>", gfal_remote_type, srmfunc,
						statuscode2errmsg(repfs->statusArray[i]->status->statusCode));
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
