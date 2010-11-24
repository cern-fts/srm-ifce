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

#include "srm_dependencies.h"
#include "srm_soap.h"


callers_t call_function = {
		soap_call_srm2__srmExtendFileLifeTime,
		soap_call_srm2__srmCheckPermission,
		soap_call_srm2__srmReleaseSpace,
		soap_call_srm2__srmReserveSpace,
		soap_call_srm2__srmStatusOfReserveSpaceRequest,
		soap_call_srm2__srmSetPermission,
		soap_call_srm2__srmGetPermission,
		soap_call_srm2__srmPing,
		soap_call_srm2__srmAbortFiles,
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
