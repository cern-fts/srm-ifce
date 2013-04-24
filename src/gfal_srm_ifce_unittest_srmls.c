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

#include "gfal_srm_ifce_unittest.h"
#include "srm_util.h"
#include "srmv2_directory_functions.h"

START_TEST (test_wait_for_new_attempt)
{
	struct srm_internal_context internal_context;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 11;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Wait for new attempt does not return timeout error for 11 attempts!");

	internal_context.attempt = 5;
	internal_context.end_time = time(NULL)-1;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Timeout error not received!");

	call_function.call_sleep = mock_sleep;
	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	fail_if (wait_for_new_attempt(&internal_context) != 0,
		   "Timeout should not occur!");
	fail_if (mock_sleep_time > 1, // be careful changing this number
		  "Random sleep time exceeded expected value !!!");


	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	internal_context.estimated_wait_time = 0;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Timeout should occur!");


	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	internal_context.estimated_wait_time = -10;
	fail_if (mock_sleep_time > 1, // be careful changing this number
			  "Random sleep time exceeded expected value !!!");

	internal_context.estimated_wait_time = 10;
	wait_for_new_attempt(&internal_context);
	fail_if (mock_sleep_time != 10,
			  "Sleep time estimated wait time not equal to sleep time!");
}
END_TEST


int  soap_call_srm2__srmLs_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	_param_18->srmLsResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmLs_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = NULL;
	resp->details = NULL;
	_param_18->srmLsResponse = resp;

	return -1; // failure
}

int  soap_call_srm2__srmLs_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->requestToken = NULL;
	resp->details = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmLsResponse = resp;

	return 0; // success
}

int  soap_call_srm2__srmLs_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = NULL;
	resp->details = NULL;
	_param_18->srmLsResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmLs_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = NULL;
	resp->requestToken = NULL;
	_param_18->srmLsResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmLs_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = NULL;
	resp->requestToken = fixture_test_string;
	_param_18->srmLsResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmLs_test7(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmLsResponse = resp;
	resp->details = (struct srm2__ArrayOfTMetaDataPathDetail*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail));
	resp->details->__sizepathDetailArray = 1;
	resp->details->pathDetailArray = (struct srm2__TMetaDataPathDetail **) soap_malloc (soap,sizeof (struct srm2__TMetaDataPathDetail *));
	resp->details->pathDetailArray[0] = (struct srm2__TMetaDataPathDetail *) soap_malloc (soap,sizeof (struct srm2__TMetaDataPathDetail ));
	resp->details->pathDetailArray[0] = NULL;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_ls_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_ls_async)
{
	struct srm_ls_output output;
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm_ls_input input;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.offset = NULL;
	input.nbfiles = 1;
	input.count = 0;
	input.numlevels  = 1;
	input.surls = test_surls_ls;
	input.offset = 0;

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test1;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!");

	internal_context.attempt = 1;
	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test2;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test3;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test4;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test5;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test6;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
    free(output.token); 
	srm_srm2__TReturnStatus_delete(output.retstatus);
    fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test7;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!");
}
END_TEST
int  soap_call_srm2__srmStatusOfLs_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequestRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfLsRequestResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmStatusOfLs_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequestRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = NULL;
	_param_18->srmStatusOfLsRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfLs_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequestRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = NULL;
	_param_18->srmStatusOfLsRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfLs_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequestRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = (struct srm2__ArrayOfTMetaDataPathDetail*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail));
	resp->details->__sizepathDetailArray = 1;
	resp->details->pathDetailArray = (struct srm2__TMetaDataPathDetail **) soap_malloc (soap,sizeof (struct srm2__TMetaDataPathDetail  *));
	resp->details->pathDetailArray[0] = (struct srm2__TMetaDataPathDetail *) soap_malloc (soap,sizeof (struct srm2__TMetaDataPathDetail ));
	resp->details->pathDetailArray[0] = NULL;
	_param_18->srmStatusOfLsRequestResponse = resp;


	return 0; // success
}


START_TEST (test_srmv2_status_of_ls_request)
{
	int i;
	struct srm_ls_input input;
	struct srm_ls_output output;
	struct srm_context context;
	struct srm_internal_context internal_context;
	int result;


	input.offset = NULL;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	output.token = "test";

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test1;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test2;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls!");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test3;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test4;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  != 1),
				   "Expected Success!");


}
END_TEST

void gfal_unittest_srmls_test_suite (Suite *s)
{
  TCase *tc_case_1 = tcase_create ("SrmLs function tests");
  tcase_add_checked_fixture (tc_case_1, NULL,NULL);
  tcase_add_test (tc_case_1, test_srmv2_ls_async);
  tcase_add_test (tc_case_1, test_srmv2_status_of_ls_request);
  tcase_add_test (tc_case_1, test_wait_for_new_attempt);
  suite_add_tcase (s, tc_case_1);
}

