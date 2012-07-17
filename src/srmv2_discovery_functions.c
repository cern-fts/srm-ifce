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
#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int srmv2_ping(struct srm_context *context,struct srm_ping_output *output)
{
	const char srmfunc[] = "AbortRequest";
	struct srm2__srmPingRequest req;
	struct srm2__srmPingResponse_ rep;
    struct soap* soap = srm_soap_init_context_new(context);
	int result;

	

	memset (&req, 0, sizeof(req));

	result = call_function.call_srm2__srmPing (soap, context->srm_endpoint, srmfunc, &req, &rep);

	if (result != 0)
	{
		// Soap call failure
		errno = srm_soap_call_err(context,soap,srmfunc);
	}else
	{
		if (rep.srmPingResponse == NULL ||
				copy_string(&output->versioninfo,rep.srmPingResponse->versionInfo))
		{
			errno = EINVAL;
			srm_soap_free(soap);
			return (-1);
		}
	}

	srm_soap_free(soap);

	return result;
}
