#ifndef SRM_UTIL_H_
#define SRM_UTIL_H_

void srm_soap_init(struct soap *soap);
void srm_soap_deinit(struct soap *soap);

void srm_errmsg (struct srm_context *context, const char *format, ...);
int srm_soup_call_err(struct srm_context *context,struct soap *soap,const char *srmfunc);
int srm_call_err(struct srm_context *context,struct srm_internal_context *internal_context,const char *srmfunc);
int srm_print_error_status(struct srm_context *context,struct srm2__TReturnStatus *status,char *srmfunc);

int statuscode2errno (int statuscode);
const char * statuscode2errmsg (int statuscode);

srm_call_status back_off_logic(struct srm_context *context,const char *srmfunc,
		struct srm_internal_context *internal_context);
int wait_for_new_attempt(struct srm_internal_context *internal_context);

#endif /* SRM_UTIL_H_ */
