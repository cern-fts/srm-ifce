#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int srmv2_set_permission(struct srm_context *context,
		struct srm_setpermission_input *input)
{
	const char srmfunc[] = "SetPermission";
	struct srm2__srmSetPermissionRequest req;
	struct srm2__srmSetPermissionResponse_ rep;
	struct soap soap;
	int result = 0,i;


	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfGroupPermissions =
				soap_malloc (&soap, sizeof(struct srm2__ArrayOfTGroupPermission))) == NULL ||
			(req.arrayOfGroupPermissions->groupPermissionArray =
			 soap_malloc (&soap, input->group_permissions_count* sizeof(struct srm2__TGroupPermission *))) == NULL ||
			(req.arrayOfUserPermissions =
			 soap_malloc (&soap, sizeof(struct srm2__ArrayOfTUserPermission))) == NULL ||
			(req.arrayOfUserPermissions->userPermissionArray =
			 soap_malloc (&soap,input->user_permissions_count* sizeof(struct srm2__TUserPermission*))) == NULL) {

		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}
	req.arrayOfGroupPermissions->__sizegroupPermissionArray = input->group_permissions_count;
	req.arrayOfUserPermissions->__sizeuserPermissionArray = input->user_permissions_count;

	for (i = 0; i < input->group_permissions_count; i++) {
		if ((req.arrayOfGroupPermissions->groupPermissionArray[i] =
					soap_malloc (&soap, sizeof(struct srm2__TGroupPermission))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		memset (req.arrayOfGroupPermissions->groupPermissionArray[i], 0, sizeof(struct srm2__TGroupPermission));
		req.arrayOfGroupPermissions->groupPermissionArray[i]->groupID = (char *)input->group_permissions[i].name_id;
		req.arrayOfGroupPermissions->groupPermissionArray[i]->mode = input->group_permissions[i].mode;
	}
	for (i = 0; i < input->user_permissions_count; i++) {
		if ((req.arrayOfUserPermissions->userPermissionArray[i] =
					soap_malloc (&soap, sizeof(struct srm2__TUserPermission))) == NULL) {
			srm_errmsg (context, "[SRM][soap_malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		memset (req.arrayOfUserPermissions->userPermissionArray[i], 0, sizeof(struct srm2__TUserPermission));
		req.arrayOfUserPermissions->userPermissionArray[i]->userID = (char *)input->user_permissions[i].name_id;
		req.arrayOfUserPermissions->userPermissionArray[i]->mode = input->user_permissions[i].mode;
	}

	req.SURL = input->surl;
	req.storageSystemInfo = NULL;
	req.authorizationID = NULL;
	req.otherPermission = &input->other_permission;
	req.ownerPermission = &input->owner_permission;
	req.permissionType =  input->permission_type;


	result = call_function.call_srm2__srmSetPermission (&soap, context->srm_endpoint, srmfunc, &req, &rep);

	if (result != 0)
	{
		// Soap call failure
		errno = srm_soup_call_err(context,&soap,srmfunc);
	}else
	{
		// check response
		if (rep.srmSetPermissionResponse == NULL)
		{
			errno = EINVAL;
			srm_soap_deinit(&soap);
			return (-1);
		}
	}

	srm_soap_deinit(&soap);

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
	struct soap soap;
	int result = 0;

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfSURLs = soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}
	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **)input->surls;
	req.authorizationID = NULL;
	req.storageSystemInfo = NULL;



	result = call_function.call_srm2__srmGetPermission (&soap, context->srm_endpoint, srmfunc, &req, &rep);

	// check response
	if (result != 0 ||
		rep.srmGetPermissionResponse == NULL ||
		copy_returnstatus(&output->retstatus,rep.srmGetPermissionResponse->returnStatus))
	{
		// Soap call failure
		errno = srm_soup_call_err(context,&soap,srmfunc);
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
			result = -1;
		}
	}

	srm_soap_deinit(&soap);

	return result;
}
