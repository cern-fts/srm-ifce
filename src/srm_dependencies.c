#include "srm_dependencies.h"
#include "srm_soap.h"

callers_t call_function = {
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
