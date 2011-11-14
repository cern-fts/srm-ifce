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
 * Authors: Zsolt Molnar, CERN
 */
#ifndef _GFAL_SRM_IFCE_INTERNAL_H_
#define _GFAL_SRM_IFCE_INTERNAL_H_

#include <assert.h>
#include "gfal_srm_ifce.h"

/* Normal assertion happens in debug mode. In non-debug mode, force crash! */
#define GFAL_SRM_IFCE_ASSERT(cond) \
    assert((cond)); \
    if ( ! (cond) ) {\
        char* c = NULL; \
        c = 0; \
    }

#endif /* _GFAL_SRM_IFCE_INTERNAL_H_ */
