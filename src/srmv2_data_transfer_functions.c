/*
 * Copyright (c) CERN 2013-2015
 *
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 *  See  http://www.eu-emi.eu/partners for details on the copyright
 *  holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "srm_ifce_internal.h"
#include "srm_soap.h"



int srmv2_put_status_estimated_wait_time(struct srm2__srmStatusOfPutRequestResponse_ *prepareToPutStatusRep){
    if( prepareToPutStatusRep && prepareToPutStatusRep->srmStatusOfPutRequestResponse && prepareToPutStatusRep->srmStatusOfPutRequestResponse->arrayOfFileStatuses
        && prepareToPutStatusRep->srmStatusOfPutRequestResponse->arrayOfFileStatuses->__sizestatusArray > 0
        && prepareToPutStatusRep->srmStatusOfPutRequestResponse->arrayOfFileStatuses->statusArray){
        struct srm2__TPutRequestFileStatus* status= *prepareToPutStatusRep->srmStatusOfPutRequestResponse->arrayOfFileStatuses->statusArray;
        if(status && status->estimatedWaitTime){
            return *status->estimatedWaitTime;
        }
    }
    return -1;
}


int srmv2_status_of_put_request_async_internal(struct srm_context *context,
        struct srm_preparetoput_input *input,
        struct srm_preparetoput_output *output,
        struct srm_internal_context *internal_context)
{
    struct srm2__ArrayOfTPutRequestFileStatus *repfs;
    int ret = 0;
    struct srm2__srmStatusOfPutRequestResponse_ srep;
    struct srm2__srmStatusOfPutRequestRequest sreq;
    const char srmfunc[] = "StatusOfPutRequest";

    srm_context_soap_init(context);

    memset (&sreq, 0, sizeof(sreq));
    sreq.requestToken = output->token;
    output->retstatus = NULL;
    output->filestatuses = NULL;

    do
    {
        ret = call_function.call_srm2__srmStatusOfPutRequest(context->soap, context->srm_endpoint, srmfunc, &sreq, &srep);
        // If no response break with failure
        if ((srep.srmStatusOfPutRequestResponse == NULL)||(ret!=0)||
                copy_returnstatus(&output->retstatus,srep.srmStatusOfPutRequestResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }
        set_estimated_wait_time(internal_context, srmv2_put_status_estimated_wait_time(&srep));
        // Check status and wait with back off logic if necessary(Internal_error)
        internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);

        repfs = srep.srmStatusOfPutRequestResponse->arrayOfFileStatuses;

    }while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


    if (output->retstatus->statusCode == SRM_USCORESPACE_USCORELIFETIME_USCOREEXPIRED) {
        srm_errmsg(context, "[SRM][%s][%s] %s: Space lifetime expired", srmfunc,
                statuscode2errmsg(output->retstatus->statusCode),
                context->srm_endpoint);
        errno = statuscode2errno(output->retstatus->statusCode);
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
        case srm_call_status_TIMEOUT:
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }
    return ret;
}


int srmv2_put_estimated_wait_time(struct srm2__srmPrepareToPutResponse_ *prepareToPutRep){
    if(prepareToPutRep && prepareToPutRep->srmPrepareToPutResponse && prepareToPutRep->srmPrepareToPutResponse->arrayOfFileStatuses
            && prepareToPutRep->srmPrepareToPutResponse->arrayOfFileStatuses->__sizestatusArray > 0
            && prepareToPutRep->srmPrepareToPutResponse->arrayOfFileStatuses->statusArray){
       struct srm2__TPutRequestFileStatus * status = *prepareToPutRep->srmPrepareToPutResponse->arrayOfFileStatuses->statusArray;
       if(status && status->estimatedWaitTime){
           return *status->estimatedWaitTime;
       }
    }
    return -1;
}


int srmv2_prepare_to_put_async_internal(struct srm_context *context,
        struct srm_preparetoput_input *input,
        struct srm_preparetoput_output *output,
        struct srm_internal_context *internal_context)
{
    int i;
    int ret = 0;
    struct srm2__srmPrepareToPutResponse_ rep;
    struct srm2__ArrayOfTPutRequestFileStatus *repfs;
    struct srm2__srmPrepareToPutRequest req;
    static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
    char *targetspacetoken;
    const char srmfunc[] = "PrepareToPut";
    struct srm_getbestspacetokens_input spacetokeninput = {0};
    SRM_LONG64 totalsize=0;

    srm_context_soap_init(context);

    memset(&req, 0, sizeof(req));
    memset(&rep, 0, sizeof(rep));
    memset(output,0,sizeof(*output));

    if ((req.arrayOfFileRequests =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTPutFileRequest))) == NULL ||
            (req.arrayOfFileRequests->requestArray =
             soap_malloc (context->soap, input->nbfiles* sizeof(struct srm2__TPutFileRequest *))) == NULL ||
            (req.transferParameters =
             soap_malloc (context->soap, sizeof(struct srm2__TTransferParameters))) == NULL) {

        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
        return (-1);
    }

    if (input->desiredpintime > 0)
        req.desiredPinLifeTime = &input->desiredpintime;

    if (context->ext && context->ext->turl_resolution_timeout)
        req.desiredTotalRequestTime = &(context->ext->turl_resolution_timeout);

    req.desiredFileStorageType = &s_types[PERMANENT];
    req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

    if (input->filesizes == NULL &&  input->nbfiles>0)
    {
        errno = EINVAL;
        return (-1);
    }

    for (i = 0; i < input->nbfiles; i++) {
        if ((req.arrayOfFileRequests->requestArray[i] =
                    soap_malloc (context->soap, sizeof(struct srm2__TPutFileRequest))) == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }
        memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TPutFileRequest));
        req.arrayOfFileRequests->requestArray[i]->targetSURL = (char *)input->surls[i];

        if ((req.arrayOfFileRequests->requestArray[i]->expectedFileSize = soap_malloc (context->soap, sizeof(ULONG64))) == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }
        *(req.arrayOfFileRequests->requestArray[i]->expectedFileSize) = input->filesizes[i];
        totalsize += input->filesizes[i]; // compute total size to determine best space token
    }

    req.transferParameters->accessPattern = NULL;
    req.transferParameters->connectionType = NULL;
    req.transferParameters->arrayOfClientNetworks = NULL;
    req.transferParameters->arrayOfTransferProtocols = NULL;

    ret = srm_set_protocol_in_transferParameters(context, req.transferParameters,input->protocols);

    if (ret)
    {
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
        ret = call_function.call_srm2__srmPrepareToPut (context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmPrepareToPutResponse== NULL)||(ret!=0)||
                copy_returnstatus(&output->retstatus,rep.srmPrepareToPutResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }
        set_estimated_wait_time(internal_context, srmv2_put_estimated_wait_time(&rep));
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
            if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
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
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }

    return ret;
}

int srmv2_get_estimated_wait_time(struct srm2__srmPrepareToGetResponse_ *prepareTogetResp){
    if(prepareTogetResp && prepareTogetResp->srmPrepareToGetResponse
       && prepareTogetResp->srmPrepareToGetResponse->arrayOfFileStatuses &&
       prepareTogetResp->srmPrepareToGetResponse->arrayOfFileStatuses->statusArray > 0
       && prepareTogetResp->srmPrepareToGetResponse->arrayOfFileStatuses->statusArray){
       struct srm2__TGetRequestFileStatus* status=  *prepareTogetResp->srmPrepareToGetResponse->arrayOfFileStatuses->statusArray;
       if(status && status->estimatedWaitTime ){
           return *status->estimatedWaitTime ;
       }
    }
    return -1;
}

int srmv2_prepare_to_get_async_internal(struct srm_context *context,
        struct srm_preparetoget_input *input,
        struct srm_preparetoget_output *output,
        struct srm_internal_context *internal_context)
{
    char *targetspacetoken;
    int i;
    int ret = 0;
    struct srm2__srmPrepareToGetResponse_ rep;
    struct srm2__ArrayOfTGetRequestFileStatus *repfs;
    struct srm2__srmPrepareToGetRequest req;
    struct srm_getbestspacetokens_input spacetokeninput = {0};
    static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
    const char srmfunc[] = "PrepareToGet";

    srm_context_soap_init(context);

    /* issue "get" request */
    memset (&req, 0, sizeof(req));
    memset(output,0,sizeof(*output));
    if ((req.arrayOfFileRequests =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
            (req.arrayOfFileRequests->requestArray =
             soap_malloc (context->soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
            (req.transferParameters =
             soap_malloc (context->soap, sizeof(struct srm2__TTransferParameters))) == NULL) {

        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
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
            return (-1);
        }
    }


    if (input->desiredpintime > 0)
        req.desiredPinLifeTime = &input->desiredpintime;

    if (context->ext && context->ext->turl_resolution_timeout)
        req.desiredTotalRequestTime = &(context->ext->turl_resolution_timeout);

    req.desiredFileStorageType = &s_types[PERMANENT];
    req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

    for (i = 0; i < input->nbfiles; i++)
    {
        if ((req.arrayOfFileRequests->requestArray[i] =
                    soap_malloc (context->soap, sizeof(struct srm2__TGetFileRequest))) == NULL)
        {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }
        memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
        req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *)input->surls[i];
        req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
    }

    req.transferParameters->accessPattern = NULL;
    req.transferParameters->connectionType = NULL;
    req.transferParameters->arrayOfClientNetworks = NULL;
    
    // fill storageSystemInfo
    if (input->nbextrainfo > 0) {
        if ((req.storageSystemInfo = 
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTExtraInfo))) == NULL ||
            (req.storageSystemInfo->extraInfoArray =
             soap_malloc (context->soap, input->nbextrainfo * sizeof(struct srm2__TExtraInfo *))) == NULL )
        {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }

        req.storageSystemInfo->__sizeextraInfoArray = input->nbextrainfo;
        for (i = 0; i < input->nbextrainfo; i++) 
        {
            if ((req.storageSystemInfo->extraInfoArray[i] =
                soap_malloc (context->soap, sizeof(struct srm2__TExtraInfo))) == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
            }
            memset (req.storageSystemInfo->extraInfoArray[i], 0, sizeof(struct srm2__TExtraInfo));
            req.storageSystemInfo->extraInfoArray[i]->key = input->extrainfo[i].key;
            req.storageSystemInfo->extraInfoArray[i]->value = input->extrainfo[i].value;
        }
    } else {
        req.storageSystemInfo = NULL;
    }
    ret = srm_set_protocol_in_transferParameters(context, req.transferParameters,input->protocols);

    if (ret)
    {
        return (-1);
    }

    do
    {
        ret = call_function.call_srm2__srmPrepareToGet (context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmPrepareToGetResponse== NULL)||(ret!=0)||
                copy_returnstatus(&output->retstatus,rep.srmPrepareToGetResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }
        set_estimated_wait_time(internal_context, srmv2_get_estimated_wait_time(&rep));
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
                if(rep.srmPrepareToGetResponse->requestToken != NULL){
                    ret =  copy_string(&output->token,rep.srmPrepareToGetResponse->requestToken);
                }
                if (ret >= 0)
                    ret = copy_pinfilestatuses_get(output->retstatus,
                                                &output->filestatuses,
                                                repfs,
                                                srmfunc);
            }
            break;
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }

    return ret;
}


