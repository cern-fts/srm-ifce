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
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"

int srmv2_getspacemd (struct srm_context *context,
		struct srm_getspacemd_input *input,struct srm_spacemd **spaces)
{
	int flags;
	int sav_errno = 0;
	int i, ret;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmGetSpaceMetaDataResponse_ tknrep;
	struct srm2__srmGetSpaceMetaDataRequest tknreq;
	struct srm2__TReturnStatus *tknrepstatp = NULL;
	struct srm2__ArrayOfTMetaDataSpace *tknrepp;
	const char srmfunc[] = "GetSpaceMetaData";

	if (input->nbtokens < 1 || input->spacetokens == NULL || context->srm_endpoint == NULL || spaces == NULL)
	{
		srm_errmsg (context, "[SRM][srmv2_getspacemd][EINVAL] Invalid arguments");
		errno = EINVAL;
		srm_soap_free(soap);
		return (-1);
	}

	if (input->spacetokens[input->nbtokens] != NULL)
	{
		srm_errmsg (context, "[SRM][srmv2_getspacemd][EINVAL] Invalid space token number");
		errno = EINVAL;
		srm_soap_free(soap);	
		return (-1);
	}


	memset (&tknreq, 0, sizeof(tknreq));

	if ((tknreq.arrayOfSpaceTokens =
				soap_malloc (soap, input->nbtokens * sizeof(struct srm2__ArrayOfString))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	tknreq.arrayOfSpaceTokens->__sizestringArray = input->nbtokens;
	tknreq.arrayOfSpaceTokens->stringArray = input->spacetokens;

	if ((ret = call_function.call_srm2__srmGetSpaceMetaData(soap, context->srm_endpoint, srmfunc, &tknreq, &tknrep)))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (tknrep.srmGetSpaceMetaDataResponse == NULL ||
			(tknrepstatp = tknrep.srmGetSpaceMetaDataResponse->returnStatus) == NULL) {
		errno = srm_call_err(context,tknrepstatp,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (tknrepstatp->statusCode != SRM_USCORESUCCESS)
	{
		errno = srm_print_error_status(context,tknrepstatp,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	tknrepp = tknrep.srmGetSpaceMetaDataResponse->arrayOfSpaceDetails;

	if (!tknrepp)
	{
		srm_errmsg (context, "[%s][%s][] %s: <empty response>",
				err_msg_begin,srmfunc, context->srm_endpoint);
		errno = ECOMM;
		srm_soap_free(soap);
		return (-1);
	}
	if (tknrepp->__sizespaceDataArray < 1 || !tknrepp->spaceDataArray) {
		srm_errmsg (context, "[%s][%s][] %s: no valid space tokens",
				err_msg_begin,srmfunc, context->srm_endpoint);
		srm_soap_free(soap);
		errno = EINVAL;
		return (-1);
	}

	if ((*spaces = (struct srm_spacemd *) calloc (input->nbtokens, sizeof (struct srm_spacemd))) == NULL) {
		srm_soap_free(soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < input->nbtokens; i++)
	{
		if (!tknrepp->spaceDataArray[i] || !tknrepp->spaceDataArray[i]->spaceToken)
			continue;
		if (tknrepp->spaceDataArray[i]->status &&
				tknrepp->spaceDataArray[i]->status->statusCode != SRM_USCORESUCCESS) {
			errno = srm_call_err(context,tknrepp->spaceDataArray[i]->status,srmfunc);
			srm_soap_free(soap);
			return (-1);
		}
		(*spaces)[i].spacetoken = strdup (tknrepp->spaceDataArray[i]->spaceToken);
		if (tknrepp->spaceDataArray[i]->owner)
			(*spaces)[i].owner = strdup (tknrepp->spaceDataArray[i]->owner);
		if (tknrepp->spaceDataArray[i]->totalSize)
			(*spaces)[i].totalsize = (SRM_LONG64) *(tknrepp->spaceDataArray[i]->totalSize);
		if (tknrepp->spaceDataArray[i]->guaranteedSize)
			(*spaces)[i].guaranteedsize = (SRM_LONG64) *(tknrepp->spaceDataArray[i]->guaranteedSize);
		if (tknrepp->spaceDataArray[i]->unusedSize)
			(*spaces)[i].unusedsize = (SRM_LONG64) *(tknrepp->spaceDataArray[i]->unusedSize);
		if (tknrepp->spaceDataArray[i]->lifetimeAssigned)
			(*spaces)[i].lifetimeassigned = *(tknrepp->spaceDataArray[i]->lifetimeAssigned);
		if (tknrepp->spaceDataArray[i]->lifetimeLeft)
			(*spaces)[i].lifetimeleft = *(tknrepp->spaceDataArray[i]->lifetimeLeft);
		if (tknrepp->spaceDataArray[i]->retentionPolicyInfo) {
			switch (tknrepp->spaceDataArray[i]->retentionPolicyInfo->retentionPolicy) {
				case REPLICA:
					(*spaces)[i].retentionpolicy = GFAL_POLICY_REPLICA;
					break;
				case OUTPUT:
					(*spaces)[i].retentionpolicy = GFAL_POLICY_OUTPUT;
					break;
				case CUSTODIAL:
					(*spaces)[i].retentionpolicy = GFAL_POLICY_CUSTODIAL;
					break;
				default:
					(*spaces)[i].retentionpolicy = GFAL_POLICY_UNKNOWN;
			}

			if (tknrepp->spaceDataArray[i]->retentionPolicyInfo->accessLatency)
			{
				switch (*(tknrepp->spaceDataArray[i]->retentionPolicyInfo->accessLatency)) {
					case ONLINE:
						(*spaces)[i].accesslatency = GFAL_LATENCY_ONLINE;
						break;
					case NEARLINE:
						(*spaces)[i].accesslatency = GFAL_LATENCY_NEARLINE;
						break;
					default:
						(*spaces)[i].accesslatency = GFAL_LATENCY_UNKNOWN;
				}
			}
		}
	}

	srm_soap_free(soap);
    errno = 0;
	return (0);
}
// returns space tokens associated to the space description
int srmv2_getspacetokens (struct srm_context *context,
		struct srm_getspacetokens_input *input,
		struct srm_getspacetokens_output *output)
{
	int flags;
	int sav_errno = 0;
	int i, ret;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmGetSpaceTokensResponse_ tknrep;
	struct srm2__srmGetSpaceTokensRequest tknreq;
	struct srm2__TReturnStatus *tknrepstatp = NULL;
	struct srm2__ArrayOfString *tknrepp;
	const char srmfunc[] = "GetSpaceTokens";

	if (input == NULL || 
        input->spacetokendesc == NULL || 
        context->srm_endpoint == NULL || 
        output == NULL)
	{
		srm_errmsg( context, "[SRM][srmv2_getspacetokens][EINVAL] Invalid arguments");
		errno = EINVAL;
		return (-1);
	}

	memset(output,0,sizeof(*output));
	memset (&tknreq, 0, sizeof(tknreq));

	tknreq.userSpaceTokenDescription = (char *) input->spacetokendesc;

	if ((ret = call_function.call_srm2__srmGetSpaceTokens (soap, context->srm_endpoint, srmfunc, &tknreq, &tknrep)))
	{
		srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (tknrep.srmGetSpaceTokensResponse == NULL ||
			(tknrepstatp = tknrep.srmGetSpaceTokensResponse->returnStatus) == NULL) {
		errno = srm_call_err(context,tknrepstatp,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (tknrepstatp->statusCode != SRM_USCORESUCCESS)
	{

		errno = srm_print_error_status(context,tknrepstatp,srmfunc);;
		srm_soap_free(soap);
		return (-1);
	}

	tknrepp = tknrep.srmGetSpaceTokensResponse->arrayOfSpaceTokens;

	if (!tknrepp) {
		srm_errmsg (context, "[SE][%s][%s] %s: <empty response>",
				 srmfunc, statuscode2errmsg (tknrepstatp->statusCode), context->srm_endpoint);
		srm_soap_free(soap);
		errno = ECOMM;
		return (-1);
	}

	output->nbtokens = tknrepp->__sizestringArray;
	if (output->nbtokens < 1 || !tknrepp->stringArray) {
		srm_errmsg (context, "[%s][%s][%s] %s: %s: No such space token descriptor",
				err_msg_begin,srmfunc, statuscode2errmsg (tknrepstatp->statusCode),
				context->srm_endpoint,input->spacetokendesc);
		srm_soap_free(soap);
		errno = EINVAL;
		return (-1);
	}

	if ((output->spacetokens = (char **) calloc (output->nbtokens + 1, sizeof (char *))) == NULL) {
		srm_soap_free(soap);
		errno = ENOMEM;
		return (-1);
	}

	for (i = 0; i < output->nbtokens; ++i)
		output->spacetokens[i] = strdup(tknrepp->stringArray[i]);

	srm_soap_free(soap);
    errno = 0;
	return (0);
}
// returns best space token
char* srmv2_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input)
{
	struct srm_getspacetokens_input getspacetoken_input;
	struct srm_getspacetokens_output getspacetoken_output;
	struct srm_getspacemd_input getspacemd_input;;
	int sav_errno = 0;
	int i, ret, numtoken = -1;
	SRM_LONG64 unusedsize = -1;
	char **spacetokens = NULL;
	struct srm_spacemd *spacemd = NULL;
	char *spacetoken = NULL;

	getspacetoken_input.spacetokendesc = input->spacetokendesc;
	ret = srmv2_getspacetokens(context,&getspacetoken_input,&getspacetoken_output);

	if (ret < 0 || getspacetoken_output.spacetokens == NULL
				|| getspacetoken_output.nbtokens < 1)
	{
		errno = ret == 0 ? ENOMEM : errno;
		return (NULL);
	}

   if (getspacetoken_output.nbtokens == 1)
   {
        errno = 0;
        spacetoken = getspacetoken_output.spacetokens[0];
        free (getspacetoken_output.spacetokens);
        return (spacetoken);
    }

    getspacemd_input.nbtokens = getspacetoken_output.nbtokens;
    getspacemd_input.spacetokens = getspacetoken_output.spacetokens;
	ret = srmv2_getspacemd (context,&getspacemd_input,&spacemd);
	if (ret < 0 || spacemd == NULL) {
		sav_errno = ret == 0 ? ENOMEM : errno;

		for (i = 0; i < getspacetoken_output.nbtokens; ++i)
			if (getspacetoken_output.spacetokens[i]) free (getspacetoken_output.spacetokens[i]);

		free (getspacetoken_output.spacetokens);
		errno = sav_errno;
		return (NULL);
	}

	// Get the spacetoken with the least free space, but a bit more than needed
	for (i = 0; i < getspacetoken_output.nbtokens; ++i) {
		if (getspacetoken_output.spacetokens[i]) free (getspacetoken_output.spacetokens[i]);
		if (spacemd[i].unusedsize < input->neededsize + SRM_SIZE_MARGIN)
			continue;
		if (numtoken < 0 || spacemd[i].unusedsize < unusedsize) {
			numtoken = i;
			unusedsize = spacemd[i].unusedsize;
		}
	}

	if (numtoken < 0) {
		// no suitable space token
		srm_errmsg (context,"[SRM][srmv2_getbestspacetoken][EINVAL] %s: no associated space token with enough free space",
				input->spacetokendesc);
		errno = EINVAL;
		return (NULL);
	}

	spacetoken = spacemd[numtoken].spacetoken;
	spacemd[numtoken].spacetoken = NULL;  // prevent it to be freed
	srm_spacemd_free (getspacetoken_output.nbtokens, spacemd);
	free (spacetokens);
    errno = 0;
	return (spacetoken);
}

// returns space tokens associated to the space description
int srmv2_reservespace(struct srm_context *context,
		struct srm_reservespace_input *input,
		struct srm_reservespace_output *output)
{
	int flags;
	int sav_errno = 0;
	int i, ret;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmReserveSpaceResponse_ rep;
	struct srm2__srmReserveSpaceRequest req;
	struct srm2__TReturnStatus *repstatp;
	struct srm2__ArrayOfString *repp;
	struct srm2__TRetentionPolicyInfo retentionPolicy;
	const char srmfunc[] = "ReserveSpace";

	if (input->spacetokendescriptor == NULL)
	{
		srm_errmsg( context, "[SRM][%s][EINVAL] Invalid arguments",srmfunc);
		errno = EINVAL;
		return (-1);
	}

	

	memset(output,0,sizeof(*output));
	memset (&req, 0, sizeof(req));

	req.userSpaceTokenDescription = input->spacetokendescriptor;
	req.desiredSizeOfTotalSpace  = NULL;
	req.desiredSizeOfGuaranteedSpace = input->desired_size;
	retentionPolicy.accessLatency = NULL;
	retentionPolicy.retentionPolicy = 0;
	req.retentionPolicyInfo = &retentionPolicy;
	req.desiredLifetimeOfReservedSpace =  &input->desired_lifetime;

	if ((ret = call_function.call_srm2__srmReserveSpace(soap, context->srm_endpoint, srmfunc, &req, &rep)))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (copy_string(&output->spacetoken,rep.srmReserveSpaceResponse->spaceToken))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if (output->spacetoken != NULL)
	{
		//printf("Reserved Space token: %s \n",output->spacetoken);
	}
	srm_soap_free(soap);
    errno = 0;
	return (0);
}

int srmv2_releasespace(struct srm_context *context,
		char *spacetoken)
{
	int flags;
	int sav_errno = 0;
	int i, ret;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmReleaseSpaceResponse_ rep;
	struct srm2__srmReleaseSpaceRequest req;
	struct srm2__TReturnStatus *repstatp;
	const char srmfunc[] = "ReserveSpace";

	if (spacetoken == NULL)
	{
		srm_errmsg( context, "[SRM][%s][EINVAL] Invalid arguments",srmfunc);
		errno = EINVAL;
		return (-1);
	}

	

	memset (&req, 0, sizeof(req));

	req.spaceToken = spacetoken;

	if ((ret = call_function.call_srm2__srmReleaseSpace(soap, context->srm_endpoint, srmfunc, &req, &rep)))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}
	srm_soap_free(soap);
    errno = 0;
	return (0);
}

// returns space tokens associated to the space description
int srmv2_purgefromspace(struct srm_context *context,
		struct srm_purgefromspace_input *input,
		struct srm_purgefromspace_output *output)
{
	int n,i,ret;
    struct soap* soap = srm_soap_init_context_new(context);
	struct srm2__srmPurgeFromSpaceResponse_ rep;
	struct srm2__srmPurgeFromSpaceRequest req;
	struct srm2__TReturnStatus *repstatp;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;

	const char srmfunc[] = "PurgeFromSpace";

	if (input->spacetoken == NULL)
	{
		srm_errmsg( context, "[SRM][%s][EINVAL] Invalid arguments",srmfunc);
		errno = EINVAL;
		return (-1);
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
	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **)input->surls;
	req.spaceToken = input->spacetoken;

	if ((ret = call_function.call_srm2__srmPurgeFromSpace(soap, context->srm_endpoint, srmfunc, &req, &rep)))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	if ((rep.srmPurgeFromSpaceResponse == NULL)||(ret!=0)||
					copy_returnstatus(&output->retstatus,rep.srmPurgeFromSpaceResponse->returnStatus))
	{
		errno = srm_soap_call_err(context,soap,srmfunc);
		srm_soap_free(soap);
		return -1;
	}


	repfs = rep.srmPurgeFromSpaceResponse->arrayOfFileStatuses;

	if (output->retstatus->statusCode != SRM_USCORESUCCESS || !repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray)
	{
		errno = srm_call_err(context,output->retstatus,srmfunc);
		srm_soap_free(soap);
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((output->statuses = (struct srmv2_filestatus*) calloc (n, sizeof (struct srmv2_filestatus))) == NULL)
	{
		errno = ENOMEM;
		srm_soap_free(soap);
		return (-1);
	}

	for (i = 0; i < n; ++i)
	{
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
