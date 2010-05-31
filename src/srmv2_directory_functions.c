#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"

int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output);
int srmv2_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output);
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input);


// Asynchronous srm ls call
int srmv2_ls_async_internal(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output,struct srm_internal_context *internal_context)
{
	int n,ret;
	enum xsd__boolean trueoption = true_;
	struct soap soap;
	const char srmfunc[] = "Ls";
	struct srm2__srmLsRequest req;
	struct srm2__srmLsResponse_ rep;
	struct srm2__ArrayOfTMetaDataPathDetail *repfs = NULL;
	int file_count = 0;
	char *files;
	srm_call_status current_status;

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	if ((req.arrayOfSURLs = soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL)
	{
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	req.fullDetailedList = &trueoption;
	req.numOfLevels = &(input->numlevels);

	if (&(input->offset) && (input->offset > 0))
	{
		req.offset = &(input->offset);
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
		ret = call_function.call_srm2__srmLs(&soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmLsResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmLsResponse->returnStatus))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_deinit(&soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context->current_status = back_off_logic(context,srmfunc,internal_context,output->retstatus);



	}while (internal_context->current_status == srm_call_status_INTERNAL_ERROR);


	switch (internal_context->current_status)
	{
		case srm_call_status_QUEUED:
			if (copy_token(&output->token,rep.srmLsResponse->requestToken))
			{
				internal_context->current_status = srm_call_status_FAILURE;
				srm_soap_deinit(&soap);
				return -1;
			}
			break;
		case srm_call_status_SUCCESS:
			// Copy file structure to another pointer for easier manipulation
			repfs = rep.srmLsResponse->details;
			// Check if file structure ok
			if (!repfs || repfs->__sizepathDetailArray <= 0 || !repfs->pathDetailArray)
			{
				// file list empty error
				internal_context->current_status  = srm_call_status_FAILURE;
				ret = -1;
			}else
			{
				// Everything is fine copy file structure and check if copy went ok
				n = copy_mdfilestatuses(output->retstatus, output->statuses,repfs);
				if (n == -1)
				{
					errno = srm_call_err(context,output->retstatus,srmfunc);
					internal_context->current_status  = srm_call_status_FAILURE;
					ret = -1;
				}
			}
			break;
		default:
			ret = -1;
			break;
	}

	srm_soap_deinit(&soap);
	return ret;
}

int srmv2_status_of_ls_request_async_internal(struct srm_context *context,
		struct srm_ls_input *input,
		struct srm_ls_output *output,
		struct srm_internal_context *internal_context)
{
	const char srmfunc[] = "StatusOfLsRequest";
	struct soap soap;
	struct srm2__srmStatusOfLsRequestResponse_ srep;
	struct srm2__srmStatusOfLsRequestRequest sreq;
	struct srm2__ArrayOfTMetaDataPathDetail *repfs = NULL;
	int ret,n;

	// wait for files ready

	srm_soap_init(&soap);

	memset (&sreq, 0, sizeof(sreq));
	sreq.requestToken = output->token;
	internal_context->current_status = srm_call_status_FAILURE;

	do
	{
		ret = call_function.call_srm2__srmStatusOfLsRequest (&soap, context->srm_endpoint, srmfunc, &sreq, &srep);
		// If no response break with failure
		if ((srep.srmStatusOfLsRequestResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,srep.srmStatusOfLsRequestResponse->returnStatus))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context->current_status = srm_call_status_FAILURE;
			srm_soap_deinit(&soap);
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
			// Check if file structure ok
			if (!repfs || repfs->__sizepathDetailArray <= 0 || !repfs->pathDetailArray)
			{
				// file list empty error
				errno = srm_call_err(context,output->retstatus,srmfunc);
				internal_context->current_status  = srm_call_status_FAILURE;
				ret = -1;
			}else
			{
				// Everything is fine copy file structure and check if copy went ok
				n = copy_mdfilestatuses(output->retstatus, output->statuses,repfs );
				if (n == -1)
				{
					errno = srm_call_err(context,output->retstatus,srmfunc);
					internal_context->current_status  = srm_call_status_FAILURE;
					ret = -1;
				}else
				{
					// TODO check this code:

				/*	if (n == 1 && input->offset && output->retstatus->statusCode == SRM_USCORETOO_USCOREMANY_USCORERESULTS &&
							repfs->pathDetailArray[0] != NULL && repfs->pathDetailArray[0]->arrayOfSubPaths != NULL)
					{
						// offset is only supported for a single directory listing
						input->offset += repfs->pathDetailArray[0]->arrayOfSubPaths->__sizepathDetailArray;
					}else if (input->offset)
					{
						input->offset = 0;
					}*/
				}
			}
			break;
		case srm_call_status_QUEUED:
			break;
		default:
			ret = -1;
			break;
	}

	srm_soap_deinit(&soap);
	return (ret);
}



int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output)
{
	struct srm2__srmRmResponse_ rep;
	struct srm2__ArrayOfTSURLReturnStatus *repfs;
	struct srm2__srmRmRequest req;
	//struct srm2__TReturnStatus *reqstatp;
	struct soap soap;
	const char srmfunc[] = "srmRm";
	struct srm_internal_context internal_context;
	int i,n,ret;

	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	// NOTE: only one file in the array
	if ((req.arrayOfSURLs = soap_malloc (&soap, sizeof(struct srm2__ArrayOfAnyURI))) == NULL) {
		srm_errmsg (context, "[SRM][soap_malloc][] error");
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	req.arrayOfSURLs->__sizeurlArray = input->nbfiles;
	req.arrayOfSURLs->urlArray = (char **) input->surls;

	// issue "srmRm" request
	do
	{
		// Gsoap call
		ret = call_function.call_srm2__srmRm (&soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmRmResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmRmResponse->returnStatus))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_deinit(&soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,output->retstatus);

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	repfs = rep.srmRmResponse->arrayOfFileStatuses;

	if (!repfs || repfs->__sizestatusArray < 1 || !repfs->statusArray) {
		errno = srm_call_err(context,output->retstatus,srmfunc);
		srm_soap_deinit(&soap);
		return (-1);
	}

	n = repfs->__sizestatusArray;

	if ((*output->statuses = (struct srmv2_filestatus*) calloc (n, sizeof (struct srmv2_filestatus))) == NULL) {
		errno = ENOMEM;
		srm_soap_deinit(&soap);
		return (-1);
	}

	for (i = 0; i < n; ++i) {
		if (!repfs->statusArray[i])
			continue;
		if (repfs->statusArray[i]->surl)
			(*output->statuses)[i].surl = strdup (repfs->statusArray[i]->surl);
		if (repfs->statusArray[i]->status)
		{
			(*output->statuses)[i].status = statuscode2errno(repfs->statusArray[i]->status->statusCode);
			srm_print_explanation(&((*output->statuses)[i].explanation), repfs->statusArray[i]->status,srmfunc);
		}
	}

	srm_soap_deinit(&soap);
    errno = 0;
	return 0;
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
	struct soap soap;
	const char srmfunc[] = "srmRmdir";

	back_off_logic_init(context,&internal_context);

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));
	req.SURL = (char *) input->surl;
	if (input->recursive)
	{
		req.recursive = &trueoption;
	}

	// issue "srmRmdir" request
	do
	{
		// Gsoap call
		ret = call_function.call_srm2__srmRmdir(&soap,context->srm_endpoint, srmfunc, &req, &rep);
		// If no response break with failure
		if ((rep.srmRmdirResponse == NULL)||(ret!=0)||
				copy_returnstatus(&output->retstatus,rep.srmRmdirResponse->returnStatus))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			internal_context.current_status = srm_call_status_FAILURE;
			srm_soap_deinit(&soap);
			return -1;
		}
		// Check status and wait with back off logic if necessary(Internal_error)
		internal_context.current_status = back_off_logic(context,srmfunc,&internal_context,output->retstatus);

	}while (internal_context.current_status == srm_call_status_INTERNAL_ERROR);

	if (internal_context.current_status == srm_call_status_SUCCESS)
	{
		if ((*output->statuses = (struct srmv2_filestatus*) calloc (1, sizeof (struct srmv2_filestatus))) == NULL)
		{
			srm_errmsg (context, "[SRM][malloc][] error");
			errno = ENOMEM;
			srm_soap_deinit(&soap);
			return (-1);
		}
		(*output->statuses)[0].surl = strdup (input->surl);
		(*output->statuses)[0].status = statuscode2errno(output->retstatus->statusCode);
		if ((*output->statuses)[0].status)
		{
			srm_print_explanation(&((*output->statuses)[0].explanation), output->retstatus,srmfunc);
		}
		srm_soap_deinit(&soap);
		errno = 0;
		return (0);
	}else
	{
		srm_soap_deinit(&soap);
		errno = ECOMM;
		return (-1);
	}
}