int srmv2_get_status_estimated_wait_time(struct srm2__srmStatusOfGetRequestResponse_ *prepareTogetRespstatus){
    if(prepareTogetRespstatus && prepareTogetRespstatus->srmStatusOfGetRequestResponse
            && prepareTogetRespstatus->srmStatusOfGetRequestResponse->arrayOfFileStatuses
            && prepareTogetRespstatus->srmStatusOfGetRequestResponse->arrayOfFileStatuses->__sizestatusArray > 0
            && prepareTogetRespstatus->srmStatusOfGetRequestResponse->arrayOfFileStatuses->statusArray){
        struct srm2__TGetRequestFileStatus* status = *prepareTogetRespstatus->srmStatusOfGetRequestResponse->arrayOfFileStatuses->statusArray;
        if(status && status->estimatedWaitTime)
            return *status->estimatedWaitTime;
    }
    return -1;
}


int srmv2_status_of_get_request_async_internal(struct srm_context *context,
        struct srm_preparetoget_input *input,
        struct srm_preparetoget_output *output,
        struct srm_internal_context *internal_context)
{
    struct srm2__ArrayOfTGetRequestFileStatus *repfs;
    int ret = 0;
    struct srm2__srmStatusOfGetRequestResponse_ srep;
    struct srm2__srmStatusOfGetRequestRequest sreq;
    const char srmfunc[] = "StatusOfGetRequest";

