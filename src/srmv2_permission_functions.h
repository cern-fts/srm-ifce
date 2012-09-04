#pragma once
#ifndef _SRMV2_PERMISSION_FUNCTIONS_
#define _SRMV2_PERMISSION_FUNCTIONS_
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

int srmv2_set_permission(struct srm_context *context,
        struct srm_setpermission_input *input);


int srmv2_get_permission(struct srm_context *context,
		struct srm_getpermission_input *input,
        struct srm_getpermission_output *output);


int srmv2_check_permission(struct srm_context *context,
		struct srm_checkpermission_input *input,
        struct srmv2_filestatus **statuses);


#endif
