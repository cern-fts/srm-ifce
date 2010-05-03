#include "srm_types.h"



int srm_copy(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output **output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_copy(context,input,output);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}
