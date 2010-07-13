#ifndef _SRM_IFCE_H
#define _SRM_IFCE_H

#include "srm_types.h"

void srm_context_init(struct srm_context *context,char *srm_endpoint);

int srm_ls(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_status_of_ls_async(struct srm_context *context,
		struct srm_ls_input *input,struct srm_ls_output *output);

int srm_bring_online(struct srm_context *context,
		struct srm_bringonline_input *input, struct srm_bringonline_output *output);

int srm_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,struct srm_bringonline_output *output);

int srm_status_of_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,struct srm_bringonline_output *output);

int srm_prepare_to_put(struct srm_context *context,
		struct srm_preparetoput_input *input,struct srm_preparetoput_output *output);

int srm_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,struct srm_preparetoput_output *output);

int srm_status_of_put_request_async(struct srm_context *context,
		struct srm_preparetoput_input *input,struct srm_preparetoput_output *output);

int srm_prepare_to_get(struct srm_context *context,
		struct srm_preparetoget_input *input,struct srm_preparetoget_output *output);

int srm_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,struct srm_preparetoget_output *output);

int srm_status_of_get_request_async(struct srm_context *context,
		struct srm_preparetoget_input *input,struct srm_preparetoget_output *output);

int srm_rm(struct srm_context *context,
		struct srm_rm_input *input,struct srm_rm_output *output);

int srm_rmdir(struct srm_context *context,
		struct srm_rmdir_input *input,struct srm_rmdir_output *output);

int srm_mkdir(struct srm_context *context,
		struct srm_mkdir_input *input);

int srm_put_done(struct srm_context *context,
		struct srm_putdone_input *input, struct srmv2_filestatus **statuses);

int srm_release_files(struct srm_context *context,
		struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses);

int srm_abort_files(struct srm_context *context,
		struct srm_abort_files_input *input,struct srmv2_filestatus **statuses);

int srm_abort_request (struct srm_context *context,
		char *reqtoken);

char* srm_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input);

int srm_getspacetokens (struct srm_context *context,
		struct srm_getspacetokens_input *input,struct srm_getspacetokens_output *output);

int srm_getspacemd (struct srm_context *context,
		struct srm_getspacemd_input *input,struct srm_spacemd **spaces);

int srm_ping(struct srm_context *context,
		struct srm_ping_output *output);

int srm_getpermission (struct srm_context *context,
		struct srm_getpermission_input *input,struct srm_getpermission_output *output);

int srm_setpermission (struct srm_context *context,
		struct srm_setpermission_input *input);

char* srmv2_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input);

int srm_extend_file_lifetime (struct srm_context *context,
		struct srm_extendfilelifetime_input *input,struct srm_extendfilelifetime_output *output);


// gsoap timeouts
void srm_set_timeout_connect (int);
int srm_get_timeout_connect ();
void srm_set_timeout_sendreceive (int);
int srm_get_timeout_sendreceive ();

#endif
