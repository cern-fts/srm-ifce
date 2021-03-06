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
#ifndef SRM_IFCE_INTERNAL_H_
#define SRM_IFCE_INTERNAL_H_


#include <errno.h>
#include <glib.h>
#include <time.h>

#include "gfal_srm_ifce.h"
#include "srm_dependencies.h"
#include "srm_util.h"
#include "srmv2_discovery_functions.h"
#include "srmv2_permission_functions.h"
#include "srmv2_data_transfer_functions.h"
#include "srmv2_directory_functions.h"
#include "srmv2_space_management_functions.h"

typedef enum _srm_polling_logic{
    SRM_POLLING_LOGIC_OLD,
    SRM_POLLING_LOGIC_MIN_MAX_EXP
} srm_polling_logic;

struct srm_context_extension{
    // asynchronous query strategy
    srm_polling_logic polling_logic;
    // polling MIN_MAX_EXP params
    struct timespec min_waittime;
    struct timespec max_waittime;
    // advanced timeout management
    int turl_resolution_timeout;
    // keep alive connections
    int keep_alive;
    // user credentials
    char *ucert, *ukey;

    // Original post header method
    int (*original_fposthdr)(struct soap*, const char*, const char*);

    // List of key/values headers
    GData* additional_headers;

    // Append to the User-Agent
    char user_agent[128];
};

/* Normal assertion for srm-ifce */
#define GFAL_SRM_IFCE_ASSERT(cond) \
    do{ \
        g_assert((cond)); \
    }while(0)




#endif

