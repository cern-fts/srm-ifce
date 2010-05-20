#include "srm_dependencies.h"
#include "srm_soap.h"

callers_t call_function = {
		soap_call_srm2__srmPutDone,
		soap_call_srm2__srmReleaseFiles,
		soap_call_srm2__srmStatusOfGetRequest,
		soap_call_srm2__srmStatusOfBringOnlineRequest,
		soap_call_srm2__srmStatusOfPutRequest,
	    soap_call_srm2__srmPrepareToGet,
	    soap_call_srm2__srmBringOnline,
	    soap_call_srm2__srmPrepareToPut,
		soap_call_srm2__srmGetSpaceMetaData,
		soap_call_srm2__srmGetSpaceTokens,
		soap_call_srm2__srmLs,
		soap_call_srm2__srmStatusOfLsRequest,
		soap_call_srm2__srmAbortRequest,
		soap_call_srm2__srmRm,
		soap_call_srm2__srmRmdir,
		soap_call_srm2__srmMkdir,
		sleep
};
