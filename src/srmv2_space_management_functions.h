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
#ifndef _SRMV2_SPACE_MANAGEMENT_FUNCTIONS_
#define _SRMV2_SPACE_MANAGEMENT_FUNCTIONS_

#include "srm_ifce_internal.h"


int srmv2_getspacemd (struct srm_context *context,
        struct srm_getspacemd_input *input,struct srm_spacemd **spaces);

// returns space tokens associated to the space description
int srmv2_getspacetokens (struct srm_context *context,
        struct srm_getspacetokens_input *input,
        struct srm_getspacetokens_output *output);

// returns best space token
char* srmv2_getbestspacetoken (struct srm_context *context,
        struct srm_getbestspacetokens_input *input);

// returns space tokens associated to the space description
int srmv2_reservespace(struct srm_context *context,
        struct srm_reservespace_input *input,
        struct srm_reservespace_output *output);


int srmv2_releasespace(struct srm_context *context,
        char *spacetoken);

int srmv2_purgefromspace(struct srm_context *context,
        struct srm_purgefromspace_input *input,
        struct srm_purgefromspace_output *output);



#endif
