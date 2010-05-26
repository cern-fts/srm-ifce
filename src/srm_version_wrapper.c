#include "srm_types.h"
#include "srm_ifce.h"



int srm_ls(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_ls_sync(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_ls_async(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_ls_async(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_status_of_ls_async(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_status_of_ls_async(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_bring_online(struct srm_context *context,struct srm_bringonline_input *input,struct srm_bringonline_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_bring_online_sync(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_bring_online_async(struct srm_context *context,struct srm_bringonline_input *input,struct srm_bringonline_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_bring_online_async(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_status_of_bring_online_async(struct srm_context *context,struct srm_bringonline_input *input,struct srm_bringonline_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_status_of_bring_online_async(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
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
			return (-1);
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
			return (-1);
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
			return (-1);
		default:
			return (-1);
	}
}
int srm_copy(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output **output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_copy(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_release_files(context,input,statuses);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_put_done(struct srm_context *context,
		struct srm_putdone_input *input, struct srmv2_filestatus **statuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_put_done(context,input,statuses);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_prepeare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_prepeare_to_get_sync(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_prepeare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_prepeare_to_put_sync(context,input,output);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_abort_files(struct srm_context *context,
		struct srm_abort_files_input *input,struct srmv2_filestatus **statuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_abort_files(context,input,statuses);
		case TYPE_SRM:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}



