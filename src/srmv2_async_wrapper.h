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

#pragma once
#ifndef _SRMV2_ASYNC_WRAPPER_H_
#define _SRMV2_ASYNC_WRAPPER_H_

#include "srm_ifce_internal.h"



int srmv2_ls_async(struct srm_context *context,
		struct srm_ls_input *input,
        struct srm_ls_output *output);

int srmv2_status_of_ls_async(struct srm_context *context,
		struct srm_ls_input *input,
        struct srm_ls_output *output);

int srmv2_prepare_to_get_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
        struct srm_preparetoget_output *output);

int srmv2_status_of_get_request_async(struct srm_context *context,
		struct srm_preparetoget_input *input,
        struct srm_preparetoget_output *output);

int srmv2_prepare_to_put_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
        struct srm_preparetoput_output *output);

int srmv2_status_of_put_request_async(struct srm_context *context,
		struct srm_preparetoput_input *input,
        struct srm_preparetoput_output *output);

int srmv2_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,
        struct srm_bringonline_output *output);


int srmv2_status_of_bring_online_async(struct srm_context *context,
		struct srm_bringonline_input *input,
        struct srm_bringonline_output *output);


#endif