/* tries to create all directories in 'dest_file' */
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input)
{
	char file[1024];
	int flags;
	int sav_errno = 0;
	char *p, *endp;
	struct srm2__srmMkdirResponse_ rep;
	struct srm2__srmMkdirRequest req;
	struct srm2__TReturnStatus *repstatp;
	struct soap soap;
	const char srmfunc[] = "Mkdir";

	srm_soap_init(&soap);

	memset (&req, 0, sizeof (struct srm2__srmMkdirRequest));

	strncpy (file, input->dir_name, 1023);

	if ((p = endp = strrchr (file, '/')) == NULL) {
		srm_errmsg (context, "[SRM][srmv2_makedirp][EINVAL] %s: Invalid SURL",  input->dir_name);
		srm_soap_deinit(&soap);
		errno = EINVAL;
		return (-1);
	}

	// 1st cycle, trying to create directories ascendingly, until success
	do {
		*p = 0;
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir (&soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			srm_soap_deinit(&soap);
			return (-1);
		}

		repstatp = rep.srmMkdirResponse->returnStatus;
		sav_errno = statuscode2errno (repstatp->statusCode);

		if (sav_errno != 0 && sav_errno != EEXIST && sav_errno != EACCES && sav_errno != ENOENT)
		{
			if (repstatp->explanation && repstatp->explanation[0])
			{
				srm_errmsg (context, "[SE][%s][%s] %s: %s: %s",
						srmfunc, statuscode2errmsg(repstatp->statusCode),
						context->srm_endpoint, input->dir_name, repstatp->explanation);
			}else
			{
				srm_errmsg (context, "[SE][%s][%s] %s: %s: <none>",
						srmfunc, statuscode2errmsg(repstatp->statusCode),
						context->srm_endpoint, input->dir_name);
			}
			srm_soap_deinit(&soap);
			errno = sav_errno;
			return (-1);
		}
	} while (sav_errno == ENOENT && (p = strrchr (file, '/')) != NULL);

	if (p == NULL) {
		// should never happen, failure must appear in soap call
		srm_errmsg (context, "[SRM][srmv2_makedirp][EINVAL] %s: Invalid SURL", input->dir_name);
		srm_soap_deinit(&soap);
		errno = EINVAL;
		return (-1);
	}

	// 2nd cycle, creating directories descendingly as of the one created by previous cycle
	*p = '/';
	sav_errno = 0;
	while (sav_errno == 0 && p < endp && (p = strchr (p + 1, 0)) != NULL)
	{
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir(&soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			errno = srm_soup_call_err(context,&soap,srmfunc);
			srm_soap_deinit(&soap);
			errno = ECOMM;
			return (-1);
		}

		if (rep.srmMkdirResponse == NULL || (repstatp = rep.srmMkdirResponse->returnStatus) == NULL)
		{
			srm_errmsg (context, "[SE][%s][] %s: <empty response>",
					srmfunc, context->srm_endpoint);
			srm_soap_deinit(&soap);
			errno = ECOMM;
			return (-1);
		}

		sav_errno = statuscode2errno (repstatp->statusCode);

		if (sav_errno != 0) {
			if (repstatp->explanation && repstatp->explanation[0])
				srm_errmsg (context , "[SE][%s][%s] %s: %s",
						srmfunc, statuscode2errmsg(repstatp->statusCode),
						file, repstatp->explanation);
			else
				srm_errmsg (context,  "[SE][%s][%s] %s: <none>",
						srmfunc, statuscode2errmsg(repstatp->statusCode), file);

			srm_soap_deinit(&soap);
			errno = sav_errno;
			return (-1);
		}

		*p = '/';
	}

	srm_soap_deinit(&soap);
	//strncpy (lastcreated_dir, dest_file, 1024);
    errno = 0;
	return (0);
}