    srm_context_soap_init(context);

    memset (&sreq, 0, sizeof(sreq));
    sreq.requestToken = output->token;
    output->retstatus = NULL;
    output->filestatuses = NULL;

    do
    {
        ret = call_function.call_srm2__srmStatusOfGetRequest(context->soap, context->srm_endpoint, srmfunc, &sreq, &srep);
        // If no response break with failure
        if ((srep.srmStatusOfGetRequestResponse == NULL)||(ret!=0) ||
                copy_returnstatus(&output->retstatus,srep.srmStatusOfGetRequestResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }
        set_estimated_wait_time(internal_context, srmv2_get_status_estimated_wait_time(&srep));
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
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }

    return ret;
}

int srmv2_put_done(struct srm_context *context,
        struct srm_putdone_input *input, struct srmv2_filestatus **statuses)
{
    struct srm_internal_context internal_context;
    int ret=0;
    struct srm2__srmPutDoneResponse_ rep;
    struct srm2__TReturnStatus *reqstatp;
    struct srm2__ArrayOfTSURLReturnStatus *repfs;
    struct srm2__srmPutDoneRequest req;
    const char srmfunc[] = "PutDone";

    srm_context_soap_init(context);

    // Setup the timeout
    back_off_logic_init(context,&internal_context);


