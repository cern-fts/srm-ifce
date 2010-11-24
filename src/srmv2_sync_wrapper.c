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
#include <stdio.h>
#include "gfal_srm_ifce_types.h"
#include "gfal_srm_ifce.h"

int srmv2_ls_sync(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// Call srm ls
	output->token = NULL;
	result = srmv2_ls_async_internal(context,input,output,&internal_context);

	internal_context.attempt = 1;

	// if ls was queued start polling statusOfLsRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_ls_request_async_internal(context,input,output,&internal_context);
		if (internal_context.current_status == srm_call_status_TIMEOUT)
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

int srmv2_prepare_to_put_sync(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	output->token = NULL;
	result = srmv2_prepare_to_put_async_internal(context,input,output,&internal_context);

	internal_context.attempt = 1;

	// if put was queued start polling statusOfPutRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_put_request_async_internal(context,input,output,&internal_context);
		if (internal_context.current_status == srm_call_status_TIMEOUT)
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

int srmv2_prepare_to_get_sync(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;
	int i,result;

	// Setup the timeout
	back_off_logic_init(context,&internal_context);

	// request
	output->token = NULL;
	result = srmv2_prepare_to_get_async_internal(context,input,output,&internal_context);

	internal_context.attempt = 1;

	// if put was queued start polling statusOfPutRequest
	while ((internal_context.current_status == srm_call_status_QUEUED)&&(result == 0))
	{
		result = srmv2_status_of_get_request_async_internal(context,input,output,&internal_context);
		if (internal_context.current_status == srm_call_status_TIMEOUT)
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
	output->token = NULL;
	result = srmv2_bring_online_async_internal(context,input,output,&internal_context);

	internal_context.attempt = 1;

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
