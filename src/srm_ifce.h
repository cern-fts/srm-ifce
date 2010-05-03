#ifndef _SRM_IFCE_H
#define _SRM_IFCE_H

#include "srm_types.h"

int srm_ls(struct srm_context *context,struct srm_ls_input *input,struct srm_ls_output **output);
int srm_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output **output);
int srm_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output **output);

#endif