    memset (&req, 0, sizeof(req));
    req.requestToken = (char *) input->reqtoken;

    // NOTE: only one SURL in the array
    if ((req.arrayOfSURLs =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
        return (-1);
    }

    req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
    req.arrayOfSURLs->urlArray = (char **) input->surls;


    do
    {
        ret = call_function.call_srm2__srmPutDone(context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmPutDoneResponse == NULL)||(ret!=0))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context.current_status = srm_call_status_FAILURE;
            return -1;
        }
        // Check status and wait with back off logic if necessary(Internal_error)
        reqstatp = rep.srmPutDoneResponse->returnStatus;
        internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,reqstatp);

        repfs = rep.srmPutDoneResponse->arrayOfFileStatuses;

    }while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

    switch(internal_context.current_status){

         case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
            {
                errno = srm_call_err(context,reqstatp,srmfunc);
                return (-1);
            }else
            {
                errno = 0;
                internal_context.current_status = srm_call_status_SUCCESS;
                ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
            }
            break;

    }

    return (ret);
}

int srmv2_release_files(struct srm_context *context,
        struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses)
{
    struct srm_internal_context internal_context;
    int ret;
    struct srm2__srmReleaseFilesResponse_ rep;
    struct srm2__ArrayOfTSURLReturnStatus *repfs;
    struct srm2__srmReleaseFilesRequest req;
    struct srm2__TReturnStatus *reqstatp;
    const char srmfunc[] = "ReleaseFiles";

    srm_context_soap_init(context);

    // Setup the timeout
    back_off_logic_init(context,&internal_context);


    memset (&req, 0, sizeof(req));
    req.requestToken = (char *) input->reqtoken;

    // NOTE: only one SURL in the array
    if ((req.arrayOfSURLs =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
        return (-1);
    }

    req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
    req.arrayOfSURLs->urlArray = (char **) input->surls;

    do
    {
        ret = call_function.call_srm2__srmReleaseFiles(context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmReleaseFilesResponse == NULL)||(ret!=0))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context.current_status = srm_call_status_FAILURE;
            return -1;
        }
        // Copy response status

        //internal_context.retstatus = rep.srmReleaseFilesResponse->returnStatus;
        // Check status and wait with back off logic if necessary(Internal_error)
        reqstatp = rep.srmReleaseFilesResponse->returnStatus;
        internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,reqstatp);

        repfs = rep.srmReleaseFilesResponse->arrayOfFileStatuses;

        // Ugly hack for Castor
        if (internal_context.current_status == srm_call_status_INTERNAL_ERROR &&
            strstr(reqstatp->explanation, "Request token") != NULL &&
            strstr(reqstatp->explanation, "not found") != NULL) {
            internal_context.current_status = srm_call_status_FAILURE;
            reqstatp->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
        }

    }while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

    switch(internal_context.current_status){

         case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
            {
                errno = srm_call_err(context,reqstatp,srmfunc);
                return (-1);
            }else
            {
                errno = 0;
                internal_context.current_status = srm_call_status_SUCCESS;
                ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
            }
            break;
    }

    return (ret);
}


