#ifndef SRM_DEPENDANCIES_H_
#define SRM_DEPENDANCIES_H_


#define WITH_NOGLOBAL
#include "stdsoap2.h"
#undef SOAP_FMAC3
#define SOAP_FMAC3 static
#undef SOAP_FMAC5
#define SOAP_FMAC5 static
#include "srmv2H.h"
#ifdef GFAL_SECURE
#include "cgsi_plugin.h"
#endif // #ifdef GFAL_SECURE


/* Function types of the appropriate SOAP calls */
typedef int (*soap_call_srm2__srmLs_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmLsRequest *,
    struct srm2__srmLsResponse_ *);

typedef int (*soap_call_srm2__srmStatusOfLsRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmStatusOfLsRequest *,
    struct srm2__srmStatusOfLsResponse_ *);

typedef int (*soap_call_srm2__srmAbortRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmAbortRequestRequest *,
    struct srm2__srmAbortRequestResponse_ *);

typedef int (*soap_call_srm2__srmRm_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmRmRequest *,
    struct srm2__srmRmResponse_ *);

typedef int (*soap_call_srm2__srmRmdir_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmRmdirRequest *,
    struct srm2__srmRmdirResponse_ *);

typedef int (*soap_call_srm2__srmMkdir_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmMkdirRequest *,
    struct srm2__srmMkdirResponse_ *);

typedef unsigned int (*sleep_fv)(
    unsigned int);

/* The collection of SOAP service calls. The pointers may be replaced with test
 * functions (dependency injection) */
typedef struct misc_callers {
	soap_call_srm2__srmLs_fv 					call_srm2__srmLs;
	soap_call_srm2__srmStatusOfLsRequest_fv 	call_srm2__srmStatusOfLsRequest;
	soap_call_srm2__srmAbortRequest_fv			call_srm2__srmAbortRequest;
	soap_call_srm2__srmRm_fv					call_srm2__srmRm;
	soap_call_srm2__srmRmdir_fv					call_srm2__srmRmdir;
	soap_call_srm2__srmMkdir_fv					call_srm2__srmMkdir;
	sleep_fv									call_sleep;
} callers_t;

extern callers_t call_function;

#endif /* SRM_DEPENDANCIES_H_ */
