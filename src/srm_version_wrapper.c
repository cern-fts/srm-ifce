#include <stdio.h>
#include "srm_types.h"
#include "srm_ifce.h"



int srm_ls(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_ls_sync(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_ls_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_status_of_ls_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_bring_online_sync(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_bring_online_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_status_of_bring_online_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_rm(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_rmdir(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_mkdir(context,input);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_copy(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_release_files(context,input,statuses);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_put_done(context,input,statuses);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_prepeare_to_get_sync(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_prepeare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_prepeare_to_get_async(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_status_of_get_request(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_status_of_get_request_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_prepeare_to_put_sync(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_prepeare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_prepare_to_put_async(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_status_of_put_request(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_status_of_put_request_async(context,input,output);
		case VERSION_1:
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
		case VERSION_2_2:
			return srmv2_abort_files(context,input,statuses);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_abort_request(struct srm_context *context,
		char *token)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_abort_request(context,token);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_ping(struct srm_context *context,
		struct srm_ping_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_ping(context,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
char* srm_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_getbestspacetoken(context,input);
		case VERSION_1:
			// TODO
			return (NULL);
		default:
			return (NULL);
	}
}
int srm_getspacetokens (struct srm_context *context,
		struct srm_getspacetokens_input *input,struct srm_getspacetokens_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_getspacetokens(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}

int srm_getspacemd (struct srm_context *context,
		struct srm_getspacemd_input *input,struct srm_spacemd **spaces)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_getspacemd(context,input,spaces);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_getpermission (struct srm_context *context,
		struct srm_getpermission_input *input,struct srm_getpermission_output *output)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_get_permission(context,input,output);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}
int srm_setpermission (struct srm_context *context,
		struct srm_setpermission_input *input)
{
	switch (context->version)
	{
		case VERSION_2_2:
			return srmv2_set_permission(context,input);
		case VERSION_1:
			// TODO
			return (-1);
		default:
			return (-1);
	}
}