int srmv2_bringonline_estimated_wait_time(struct srm2__srmBringOnlineResponse_ *bringOnlineRep){
    if( bringOnlineRep && bringOnlineRep->srmBringOnlineResponse && bringOnlineRep->srmBringOnlineResponse->arrayOfFileStatuses
            && bringOnlineRep->srmBringOnlineResponse->arrayOfFileStatuses->__sizestatusArray > 0
            && bringOnlineRep->srmBringOnlineResponse->arrayOfFileStatuses->statusArray){
        struct  srm2__TBringOnlineRequestFileStatus* status =  *bringOnlineRep->srmBringOnlineResponse->arrayOfFileStatuses->statusArray;
        if(status && status->estimatedWaitTime){
            return *status->estimatedWaitTime;
        }
    }
    return -1;
}



int srmv2_bring_online_async_internal (struct srm_context *context,
        struct srm_bringonline_input *input,
        struct srm_bringonline_output *output,
        struct srm_internal_context *internal_context)
{
    int ret,i;
    struct srm_getbestspacetokens_input spacetokeninput = {0};
    struct srm2__srmBringOnlineResponse_ rep;
    struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
    struct srm2__srmBringOnlineRequest req;
    static enum srm2__TFileStorageType s_types[] = {VOLATILE, DURABLE, PERMANENT};
    char *targetspacetoken;
    const char srmfunc[] = "BringOnline";

    srm_context_soap_init(context);

    // issue "bringonline" request
    memset(output,0,sizeof(*output));
    memset (&req, 0, sizeof(req));

