/*
 * Copyright (c) CERN 2013-2015
 *
 * Copyright (c) Members of the EMI Collaboration. 2010-2013
 *  See  http://www.eu-emi.eu/partners for details on the copyright
 *  holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SRM_IFCE_H
#define _SRM_IFCE_H

#include <glib.h>
#include "gfal_srm_ifce_types.h"

/**
  @brief create a srm_context

  srm context are used by all the srm operations
  need to be free with @ref srm_context_free after usage
*/
srm_context_t srm_context_new(const char * srm_endpoint, char *errbuf,int errbufsz,int verbose);

srm_context_t srm_context_new2(const char * srm_endpoint, char *errbuf,int errbufsz,int verbose, int keep_alive);

/**
  free resources used by a srm context
*/
void srm_context_free(srm_context_t context);

/**
  brief : old compatibility : deprecated
*/
void srm_context_init(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose);

void srm_context_init2(struct srm_context *context,char *srm_endpoint,char *errbuf,int errbufsz,int verbose,
                       int keep_alive);

/**
 * set user credentials
 */
void srm_set_credentials(struct srm_context *context, const char *ucert, const char *ukey);

/**
 * Set total desired request time. For instance, for bringonline
 */
void srm_set_desired_request_time(struct srm_context *context, int timeout);

/**
 set a custom header
 Set value to NULL to remove the header
 */
void srm_set_http_header(struct srm_context *context, const char *key, const char *value);

/**
 set the custom User-Agent string
 It will be appended to what gsoap sets
 */
void srm_set_user_agent(struct srm_context *context, const char *user_agent, ...);

/* srm_ls functions */
void srm_ls_output_destroy(struct srm_ls_output *output);

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

int srm_mv(struct srm_context *context, struct srm_mv_input *input);

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

int srm_xping(struct srm_context *context,
        struct srm_xping_output *output);

int srm_xping_output_free(struct srm_xping_output output);

int srm_getpermission (struct srm_context *context,
		struct srm_getpermission_input *input,struct srm_getpermission_output *output);

int srm_setpermission (struct srm_context *context,
		struct srm_setpermission_input *input);

int srm_check_permission(struct srm_context *context,
		struct srm_checkpermission_input *input,struct srmv2_filestatus **statuses);

char* srmv2_getbestspacetoken (struct srm_context *context,
		struct srm_getbestspacetokens_input *input);

int srm_extend_file_lifetime (struct srm_context *context,
		struct srm_extendfilelifetime_input *input,struct srm_extendfilelifetime_output *output);

int srm_purgefromspace(struct srm_context *context,
		struct srm_purgefromspace_input *input,struct srm_purgefromspace_output *output);

// memory management
void srm_srmv2_pinfilestatus_delete(struct srmv2_pinfilestatus*  srmv2_pinstatuses, int n);
void srm_srmv2_mdfilestatus_delete(struct srmv2_mdfilestatus* mdfilestatus, int n);
void srm_srmv2_filestatus_delete(struct srmv2_filestatus*  srmv2_statuses, int n);
void srm_srm2__TReturnStatus_delete(struct srm2__TReturnStatus* status);

// gsoap timeouts
void srm_set_timeout_connect (int);
int srm_get_timeout_connect ();
void srm_set_timeout_sendreceive (int);
int srm_get_timeout_sendreceive ();

#endif
