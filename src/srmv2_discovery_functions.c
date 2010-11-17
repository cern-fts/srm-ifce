#include <errno.h>
#include "srm_soap.h"
#include "srm_util.h"
#include "srm_dependencies.h"


int srmv2_ping(struct srm_context *context,struct srm_ping_output *output)
{
	const char srmfunc[] = "AbortRequest";
	struct srm2__srmPingRequest req;
	struct srm2__srmPingResponse_ rep;
	struct soap soap;
	int result;

	srm_soap_init(&soap);

	memset (&req, 0, sizeof(req));

	result = call_function.call_srm2__srmPing (&soap, context->srm_endpoint, srmfunc, &req, &rep);

	if (result != 0)
	{
		// Soap call failure
		errno = srm_soap_call_err(context,&soap,srmfunc);
	}else
	{
		if (rep.srmPingResponse == NULL ||
				copy_string(&output->versioninfo,rep.srmPingResponse->versionInfo))
		{
			errno = EINVAL;
			srm_soap_deinit(&soap);
			return (-1);
		}
	}

	srm_soap_deinit(&soap);

	return result;
}