    if ((req.arrayOfFileRequests =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTGetFileRequest))) == NULL ||
            (req.arrayOfFileRequests->requestArray =
             soap_malloc (context->soap, input->nbfiles * sizeof(struct srm2__TGetFileRequest *))) == NULL ||
            (req.transferParameters =
             soap_malloc (context->soap, sizeof(struct srm2__TTransferParameters))) == NULL) {

        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
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
            return (-1);
        }
    }
    // fill storageSystemInfo
    if (input->nbextrainfo > 0) {
        if ((req.storageSystemInfo =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTExtraInfo))) == NULL ||
            (req.storageSystemInfo->extraInfoArray =
             soap_malloc (context->soap, input->nbextrainfo * sizeof(struct srm2__TExtraInfo *))) == NULL )
        {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }

        req.storageSystemInfo->__sizeextraInfoArray = input->nbextrainfo;
        for (i = 0; i < input->nbextrainfo; i++)
        {
            if ((req.storageSystemInfo->extraInfoArray[i] =
                soap_malloc (context->soap, sizeof(struct srm2__TExtraInfo))) == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
            }
            memset (req.storageSystemInfo->extraInfoArray[i], 0, sizeof(struct srm2__TExtraInfo));
            req.storageSystemInfo->extraInfoArray[i]->key = input->extrainfo[i].key;
            req.storageSystemInfo->extraInfoArray[i]->value = input->extrainfo[i].value;
        }
    } else {
        req.storageSystemInfo = NULL;
    }

    req.authorizationID = NULL;
    req.userRequestDescription = NULL;
    req.desiredFileStorageType = &s_types[PERMANENT];
    req.targetFileRetentionPolicyInfo = NULL;
    req.deferredStartTime = NULL;
    if (input->desiredpintime > 0)
        req.desiredLifeTime = &input->desiredpintime;

    if (context->ext && context->ext->turl_resolution_timeout)
        req.desiredTotalRequestTime = &(context->ext->turl_resolution_timeout);

    req.arrayOfFileRequests->__sizerequestArray = input->nbfiles;

    for (i = 0; i < input->nbfiles; i++) {
        if ((req.arrayOfFileRequests->requestArray[i] =
                soap_malloc (context->soap, sizeof(struct srm2__TGetFileRequest))) == NULL) {
            srm_errmsg (context, "[SRM][soap_malloc][] error");
            errno = ENOMEM;
            return (-1);
        }
        memset (req.arrayOfFileRequests->requestArray[i], 0, sizeof(struct srm2__TGetFileRequest));
        req.arrayOfFileRequests->requestArray[i]->sourceSURL = (char *) input->surls[i];
        req.arrayOfFileRequests->requestArray[i]->dirOption = NULL;
    }

    req.transferParameters->accessPattern = NULL;
    req.transferParameters->connectionType = NULL;
    req.transferParameters->arrayOfClientNetworks = NULL;

    ret = srm_set_protocol_in_transferParameters(context, req.transferParameters,input->protocols);

    if (ret)
    {
        return (-1);
    }
    do
    {
        ret = call_function.call_srm2__srmBringOnline (context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmBringOnlineResponse == NULL)||(ret!=0)||
                copy_returnstatus(&output->retstatus,rep.srmBringOnlineResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }

        set_estimated_wait_time(internal_context, srmv2_bringonline_estimated_wait_time(&rep));
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
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
           errno = ETIMEDOUT;
           ret = -1;
           break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }

    return ret;
}


int srmv2_bringonline_status_estimated_wait_time(struct srm2__srmStatusOfBringOnlineRequestResponse_ *bringOnlineStatusRep)
{
    if( bringOnlineStatusRep && bringOnlineStatusRep->srmStatusOfBringOnlineRequestResponse && bringOnlineStatusRep->srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses
         && bringOnlineStatusRep->srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses->__sizestatusArray > 0
         && bringOnlineStatusRep->srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses->statusArray){
        struct srm2__TBringOnlineRequestFileStatus* status = *bringOnlineStatusRep->srmStatusOfBringOnlineRequestResponse->arrayOfFileStatuses->statusArray;
        if(status && status->estimatedWaitTime){
            return *status->estimatedWaitTime;
        }
    }
    return -1;
}


int srmv2_status_of_bring_online_async_internal (struct srm_context *context,
        struct srm_bringonline_input *input,
        struct srm_bringonline_output *output,
        struct srm_internal_context *internal_context)
{
    int ret;
    struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs;
    struct srm2__srmStatusOfBringOnlineRequestResponse_ srep;
    struct srm2__srmStatusOfBringOnlineRequestRequest sreq;
    const char srmfunc[] = "StatusOfBringOnlineRequest";

