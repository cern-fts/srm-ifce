#include "srm_types.h"



int srm_copy(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output **output)
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

int srm_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_release_files(context,input,statuses);
		case TYPE_SRM:
			// TODO
			return 0;
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
			return 0;
		default:
			return (-1);
	}
}

int srm_prepeare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input, struct srmv2_pinfilestatus **filestatuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_prepeare_to_get(context,input,filestatuses);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}

int srm_prepeare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srmv2_pinfilestatus **filestatuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_prepeare_to_put(context,input,filestatuses);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}

int srm_abort_files(struct srm_context *context,
		struct srm_abortfiles_input *input,struct srmv2_filestatus **statuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_abort_files(context,input,statuses);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}

int srm_bring_online(struct srm_context *context,
		struct srm_bringonline_input *input, struct srmv2_pinfilestatus **filestatuses)
{
	switch (context->version)
	{
		case TYPE_SRMv2:
			return srmv2_bring_online(context,input,filestatuses);
		case TYPE_SRM:
			// TODO
			return 0;
		default:
			return (-1);
	}
}
