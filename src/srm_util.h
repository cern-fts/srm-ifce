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
#ifndef SRM_UTIL_H_
#define SRM_UTIL_H_

#include "time_utils.h"

//static allocation
void srm_soap_init(struct soap *soap);
void srm_soap_deinit(struct soap *soap);


// dyn allocation
/**
  dynamic allocation of a srm soap context
  timeouts are initialized from the default values
*/
struct soap * srm_soap_init_new();

/**
  dynamic allocation of a srm soap context
  timeouts are initialized from the context values ( default, excepted if user-change
*/
struct soap * srm_soap_init_context_new(struct srm_context* c);

/**
  free an srm gsoap context
*/
void srm_soap_free(struct soap *soap);

void srm_print_explanation(char **explanation,struct srm2__TReturnStatus *reqstatp,const char *srmfunc);

extern const char *err_msg_begin;
void srm_errmsg (struct srm_context *context, const char *format, ...);
int srm_soap_call_err(struct srm_context *context,struct soap *soap,const char *srmfunc);
int srm_call_err(struct srm_context *context,struct srm2__TReturnStatus  *retstatus,const char *srmfunc);
int srm_print_error_status(struct srm_context *context,struct srm2__TReturnStatus *status,const char *srmfunc);
int srm_print_error_status_additional(struct srm_context *context,struct srm2__TReturnStatus *status,const char *srmfunc,char *additional_info);

int statuscode2errno (int statuscode);
const char * statuscode2errmsg (int statuscode);

void back_off_logic_init(struct srm_context *context,struct srm_internal_context *internal_context);
srm_call_status back_off_logic(struct srm_context *context,const char *srmfunc,
		struct srm_internal_context *internal_context,struct srm2__TReturnStatus  *retstatus);
int wait_for_new_attempt(struct srm_internal_context *internal_context);

void srm_spacemd_free (int nbtokens, struct srm_spacemd *smd);

int srm_set_protocol_in_transferParameters(
	struct srm_context *context,
    struct soap* soap,
    struct srm2__TTransferParameters* transferParameters,
    char** protocols);

char* srm_strip_string(const char* str, const char chr);
int srm_count_elements_of_string_array(char** a);

int copy_string(char **dest,char *src);

int copy_permissionfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLPermissionReturn *repfs,
						const char *srmfunc,
						int amode);

int copy_filestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_filestatus **statuses,
						struct srm2__ArrayOfTSURLReturnStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_extendlifetime(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTSURLLifetimeReturnStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_get(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTGetRequestFileStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_put(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTPutRequestFileStatus *repfs,
						const char *srmfunc);

int copy_pinfilestatuses_bringonline(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_pinfilestatus **filestatuses,
						struct srm2__ArrayOfTBringOnlineRequestFileStatus *repfs,
						const char *srmfunc);

int copy_mdfilestatuses(struct srm2__TReturnStatus *reqstatp,
						struct srmv2_mdfilestatus **statuses,
						struct srm2__ArrayOfTMetaDataPathDetail *repfs);

int copy_returnstatus(struct srm2__TReturnStatus **destination,
		struct srm2__TReturnStatus *returnStatus);

int copy_filepermissions(struct srm2__TReturnStatus *reqstatp,
		struct srm_filepermission **permissions,
		struct srm2__ArrayOfTPermissionReturn *repperm);

void set_estimated_wait_time(struct srm_internal_context *internal_context, int time);

int wait_for_new_attempt_min_max_ng(srm_context_t context, struct srm_internal_context *internal_context);

int wait_switch_auto(srm_context_t context, struct srm_internal_context *internal_context);

/* General utility functions */

/**
 * Normalize a SURL: remove multiple slashes (exept for "srm://"), and add a single slash 
 * to the end of the SURL. For example, SURL
 *
 * srm://server:port/dir1//dir2
 *
 * will be
 *
 * srm://server:port/dir1/dir2/
 *
 * @param surl The SURL to be normalized
 *
 * @return The dyamically allocated normalized SURL.
*/
char* srm_util_normalize_surl(const char* surl);


/**
 * Remove multiple, subsequent characters from a string. For instance:
 *
 * srm_util_consolidate_multiple_characters("charrrrrstring, 'r');
 *
 * returns "charstring".
 *
 * The returned string is dynamically allocated, free it!
 *
 * @param s the string to be consolidated. 
 * @param c the character to look for
 * @param start consolidating starts from this index. Before this index, all the characters
 *              are copied.
 *
 * @return the consolidated string, dynamically allocated, or NULL if s was NULL.
 */
char* srm_util_consolidate_multiple_characters(const char* s, const char c, const int start);


/**
  * Add (concatenate) two strings into a dynamically allocated new string.
  *
  * @param s1 First string
  * @param s2 Second string (will be added after s2)
  *
  * @return The dynamically allocated concatenation.
  */
char* srm_util_add_strings(const char* s1, const char* s2);

#endif /* SRM_UTIL_H_ */
