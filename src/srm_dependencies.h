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
typedef int (*soap_call_srm2__srmGetSpaceMetaData_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmGetSpaceMetaDataRequest *,
    struct srm2__srmGetSpaceMetaDataResponse_ *);

typedef int (*soap_call_srm2__srmGetSpaceTokens_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmGetSpaceTokensRequest *,
    struct srm2__srmGetSpaceTokensResponse_ *);

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

typedef int (*soap_call_srm2__srmPrepareToGet_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmPrepareToGetRequest *,
    struct srm2__srmPrepareToGetResponse_ *);

typedef int (*soap_call_srm2__srmBringOnline_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmBringOnlineRequest *,
    struct srm2__srmBringOnlineResponse_ *);

typedef int (*soap_call_srm2__srmPrepareToPut_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmPrepareToPutRequest *,
    struct srm2__srmPrepareToPutResponse_ *);

typedef int (*soap_call_srm2__srmStatusOfPutRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmStatusOfPutRequestRequest *,
    struct srm2__srmStatusOfPutRequestResponse_ *);

typedef int (*soap_call_srm2__srmStatusOfBringOnlineRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmStatusOfBringOnlineRequestRequest *,
    struct srm2__srmStatusOfBringOnlineRequestResponse_ *);

typedef int (*soap_call_srm2__srmStatusOfGetRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmStatusOfGetRequestRequest *,
    struct srm2__srmStatusOfGetRequestResponse_ *);

typedef int (*soap_call_srm2__srmPutDone_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmPutDoneRequest*,
    struct srm2__srmPutDoneResponse_ *);

typedef int (*soap_call_srm2__srmReleaseFiles_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmReleaseFilesRequest*,
    struct srm2__srmReleaseFilesResponse_ *);

typedef int (*soap_call_srm2__srmAbortFiles_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmAbortFilesRequest *,
    struct srm2__srmAbortFilesResponse_ *);

typedef int (*soap_call_srm2__srmPing_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmPingRequest *,
    struct srm2__srmPingResponse_ *);

typedef int (*soap_call_srm2__srmSetPermission_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmSetPermissionRequest *,
    struct srm2__srmSetPermissionResponse_ *);

typedef int (*soap_call_srm2__srmGetPermission_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmGetPermissionRequest *,
    struct srm2__srmGetPermissionResponse_ *);

typedef int (*soap_call_srm2__srmReserveSpace_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmReserveSpaceRequest *,
    struct srm2__srmReserveSpaceResponse_ *);

typedef int (*soap_call_srm2__srmStatusOfReserveSpaceRequest_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmStatusOfReserveSpaceRequest *,
    struct srm2__srmStatusOfReserveSpaceRequestResponse_ *);

typedef int (*soap_call_srm2__srmReleaseSpace_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmReleaseSpaceRequest *,
    struct srm2__srmReleaseSpaceResponse_ *);

typedef int (*soap_call_srm2__srmCheckPermission_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmCheckPermissionRequest *,
    struct srm2__srmCheckPermissionResponse_ *);

typedef int (*soap_call_srm2__srmExtendFileLifeTime_fv)(
    struct soap *, const char *, const char *,
    struct srm2__srmExtendFileLifeTimeRequest *,
    struct srm2__srmExtendFileLifeTimeResponse_ *);


typedef unsigned int (*sleep_fv)(
    unsigned int);

/* The collection of SOAP service calls. The pointers may be replaced with test
 * functions (dependency injection) */
typedef struct misc_callers {
	soap_call_srm2__srmExtendFileLifeTime_fv	call_srm2__srmExtendFileLifeTime;
	soap_call_srm2__srmCheckPermission_fv		call_srm2__srmCheckPermission;
	soap_call_srm2__srmReleaseSpace_fv			call_srm2__srmReleaseSpace;
	soap_call_srm2__srmReserveSpace_fv			call_srm2__srmReserveSpace;
	soap_call_srm2__srmStatusOfReserveSpaceRequest_fv	call_srm2__srmStatusOfReserveSpaceRequest;
	soap_call_srm2__srmSetPermission_fv			call_srm2__srmSetPermission;
	soap_call_srm2__srmGetPermission_fv			call_srm2__srmGetPermission;
	soap_call_srm2__srmPing_fv					call_srm2__srmPing;
	soap_call_srm2__srmAbortFiles_fv			call_srm2__srmAbortFiles;
	soap_call_srm2__srmPutDone_fv 				call_srm2__srmPutDone;
	soap_call_srm2__srmReleaseFiles_fv			call_srm2__srmReleaseFiles;
	soap_call_srm2__srmStatusOfGetRequest_fv	call_srm2__srmStatusOfGetRequest;
	soap_call_srm2__srmStatusOfBringOnlineRequest_fv	call_srm2__srmStatusOfBringOnlineRequest;
	soap_call_srm2__srmStatusOfPutRequest_fv	call_srm2__srmStatusOfPutRequest;
	soap_call_srm2__srmPrepareToGet_fv			call_srm2__srmPrepareToGet;
	soap_call_srm2__srmBringOnline_fv			call_srm2__srmBringOnline;
	soap_call_srm2__srmPrepareToPut_fv			call_srm2__srmPrepareToPut;
	soap_call_srm2__srmGetSpaceMetaData_fv		call_srm2__srmGetSpaceMetaData;
	soap_call_srm2__srmGetSpaceTokens_fv		call_srm2__srmGetSpaceTokens;
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
