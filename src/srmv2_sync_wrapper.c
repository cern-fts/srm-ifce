#include "srm_types.h"
#include "srm_ifce.h"

int srmv2_ls_sync(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// Call srm ls
	result = srmv2_ls_async_internal(context,input,output,&internal_context);


	// if ls was queued start polling statusOfLsRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_ls_request_async_internal(context,input,output,&internal_context);
		if ((internal_context.current_status != srm_call_status_SUCCESS)
				&&(internal_context.current_status != srm_call_status_QUEUED)
				&&(result!=0))
		{
			srmv2_abort_request(context,&internal_context);
			return -1;
		}

		// ls status of request
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return result;
}

int srmv2_prepeare_to_put_sync(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	result = srmv2_prepare_to_put_async_internal(context,input,output,&internal_context);


	// if put was queued start polling statusOfPutRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_put_request_async_internal(context,input,output,&internal_context);
		if ((internal_context.current_status != srm_call_status_SUCCESS)
				&&(internal_context.current_status != srm_call_status_QUEUED)
				&&(result!=0))
		{
			srmv2_abort_request(context,output->token);
			return -1;
		}
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return result;
}

int srmv2_prepeare_to_get_sync(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	result = srmv2_prepare_to_get_async_internal(context,input,output,&internal_context);


	// if put was queued start polling statusOfPutRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_get_request_async_internal(context,input,output,&internal_context);
		if ((internal_context.current_status != srm_call_status_SUCCESS)
				&&(internal_context.current_status != srm_call_status_QUEUED)
				&&(result!=0))
		{
			srmv2_abort_request(context,output->token);
			return -1;
		}
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return result;
}
int srmv2_bring_online_sync(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	result = srmv2_bring_online_async_internal(context,input,output,&internal_context);


	// if put was queued start polling statusOfPutRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_bring_online_async_internal(context,input,output,&internal_context);
		if ((internal_context.current_status != srm_call_status_SUCCESS)
				&&(internal_context.current_status != srm_call_status_QUEUED)
				&&(result!=0))
		{
			srmv2_abort_request(context,output->token);
			return -1;
		}
	}

	if (internal_context.current_status != srm_call_status_SUCCESS)
	{
		return -1;
	}
	return result;
}
