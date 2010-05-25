#include "srm_types.h"
#include "srm_ifce.h"

int srmv2_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_ls_async_internal(context,input,output,&internal_context);
}

int srmv2_status_of_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output)
{
	struct srm_internal_context internal_context;

	back_off_logic_init(context,&internal_context);

	return srmv2_ls_async_internal(context,input,output,&internal_context);
}
