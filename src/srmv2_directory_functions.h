#pragma once
#ifndef _SRMV2_DIRECTORY_FUNCTIONS_H_
#define _SRMV2_DIRECTORY_FUNCTIONS_H_
/*
 * Copyright (c) Members of the EGEE Collaboration. 2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Adrien Devresse, CERN
 */

#include "srm_ifce_internal.h"

// Utility functions
int srmv2_check_srm_root(const char* surl);

int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output);
int srmv2_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output);
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input);


int srmv2_ls_async_internal(struct srm_context *context,
        struct srm_ls_input *input,struct srm_ls_output *output,struct srm_internal_context *internal_context);

int srmv2_status_of_ls_request_async_internal(struct srm_context *context,
		struct srm_ls_input *input,
		struct srm_ls_output *output,
        struct srm_internal_context *internal_context);


int srmv2_rm(struct srm_context *context,struct srm_rm_input *input,struct srm_rm_output *output);

//srmv2_rmdir (const char *surl, const char *srm_endpoint, int recursive,
//struct srmv2_filestatus **statuses, char *errbuf, int errbufsz, int timeout)

int srmv2_rmdir(struct srm_context *context,struct srm_rmdir_input *input,struct srm_rmdir_output *output);

/* tries to create all directories in 'dest_file' */
int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input);

int srmv2_extend_file_lifetime(struct srm_context *context,
		struct srm_extendfilelifetime_input *input,
        struct srm_extendfilelifetime_output *output);

int srmv2_check_srm_root(const char* surl);

int srmv2_mv(struct srm_context *context, struct srm_mv_input *input);

#endif
