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

#include "srmv2_permission_functions.h"
#include "srm_soap.h"



int srmv2_set_permission(struct srm_context *context,
		struct srm_setpermission_input *input)
{
	int result = 0,i;
	const char srmfunc[] = "SetPermission";
	struct srm2__srmSetPermissionRequest req;
	struct srm2__srmSetPermissionResponse_ rep;
	enum srm2__TPermissionMode otherPermission;
	enum srm2__TPermissionMode ownerPermission;

    srm_context_soap_init(context);

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfGroupPermissions =
				soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTGroupPermission))) == NULL ||
			(req.arrayOfGroupPermissions->groupPermissionArray =
			 soap_malloc (context->soap, input->group_permissions_count* sizeof(struct srm2__TGroupPermission *))) == NULL ||
			(req.arrayOfUserPermissions =
			 soap_malloc (context->soap, sizeof(struct srm2__ArrayOfTUserPermission))) == NULL ||
			(req.arrayOfUserPermissions->userPermissionArray =
			 soap_malloc (context->soap,input->user_permissions_count* sizeof(struct srm2__TUserPermission*))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		return (-1);
	}
	req.arrayOfGroupPermissions->__sizegroupPermissionArray = input->group_permissions_count;
	req.arrayOfUserPermissions->__sizeuserPermissionArray = input->user_permissions_count;

	for (i = 0; i < input->group_permissions_count; i++) {
		if ((req.arrayOfGroupPermissions->groupPermissionArray[i] =
					soap_malloc (context->soap, sizeof(struct srm2__TGroupPermission))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			return (-1);
		}
		memset (req.arrayOfGroupPermissions->groupPermissionArray[i], 0, sizeof(struct srm2__TGroupPermission));
		req.arrayOfGroupPermissions->groupPermissionArray[i]->groupID = (char *)input->group_permissions[i].name_id;
		req.arrayOfGroupPermissions->groupPermissionArray[i]->mode = input->group_permissions[i].mode;
	}
	for (i = 0; i < input->user_permissions_count; i++) {
		if ((req.arrayOfUserPermissions->userPermissionArray[i] =
					soap_malloc (context->soap, sizeof(struct srm2__TUserPermission))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			return (-1);
		}
		memset (req.arrayOfUserPermissions->userPermissionArray[i], 0, sizeof(struct srm2__TUserPermission));
		req.arrayOfUserPermissions->userPermissionArray[i]->userID = (char *)input->user_permissions[i].name_id;
		req.arrayOfUserPermissions->userPermissionArray[i]->mode = input->user_permissions[i].mode;
	}

	req.SURL = input->surl;
	req.storageSystemInfo = NULL;
	req.authorizationID = NULL;
	otherPermission = (enum srm2__TPermissionMode)input->other_permission;
	ownerPermission = (enum srm2__TPermissionMode)input->owner_permission;
	req.otherPermission = &otherPermission;
	req.ownerPermission = &ownerPermission;
	req.permissionType =  input->permission_type;


	result = call_function.call_srm2__srmSetPermission (context->soap, context->srm_endpoint, srmfunc, &req, &rep);

	if (result != 0||
			rep.srmSetPermissionResponse== NULL ||
			rep.srmSetPermissionResponse->returnStatus == NULL)
	{
		// Soap call failure
		errno = srm_soap_call_err(context, srmfunc);
		result = -1;
	}else
	{
		// check response
		if (rep.srmSetPermissionResponse->returnStatus->statusCode != SRM_USCORESUCCESS)
		{
			errno = statuscode2errno(rep.srmSetPermissionResponse->returnStatus->statusCode);
			return (-1);
		}
	}

	return result;
}

int srmv2_get_permission(struct srm_context *context,
		struct srm_getpermission_input *input,
		struct srm_getpermission_output *output)
{
	const char srmfunc[] = "GetPermission";
	struct srm2__srmGetPermissionRequest req;
	struct srm2__srmGetPermissionResponse_ rep;
	struct srm2__ArrayOfTPermissionReturn *repperm;
	int result = 0;

	srm_context_soap_init(context);

	memset (&req, 0, sizeof(req));
	memset(output,0,sizeof(*output));

	if ((req.arrayOfSURLs = soap_malloc (context->soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		return (-1);
	}
	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **)input->surls;
	req.authorizationID = NULL;
	req.storageSystemInfo = NULL;

	result = call_function.call_srm2__srmGetPermission (context->soap, context->srm_endpoint, srmfunc, &req, &rep);

	// check response
	if (result != 0 ||
		rep.srmGetPermissionResponse == NULL ||
		copy_returnstatus(&output->retstatus,rep.srmGetPermissionResponse->returnStatus))
	{
		// Soap call failure
		errno = srm_soap_call_err(context, srmfunc);
		result = -1;
	}else
	{
		repperm = rep.srmGetPermissionResponse->arrayOfPermissionReturns;
		if (output->retstatus->statusCode == SRM_USCORESUCCESS ||
				output->retstatus->statusCode == SRM_USCOREPARTIAL_USCORESUCCESS)
		{
			result = copy_filepermissions(output->retstatus,
					&output->permissions,
					repperm);
		}else
		{
			errno = srm_call_err(context,output->retstatus,srmfunc);
			result = -1;
		}
	}

	return result;
}


int srmv2_check_permission(struct srm_context *context,
		struct srm_checkpermission_input *input,
		struct srmv2_filestatus **statuses)
{
	int ret;
	struct srm2__srmCheckPermissionResponse_ rep;
	struct srm2__ArrayOfTSURLPermissionReturn *repfs;
	struct srm2__srmCheckPermissionRequest req;
	struct srm2__TReturnStatus *reqstatp;
	const char srmfunc[] = "CheckPermission";

	srm_context_soap_init(context);

	memset (&req, 0, sizeof(req));

	/* NOTE: only one SURL in the array */
	if ((req.arrayOfSURLs =
				soap_malloc (context->soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	if (call_function.call_srm2__srmCheckPermission (context->soap, context->srm_endpoint, srmfunc, &req, &rep) != 0)
	{
		errno = srm_soap_call_err(context, srmfunc);
		return (-1);
	}

	if (rep.srmCheckPermissionResponse == NULL || (reqstatp = rep.srmCheckPermissionResponse->returnStatus) == NULL)
	{
		srm_errmsg (context, "[SRM][%s][] %s: <empty response>",srmfunc, context->srm_endpoint);
		errno = ECOMM;
		return (-1);
	}

	repfs = rep.srmCheckPermissionResponse->arrayOfPermissions;

	if (!repfs || repfs->__sizesurlPermissionArray < 1 || !repfs->surlPermissionArray)
	{
		errno = srm_soap_call_err(context, srmfunc);
		return (-1);
	}

	ret = copy_permissionfilestatuses(reqstatp,
									statuses,
									repfs,
									srmfunc,
									input->amode);
	return (ret);

}
