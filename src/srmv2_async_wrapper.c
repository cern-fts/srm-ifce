#include "srm_types.h"
#include "srm_ifce.h"

int srmv2_ls_async(struct srm_context *context,
		struct srm_ls_input *input,
		struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_ls_async_internal(context,input,output,&internal_context);
}

int srmv2_status_of_ls_async(struct srm_context *context,
		struct srm_ls_input *input,
		struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_status_of_ls_request_async_internal(context,input,output,&internal_context);
}
int srmv2_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_prepare_to_get_async_internal(context,input,output,&internal_context);
}
int srmv2_status_of_get_request_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_status_of_get_request_async_internal(context,input,output,&internal_context);
}
int srmv2_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_prepare_to_put_async_internal(context,input,output,&internal_context);
}
int srmv2_status_of_put_request_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_status_of_put_request_async_internal(context,input,output,&internal_context);
}
int srmv2_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_bring_online_async_internal(context,input,output,&internal_context);
}
int srmv2_status_of_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_status_of_bring_online_async_internal(context,input,output,&internal_context);
}



