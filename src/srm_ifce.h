#ifndef _SRM_IFCE_H
#define _SRM_IFCE_H

#include "srm_types.h"

int srm_ls(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output **output);

int srm_rm(struct srm_context *context,
		struct srm_rm_input *input,struct srm_rm_output **output);

int srm_rmdir(struct srm_context *context,
		struct srm_rmdir_input *input,struct srm_rmdir_output **output);

int srm_mkdir(struct srm_context *context,
		struct srm_mkdir_input *input);

char* srmv2_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input);

int srmv2_getspacetokens (struct srm_context *context,
		struct srm_getspacetokens_input *input,struct srm_getspacetokens_output *output);

int srmv2_getspacemd (struct srm_context *context,
		struct srmv2_getspacemd_input *input, srm_spacemd **spaces);

int srmv2_prepeare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input, struct srmv2_pinfilestatus **filestatuses);

int srmv2_prepeare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input, struct srmv2_pinfilestatus **filestatuses);

int srmv2_put_done(struct srm_context *context,
		struct srm_putdone_input *input, struct srmv2_filestatus **statuses);

int srmv2_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses);

#endif