    srm_context_soap_init(context);

    memset (&sreq, 0, sizeof(sreq));
    sreq.requestToken = (char *) output->token;
    if (input->surls) {
        sreq.arrayOfSourceSURLs = soap_malloc(context->soap, sizeof(struct srm2__ArrayOfAnyURI));
        sreq.arrayOfSourceSURLs->__sizeurlArray = input->nbfiles;
        sreq.arrayOfSourceSURLs->urlArray = input->surls;
    }
    output->retstatus = NULL;
    output->filestatuses = NULL;


    do
    {
        ret = call_function.call_srm2__srmStatusOfBringOnlineRequest(context->soap, context->srm_endpoint, srmfunc, &sreq, &srep);
        // If no response break with failure
        if ((srep.srmStatusOfBringOnlineRequestResponse == NULL)||(ret!=0)||
                copy_returnstatus(&output->retstatus,srep.srmStatusOfBringOnlineRequestResponse->returnStatus))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context->current_status = srm_call_status_FAILURE;
            return -1;
        }
        set_estimated_wait_time(internal_context, srmv2_bringonline_status_estimated_wait_time(&srep));
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
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
            errno = ETIMEDOUT;
            ret = -1;
            break;
        default:
            errno = srm_call_err(context,output->retstatus,srmfunc);
            ret = -1;
            break;
    }

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
    const char srmfunc[] = "AbortFiles";

    srm_context_soap_init(context);

    // Setup the timeout
    back_off_logic_init(context,&internal_context);

    memset (&req, 0, sizeof(req));

    req.requestToken = (char *) input->reqtoken;

    // NOTE: only one SURL in the array
    if ((req.arrayOfSURLs =
                soap_malloc (context->soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
        srm_errmsg (context, "[SRM][soap_malloc][] error");
        errno = ENOMEM;
        return (-1);
    }

    req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
    req.arrayOfSURLs->urlArray = (char **) input->surls;

    do
    {
        ret = call_function.call_srm2__srmAbortFiles (context->soap, context->srm_endpoint, srmfunc, &req, &rep);
        // If no response break with failure
        if ((rep.srmAbortFilesResponse == NULL)||(ret!=0))
        {
            errno = srm_soap_call_err(context, srmfunc);
            internal_context.current_status = srm_call_status_FAILURE;
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
                return (-1);
            }else
            {
                errno = 0;
                internal_context.current_status = srm_call_status_SUCCESS;
                ret = copy_filestatuses(reqstatp,statuses,repfs,srmfunc);
            }
            break;
        case srm_call_status_TIMEOUT: // add timeout management for backoff logic
           errno = ETIMEDOUT;
           ret = -1;
           break;
        default:
            ret = -1;
            break;
    }

    return (ret);
}

int srmv2_abort_request(struct srm_context *context,char *token)
{
    const char srmfunc[] = "AbortRequest";
    struct srm2__srmAbortRequestRequest abortreq;
    struct srm2__srmAbortRequestResponse_ abortrep;
    int result;

    srm_context_soap_init(context);

    memset (&abortreq, 0, sizeof(abortreq));

    if (token == NULL)
    {
        // No token supplied
        errno = EINVAL;
        return (-1);
    }else
    {
        abortreq.requestToken = token;

        result = call_function.call_srm2__srmAbortRequest (context->soap, context->srm_endpoint, srmfunc, &abortreq, &abortrep);

        if (result != 0)
        {
            // Soap call failure
            errno = srm_soap_call_err(context, srmfunc);
        }else
        {
            if (abortrep.srmAbortRequestResponse == NULL ||
                    abortrep.srmAbortRequestResponse->returnStatus == NULL ||
                    abortrep.srmAbortRequestResponse->returnStatus->statusCode != SRM_USCORESUCCESS)
            {
                errno = EINVAL;
                return (-1);
            }
        }
    }

    return result;
}
