#include "srm_types.h"
#include "srm_ifce.h"



int srm_ls(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_ls(context,input,output);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}

int srm_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_rm(context,input,output);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}
int srm_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_rmdir(context,input,output);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}
int srm_mkdir(struct srm_context *context,struct srm_mkdir_input *input)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_mkdir(context,input);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}
