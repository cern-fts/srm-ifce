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
	const char srmfunc[] = "Ping";
	struct srm2__srmPingRequest req;
	struct srm2__srmPingResponse_ rep;
	int result;

	srm_context_soap_init(context);

	memset (&req, 0, sizeof(req));

	result = call_function.call_srm2__srmPing (context->soap, context->srm_endpoint, srmfunc, &req, &rep);

	if (result != 0)
	{
		// Soap call failure
		errno = srm_soap_call_err(context, srmfunc);
		result = -1;
	}else
	{
		if (rep.srmPingResponse == NULL ||
				copy_string(&output->versioninfo,rep.srmPingResponse->versionInfo))
		{
			errno = EINVAL;
			return -1;
		}
	}

	return result;
}


int srmv2_xping(struct srm_context *context, struct srm_xping_output *output)
{
    const char srmfunc[] = "XPing";
    struct srm2__srmPingRequest req;
    struct srm2__srmPingResponse_ rep;
    int result, i = 0;

    srm_context_soap_init(context);

    memset (&req, 0, sizeof(req));

    result = call_function.call_srm2__srmPing (context->soap, context->srm_endpoint, srmfunc, &req, &rep);

    if (result != 0)
    {
        // Soap call failure
        errno = srm_soap_call_err(context, srmfunc);
        result = -1;
    }else
    {
        if (rep.srmPingResponse == NULL)
        {
            errno = EINVAL;
            return -1;
        }
        if (copy_string(&output->versioninfo, rep.srmPingResponse->versionInfo) < 0) {
            errno = EINVAL;
            return -1;
        }

        if (rep.srmPingResponse->otherInfo) {
			output->n_extra = rep.srmPingResponse->otherInfo->__sizeextraInfoArray;
			output->extra = calloc(output->n_extra, sizeof(struct srm_key_value));
			if (!output->extra) {
				free(output->versioninfo);
				errno = EINVAL;
				return -1;
			}

			for (i = 0; i < output->n_extra; ++i) {
				if (copy_string(&output->extra[i].key, rep.srmPingResponse->otherInfo->extraInfoArray[i]->key) < 0)
					goto xping_abort;
				if (copy_string(&output->extra[i].value, rep.srmPingResponse->otherInfo->extraInfoArray[i]->value) < 0)
					goto xping_abort;
			}
        }
        else {
        	output->n_extra = 0;
        	output->extra = NULL;
        }
        result = 0;
    }
    return result;
xping_abort:
    while (i > 0) {
        free(output->extra[i].key);
        free(output->extra[i].value);
        --i;
    }
    free(output->extra);
    free(output->versioninfo);
    return -1;
}
