#ifndef SRM_UTIL_H_
#define SRM_UTIL_H_

void srm_soap_init(struct soap *soap);
void srm_soap_deinit(struct soap *soap);

void srm_errmsg (struct srm_context *context, const char *format, ...);
int srm_soup_call_err(struct srm_context *context,struct soap *soap,const char *srmfunc);
int srm_call_err(struct srm_context *context,struct srm2__TReturnStatus  *retstatus,const char *srmfunc);
int srm_print_error_status(struct srm_context *context,struct srm2__TReturnStatus *status,const char *srmfunc);

int statuscode2errno (int statuscode);
const char * statuscode2errmsg (int statuscode);

void back_off_logic_init(struct srm_context *context,struct srm_internal_context *internal_context);
srm_call_status back_off_logic(struct srm_context *context,const char *srmfunc,
		struct srm_internal_context *internal_context,struct srm2__TReturnStatus  *retstatus);
int wait_for_new_attempt(struct srm_internal_context *internal_context);

void srm_spacemd_free (int nbtokens, struct srm_spacemd *smd);

int srm_set_protocol_in_transferParameters(
	struct srm_context *context,
    struct soap* soap,
    struct srm2__TTransferParameters* transferParameters,
    char** protocols);

char* srm_strip_string(const char* str, const char chr);
int srm_count_elements_of_string_array(char** a);

int copy_string(char **dest,char *src);

int copy_permissionfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLPermissionReturn *repfs,
						const char *srmfunc,
						int amode);

int copy_filestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLReturnStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_extendlifetime(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTSURLLifetimeReturnStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_get(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTGetRequestFileStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_put(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_bringonline(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs,
						const char *srmfunc);

int copy_mdfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_mdfilestatus **statuses,
						struct srm2__ArrayOfTMetaDataPathDetail *repfs);

int copy_returnstatus(struct srm2__TReturnStatus **destination,
		struct srm2__TReturnStatus *returnStatus);

int copy_filepermissions(struct srm2__TReturnStatus *reqstatp,
		struct srm_filepermission **permissions,
		struct srm2__ArrayOfTPermissionReturn *repperm);

void set_estimated_wait_time(struct srm_internal_context *internal_context, int *time);


#endif /* SRM_UTIL_H_ */
