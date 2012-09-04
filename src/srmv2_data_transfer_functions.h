#pragma once
#ifndef _SRMV2_DATA_TRANSFER_FUNCTIONS_
#define _SRMV2_DATA_TRANSFER_FUNCTIONS_
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
 * Authors: Todor Manev  IT-GT CERN
 */


#include "srm_ifce_internal.h"


int srmv2_status_of_put_request_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
        struct srm_preparetoput_output *output,
		struct srm_internal_context *internal_context);

int srmv2_prepare_to_put_async_internal(struct srm_context *context,
		struct srm_preparetoput_input *input,
		struct srm_preparetoput_output *output,
		struct srm_internal_context *internal_context);

int srmv2_prepare_to_get_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_get_request_async_internal(struct srm_context *context,
		struct srm_preparetoget_input *input,
		struct srm_preparetoget_output *output,
		struct srm_internal_context *internal_context);

int srmv2_status_of_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);

int srmv2_bring_online_async_internal (struct srm_context *context,
		struct srm_bringonline_input *input,
		struct srm_bringonline_output *output,
		struct srm_internal_context *internal_context);

int srmv2_release_files(struct srm_context *context,
        struct srm_releasefiles_input *input, struct srmv2_filestatus **statuses);

int srmv2_put_done(struct srm_context *context,
        struct srm_putdone_input *input, struct srmv2_filestatus **statuses);

int srmv2_abort_files(struct srm_context *context,
        struct srm_abort_files_input *input,struct srmv2_filestatus **statuses);


#endif
