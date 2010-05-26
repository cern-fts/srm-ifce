#ifndef _SRM_IFCE_H
#define _SRM_IFCE_H

#include "srm_types.h"

int srm_ls(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_status_of_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_rm(struct srm_context *context,
		struct srm_rm_input *input,struct srm_rm_output *output);

int srm_rmdir(struct srm_context *context,
		struct srm_rmdir_input *input,struct srm_rmdir_output *output);

int srm_mkdir(struct srm_context *context,
		struct srm_mkdir_input *input);

int srm_bring_online(struct srm_context *context,
		struct srm_bringonline_input *input, struct srm_bringonline_output *output);

int srm_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,struct srm_bringonline_output *output);

int srm_status_of_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,struct srm_bringonline_output *output);

char* srmv2_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input);

int srmv2_getspacetokens (struct srm_context *context,
		struct srm_getspacetokens_input *input,struct srm_getspacetokens_output *output);

int srmv2_getspacemd (struct srm_context *context,
		struct srmv2_getspacemd_input *input, srm_spacemd **spaces);

int srm_prepeare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input,struct srm_preparetoput_output *output);

int srm_prepeare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input,struct srm_preparetoget_output *output);


int srm_put_done(struct srm_context *context,
		struct srm_putdone_input *input, struct srmv2_filestatus **statuses);

int srm_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses);

int srm_abort_files(struct srm_context *context,
		struct srm_abort_files_input *input,struct srmv2_filestatus **statuses);

int srm_abort_request (struct srm_context *context,
		char *reqtoken);

#endif
