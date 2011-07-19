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
#ifndef _GFAL_SRM_IFCE_UNITTEST_H_
#define _GFAL_SRM_IFCE_UNITTEST_H_

#include <check.h>
#include "srm_dependencies.h"
#include "gfal_srm_ifce.h"

/* Test functions */
extern void gfal_unittest_srmls_test_suite (Suite *s);

/* Mock functions */
extern unsigned int mock_sleep(unsigned int time);

/* Fixtures */
extern char fixture_test_string[];

/* Global properties */
extern unsigned int mock_sleep_time;

#endif /* _GFAL_SRM_IFCE_UNITTEST_H */


