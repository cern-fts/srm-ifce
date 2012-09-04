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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <errno.h>
#include "srmv2H.h"
#include "gfal_srm_ifce_types.h"
#include "srm_util.h"
#include "srm_soap.h"

#include "gfal_srm_ifce_unittest.h"

char test_dir[] = "/test/1/2/3/4";
char* fixture_test_strings[3]= { "fixture_test_string1","fixture_test_string2",NULL};
void PrintResult(struct srmv2_mdfilestatus* output);

START_TEST (test_wait_for_new_attempt)
{
	struct srm_internal_context internal_context;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 11;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Wait for new attempt does not return timeout error for 11 attempts!");

	internal_context.attempt = 5;
    internal_context.end_time = time(NULL)-20;
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
    fail_if (wait_for_new_attempt(&internal_context) != 0,
           "Timeout should not occur, estimated wait time == 0 ignored !");


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

//////////////////////////////////////////////////////////////////
// test wait_for_new_attempt function
//////////////////////////////////////////////////////////////////
START_TEST (test_back_off_logic)
{
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	srm_call_status result;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	call_function.call_sleep = mock_sleep; // set mock sleep function

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+10000;

	result = back_off_logic(&context,srmfunc,&internal_context,NULL);
	fail_if ((result  != srm_call_status_FAILURE)||(errno != ECOMM),
			   "if internal_context->retstatus is NULL the function must return FAILURE!");

	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);
	fail_if ((internal_context.attempt   != 2),
				   "Wait new attempt!");

	fail_if ((result  != srm_call_status_INTERNAL_ERROR),
				   "Expected Internal Error!");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 1!");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 2!");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 3!");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 1!");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 2!");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCORESUCCESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_SUCCESS),
					   "Expected Success!");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREFAILURE;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_FAILURE),
					   "Expected Failure!");
}
END_TEST

int soap_call_srm2__abort_request_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
		struct srm2__srmAbortRequestRequest *request,
	    struct srm2__srmAbortRequestResponse_ *_param_18)
{
	_param_18 = NULL;
	return 0;
}
int soap_call_srm2__abort_request_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
		struct srm2__srmAbortRequestRequest *request,
	    struct srm2__srmAbortRequestResponse_ *_param_18)
{
	return -1;
}
int soap_call_srm2__abort_request_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action,
		struct srm2__srmAbortRequestRequest *request,
	    struct srm2__srmAbortRequestResponse_ *_param_18)
{
	struct srm2__srmAbortRequestResponse *resp  = (struct srm2__srmAbortRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmAbortRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR; // Failure
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmAbortRequestResponse = resp;

	return 0; // success
}
int soap_call_srm2__abort_request_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action,
		struct srm2__srmAbortRequestRequest *request,
	    struct srm2__srmAbortRequestResponse_ *_param_18)
{
	struct srm2__srmAbortRequestResponse *resp  = (struct srm2__srmAbortRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmAbortRequestResponse));
	resp->returnStatus = NULL; // FAILURE
	_param_18->srmAbortRequestResponse = resp;

	return 0; // success
}
int soap_call_srm2__abort_request_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action,
		struct srm2__srmAbortRequestRequest *request,
	    struct srm2__srmAbortRequestResponse_ *_param_18)
{
	struct srm2__srmAbortRequestResponse *resp  = (struct srm2__srmAbortRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmAbortRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmAbortRequestResponse = resp;

	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_abort_request
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_abort_request)
{
	const char *token = "testtoken";
	struct srm_context context;
	struct srm2__TReturnStatus retstatus;
	srm_call_status result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test1;
	result = srmv2_abort_request(&context,NULL);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test2;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != -1),
				   "Expected Failure!");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test3;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != -1),
				   "Expected Failure!");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test4;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != -1),
				   "Expected Failure!");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test5;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != 0),
				   "Expected Success!");
}
END_TEST
int  soap_call_srm2__srmRmDir_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmRmdirRequest *srmRmdirRequest,
						struct srm2__srmRmdirResponse_ *_param_18)
{
	struct srm2__srmRmdirResponse *resp  = (struct srm2__srmRmdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmRmdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmRmdirResponse = resp;
	return 0;
}
int  soap_call_srm2__srmRmDir_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmRmdirRequest *srmRmdirRequest,
						struct srm2__srmRmdirResponse_ *_param_18)
{
    _param_18->srmRmdirResponse = NULL;
    return -1;
}
int  soap_call_srm2__srmRmDir_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmRmdirRequest *srmRmdirRequest,
						struct srm2__srmRmdirResponse_ *_param_18)
{
	struct srm2__srmRmdirResponse *resp  = (struct srm2__srmRmdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmRmdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmRmdirResponse = resp;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_rmdir
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_rmdir)
{
	struct srmv2_filestatus *filestatus;
	struct srm_rmdir_input input;
	struct srm_rmdir_output output;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.surl = fixture_test_string;
	input.recursive = 0;

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test1;
	result = srmv2_rmdir(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != 1),
				   "Expected Success!");

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test2;
	result = srmv2_rmdir(&context,&input,&output);
	fail_if ((result  != -1),
				   "Expected Failure 2!");

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test3;
	result = srmv2_rmdir(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != 1),
				   "Expected Success!");
}
END_TEST
int  soap_call_srm2__srmRm_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmRmRequest *srmRmRequest,
						struct srm2__srmRmResponse_ *_param_18)
{
	struct srm2__srmRmResponse *resp  = (struct srm2__srmRmResponse *) soap_malloc (soap,sizeof (struct srm2__srmRmResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmRmResponse = resp;
	return 0;
}
int  soap_call_srm2__srmRm_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmRmRequest *srmRmRequest,
						struct srm2__srmRmResponse_ *_param_18)
{
	struct srm2__srmRmResponse *resp  = (struct srm2__srmRmResponse *) soap_malloc (soap,sizeof (struct srm2__srmRmResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmRmResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;

	return 0;
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_rm
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_rm)
{
	struct srmv2_filestatus *filestatus;
	struct srm_rm_input input;
	struct srm_rm_output output;
	struct srm_context context;

	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.surls = fixture_test_strings;
	input.nbfiles = 1;

	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test1;
	result = srmv2_rm(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test2;
	result = srmv2_rm(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != 1),
				   "Expected Success!");
}
END_TEST

int  soap_call_srm2__srmMkdir_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmMkdirRequest *srmMkdirRequest,
						struct srm2__srmMkdirResponse_ *_param_18)
{
	struct srm2__srmMkdirResponse *resp  = (struct srm2__srmMkdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmMkdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmMkdirResponse = resp;
	return 0;
}
int  soap_call_srm2__srmMkdir_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmMkdirRequest *srmMkdirRequest,
						struct srm2__srmMkdirResponse_ *_param_18)
{
	struct srm2__srmMkdirResponse *resp  = (struct srm2__srmMkdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmMkdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmMkdirResponse = resp;

	return 0;
}
int  soap_call_srm2__srmMkdir_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmMkdirRequest *srmMkdirRequest,
						struct srm2__srmMkdirResponse_ *_param_18)
{
	struct srm2__srmMkdirResponse *resp  = (struct srm2__srmMkdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmMkdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREDUPLICATION_USCOREERROR; // dir already exist
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmMkdirResponse = resp;

	return 0;
}
int mkdir_test_ok = 0;
int  soap_call_srm2__srmMkdir_test_last_level(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmMkdirRequest *srmMkdirRequest,
						struct srm2__srmMkdirResponse_ *_param_18)
{
	int i,n;
	struct srm2__srmMkdirResponse *resp  = (struct srm2__srmMkdirResponse *) soap_malloc (soap,sizeof (struct srm2__srmMkdirResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmMkdirResponse = resp;

	mkdir_test_ok = 0;
	n = strlen(test_dir);
	// Check if we create the full directory not using strcmp because of last slash char
	for(i=0;i<n && i<strlen(srmMkdirRequest->SURL);i++)
	{
		if (srmMkdirRequest->SURL[i] != test_dir[i])
		{
			break;
		}
	}
	if ((i == n) ||
			(i == (n-1) && test_dir[n-1] == '/'))
	{
		mkdir_test_ok = 1;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_mkdir
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_mkdir)
{
	struct srm_mkdir_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.dir_name = test_dir;

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test1;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test2;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != 0),
				   "Expected Success!");

	/*call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test3;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != -1),
				   "Expected Failure 1!");
*/
	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test_last_level;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != 0),
				   "Expected Success!");
	fail_if ((mkdir_test_ok  != 1),
				   "Expected Success!");

}
END_TEST

int  soap_call_srm2__srmAbortFiles_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmAbortFilesRequest *srmAbortFilesRequest,
						struct srm2__srmAbortFilesResponse_ *_param_18)
{
	struct srm2__srmAbortFilesResponse *resp  = (struct srm2__srmAbortFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmAbortFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST; // token empty
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmAbortFilesResponse = resp;

	return 0;
}

int  soap_call_srm2__srmAbortFiles_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmAbortFilesRequest *srmAbortFilesRequest,
						struct srm2__srmAbortFilesResponse_ *_param_18)
{
	struct srm2__srmAbortFilesResponse *resp  = (struct srm2__srmAbortFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmAbortFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS; // token empty
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmAbortFilesResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;

	return 0;
}

//////////////////////////////////////////////////////////////////
// test test_abort_files
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_abort_files)
{
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	struct srmv2_filestatus *statuses;
	struct srm_abort_files_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test1;
	result = srmv2_abort_files(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test2;
	result = srmv2_abort_files(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!");
}
END_TEST

int  soap_call_srm2__srmPutDone_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmPutDoneRequest *srmAbortFilesRequest,
						struct srm2__srmPutDoneResponse_ *_param_18)
{
	struct srm2__srmPutDoneResponse *resp  = (struct srm2__srmPutDoneResponse *) soap_malloc (soap,sizeof (struct srm2__srmPutDoneResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPutDoneResponse = resp;

	return 0;
}
int  soap_call_srm2__srmPutDone_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmPutDoneRequest *srmAbortFilesRequest,
						struct srm2__srmPutDoneResponse_ *_param_18)
{
	struct srm2__srmPutDoneResponse *resp  = (struct srm2__srmPutDoneResponse *) soap_malloc (soap,sizeof (struct srm2__srmPutDoneResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmPutDoneResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;


	return 0;
}

int  soap_call_srm2__srmPutDone_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmPutDoneRequest *srmAbortFilesRequest,
						struct srm2__srmPutDoneResponse_ *_param_18)
{
	struct srm2__srmPutDoneResponse *resp  = (struct srm2__srmPutDoneResponse *) soap_malloc (soap,sizeof (struct srm2__srmPutDoneResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmPutDoneResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;


	return 0;
}

int  soap_call_srm2__srmPutDone_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmPutDoneRequest *srmAbortFilesRequest,
						struct srm2__srmPutDoneResponse_ *_param_18)
{
	struct srm2__srmPutDoneResponse *resp  = (struct srm2__srmPutDoneResponse *) soap_malloc (soap,sizeof (struct srm2__srmPutDoneResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPutDoneResponse = NULL; // THIS FAILS


	return 0;
}
int  soap_call_srm2__srmPutDone_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmPutDoneRequest *srmAbortFilesRequest,
						struct srm2__srmPutDoneResponse_ *_param_18)
{
	struct srm2__srmPutDoneResponse *resp  = (struct srm2__srmPutDoneResponse *) soap_malloc (soap,sizeof (struct srm2__srmPutDoneResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = NULL;// THIS FAILS
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPutDoneResponse = resp;


	return 0;
}


//////////////////////////////////////////////////////////////////
// test test_put_done
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_put_done)
{
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	struct srmv2_filestatus *statuses;
	struct srm_putdone_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test1;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test2;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test3;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test4;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test5;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!");
}
END_TEST
int  soap_call_srm2__srmReleaseFiles_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmReleaseFilesRequest *srmAbortFilesRequest,
						struct srm2__srmReleaseFilesResponse_ *_param_18)
{
	struct srm2__srmReleaseFilesResponse *resp  = (struct srm2__srmReleaseFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmReleaseFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINVALID_USCOREREQUEST;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmReleaseFilesResponse = resp;

	return 0;
}
int  soap_call_srm2__srmReleaseFiles_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmReleaseFilesRequest *srmAbortFilesRequest,
						struct srm2__srmReleaseFilesResponse_ *_param_18)
{
	struct srm2__srmReleaseFilesResponse *resp  = (struct srm2__srmReleaseFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmReleaseFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmReleaseFilesResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;


	return 0;
}

int  soap_call_srm2__srmReleaseFiles_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmReleaseFilesRequest *srmAbortFilesRequest,
						struct srm2__srmReleaseFilesResponse_ *_param_18)
{
	struct srm2__srmReleaseFilesResponse *resp  = (struct srm2__srmReleaseFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmReleaseFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmReleaseFilesResponse = resp;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLReturnStatus *)soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;


	return 0;
}

int  soap_call_srm2__srmReleaseFiles_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmReleaseFilesRequest *srmAbortFilesRequest,
						struct srm2__srmReleaseFilesResponse_ *_param_18)
{
	struct srm2__srmReleaseFilesResponse *resp  = (struct srm2__srmReleaseFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmReleaseFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmReleaseFilesResponse = NULL; // THIS FAILS


	return 0;
}
int  soap_call_srm2__srmReleaseFiles_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action,
						struct srm2__srmReleaseFilesRequest *srmAbortFilesRequest,
						struct srm2__srmReleaseFilesResponse_ *_param_18)
{
	struct srm2__srmReleaseFilesResponse *resp  = (struct srm2__srmReleaseFilesResponse *) soap_malloc (soap,sizeof (struct srm2__srmReleaseFilesResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = NULL;// THIS FAILS
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmReleaseFilesResponse = resp;


	return 0;
}


//////////////////////////////////////////////////////////////////
// test test_release_files
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_release_files)
{
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	struct srmv2_filestatus *statuses;
	struct srm_releasefiles_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test1;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test2;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test3;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test4;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test5;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!");
}
END_TEST
int  soap_call_srm2__srmBringOnline_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	resp->returnStatus = NULL;
	resp->remainingTotalRequestTime = NULL;
	resp->requestToken = NULL;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmBringOnline_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->remainingTotalRequestTime = NULL;
	resp->requestToken = NULL;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	return -1; // failure
}

int  soap_call_srm2__srmBringOnline_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	resp->remainingTotalRequestTime = NULL;
	resp->requestToken = NULL;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
	retstatus->explanation = NULL;
	return 0; // success
}

int  soap_call_srm2__srmBringOnline_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = NULL;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	_param_18->srmBringOnlineResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmBringOnline_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = NULL;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	_param_18->srmBringOnlineResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmBringOnline_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = fixture_test_string;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse = resp;
	_param_18->srmBringOnlineResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmBringOnline_test7(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmBringOnlineResponse = resp;
	_param_18->srmBringOnlineResponse->remainingTotalRequestTime = NULL;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TBringOnlineRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__TBringOnlineRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__TBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_ls_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_bring_online_async)
{
	struct srm_bringonline_input input;
	struct srm_bringonline_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test1;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test2;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test3;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test4;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test5;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test6;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	free(output.token);
    fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!");


	output.filestatuses= filestatus;
	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test7;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!");
}
END_TEST
int  soap_call_srm2__srmStatusOfBringOnline_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequestRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfBringOnlineRequestResponse = NULL;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmStatusOfBringOnline_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequestRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfBringOnline_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequestRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfBringOnline_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequestRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TBringOnlineRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__TBringOnlineRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__TBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfBringOnline_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequestRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL; // FAILS
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;


	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_status_of_bring_online
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_status_of_bring_online_async)
{
	int i;
	struct srm_bringonline_input input;
	struct srm_bringonline_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test1;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test2;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test3;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= filestatus;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test4;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!");


	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test5;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!");
}
END_TEST

int  soap_call_srm2__srmPrepareToGet_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	_param_18->srmPrepareToGetResponse = NULL;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmPrepareToGet_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmBringOnlineRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;
	return -1; // failure
}

int  soap_call_srm2__srmPrepareToGet_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
	retstatus->explanation = NULL;
	return 0; // success
}

int  soap_call_srm2__srmPrepareToGet_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = NULL;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToGet_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = NULL;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToGet_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = fixture_test_string;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToGet_test7(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TGetRequestFileStatus **) soap_malloc (soap,sizeof (struct srm2__TGetRequestFileStatus  *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TGetRequestFileStatus *) soap_malloc (soap,sizeof (struct srm2__TGetRequestFileStatus ));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_prepare_to_get_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_prepare_to_get_async)
{
	struct srm_preparetoget_input input;
	struct srm_preparetoget_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test1;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test2;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test3;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test4;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test5;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test6;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
    free(output.token);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!");


	output.filestatuses= filestatus;
	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test7;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!");
}
END_TEST
int  soap_call_srm2__srmStatusOfGet_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequestRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfGetRequestResponse = NULL;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmStatusOfGet_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequestRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfGet_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequestRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfGet_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequestRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TGetRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__TGetRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__TGetRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfGet_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequestRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL; // FAILS
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_status_of_get_request_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_status_of_get_request_async)
{
	int i;
	struct srm_preparetoget_input input;
	struct srm_preparetoget_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test1;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test2;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED),
				   "Expected Queued in first call!");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	    srm_srm2__TReturnStatus_delete(output.retstatus);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test3;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= filestatus;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test4;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!");


	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test5;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!");
}
END_TEST
int  soap_call_srm2__srmPrepareToPut_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	_param_18->srmPrepareToPutResponse = NULL;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmPrepareToPut_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmBringOnlineRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREFAILURE;
	retstatus->explanation = NULL;

	return -1; // failure
}

int  soap_call_srm2__srmPrepareToPut_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
	retstatus->explanation = NULL;

	return 0; // success
}

int  soap_call_srm2__srmPrepareToPut_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = NULL;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToPut_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = NULL;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToPut_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->requestToken = fixture_test_string;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToPut_test7(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TPutRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__TPutRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__TPutRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_prepare_to_put_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_prepare_to_put_async)
{
	struct srm_preparetoput_input input;
	struct srm_preparetoput_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	SRM_LONG64 filesizes_test[] = { 1024 };
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.filesizes = filesizes_test;
	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test1;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test2;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test3;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test4;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test5;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test6;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
    free(output.token);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!");


	input.filesizes = NULL;
	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test7;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result != -1),
					"Expected Failure 4!");

	input.filesizes = filesizes_test;
	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test7;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!");
}
END_TEST
int  soap_call_srm2__srmStatusOfPut_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequestRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfPutRequestResponse = NULL;
	return 0; // success but return statuses are NULL
}

int  soap_call_srm2__srmStatusOfPut_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequestRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmStatusOfPutRequestResponse = resp;
	resp->remainingTotalRequestTime = NULL;
	return 0; // success
}
int  soap_call_srm2__srmStatusOfPut_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequestRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	resp->remainingTotalRequestTime = NULL;
	_param_18->srmStatusOfPutRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfPut_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequestRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->remainingTotalRequestTime = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TPutRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__TPutRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__TPutRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfPutRequestResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfPut_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequestRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL; // FAILS
	resp->remainingTotalRequestTime = NULL;
	_param_18->srmStatusOfPutRequestResponse = resp;


	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_status_of_put_request_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_status_of_put_request_async)
{
	int i;
	struct srm_preparetoput_input input;
	struct srm_preparetoput_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2",NULL};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... putbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test1;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test2;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	    srm_srm2__TReturnStatus_delete(output.retstatus);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test3;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= filestatus;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test4;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!");


	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test5;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!");
}
END_TEST

int  soap_call_srm2__srmGetSpaceMetaData_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceMetaDataRequest *srmGetSpaceMetaDataRequest,
						struct srm2__srmGetSpaceMetaDataResponse_ *_param_18)
{
	struct srm2__srmGetSpaceMetaDataResponse *resp  = (struct srm2__srmGetSpaceMetaDataResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceMetaDataResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceDetails = NULL; // FAILS
	_param_18->srmGetSpaceMetaDataResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmGetSpaceMetaData_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceMetaDataRequest *srmGetSpaceMetaDataRequest,
						struct srm2__srmGetSpaceMetaDataResponse_ *_param_18)
{
	_param_18->srmGetSpaceMetaDataResponse = NULL; // FAILS
	return 0; // success
}
int  soap_call_srm2__srmGetSpaceMetaData_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceMetaDataRequest *srmGetSpaceMetaDataRequest,
						struct srm2__srmGetSpaceMetaDataResponse_ *_param_18)
{
	struct srm2__srmGetSpaceMetaDataResponse *resp  = (struct srm2__srmGetSpaceMetaDataResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceMetaDataResponse));
	resp->returnStatus = NULL;// FAILS
	resp->arrayOfSpaceDetails = NULL;
	_param_18->srmGetSpaceMetaDataResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmGetSpaceMetaData_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceMetaDataRequest *srmGetSpaceMetaDataRequest,
						struct srm2__srmGetSpaceMetaDataResponse_ *_param_18)
{
	struct srm2__srmGetSpaceMetaDataResponse *resp  = (struct srm2__srmGetSpaceMetaDataResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceMetaDataResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceDetails = NULL;
	_param_18->srmGetSpaceMetaDataResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmGetSpaceMetaData_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceMetaDataRequest *srmGetSpaceMetaDataRequest,
						struct srm2__srmGetSpaceMetaDataResponse_ *_param_18)
{
	struct srm2__srmGetSpaceMetaDataResponse *resp  = (struct srm2__srmGetSpaceMetaDataResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceMetaDataResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceDetails = (struct srm2__ArrayOfTMetaDataSpace*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataSpace));
	resp->arrayOfSpaceDetails->__sizespaceDataArray = 1;
	resp->arrayOfSpaceDetails->spaceDataArray = (struct srm2__TMetaDataSpace**) soap_malloc (soap,sizeof (struct srm2__TMetaDataSpace *));
	resp->arrayOfSpaceDetails->spaceDataArray[0] = (struct srm2__TMetaDataSpace*) soap_malloc (soap,sizeof (struct srm2__TMetaDataSpace));
	resp->arrayOfSpaceDetails->spaceDataArray[0] = NULL;
	_param_18->srmGetSpaceMetaDataResponse = resp;



	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_getspacemd
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_getspacemd)
{
	int i;
	struct srm_getspacemd_input input;
	struct srm_spacemd *spaces;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;

	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_tokens1[] = {"token1",NULL};
	char *test_tokens2[] = {"token1","token2",NULL};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.nbtokens = 0;
	input.spacetokens = NULL;

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
				    "Expected Failure !");

	input.nbtokens = 1;
	input.spacetokens = NULL;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
				    "Expected Failure !");

	input.nbtokens = 1; // FAILS wrong count
	input.spacetokens = test_tokens2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !");

	input.nbtokens = 2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test2;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test3;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test4;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !");


	input.nbtokens = 1;
	input.spacetokens = test_tokens1;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test5;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != 0),
					"Expected Success!");
}
END_TEST


int  soap_call_srm2__srmGetSpaceTokens_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	struct srm2__srmGetSpaceTokensResponse *resp  = (struct srm2__srmGetSpaceTokensResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceTokensResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceTokens = NULL; // FAILS
	_param_18->srmGetSpaceTokensResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmGetSpaceTokens_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	_param_18->srmGetSpaceTokensResponse = NULL; // FAILS
	return 0; // success
}
int  soap_call_srm2__srmGetSpaceTokens_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	struct srm2__srmGetSpaceTokensResponse *resp  = (struct srm2__srmGetSpaceTokensResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceTokensResponse));
	resp->returnStatus = NULL;// FAILS
	resp->arrayOfSpaceTokens = NULL;
	_param_18->srmGetSpaceTokensResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmGetSpaceTokens_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	struct srm2__srmGetSpaceTokensResponse *resp  = (struct srm2__srmGetSpaceTokensResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceTokensResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceTokens = NULL;
	_param_18->srmGetSpaceTokensResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmGetSpaceTokens_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	struct srm2__srmGetSpaceTokensResponse *resp  = (struct srm2__srmGetSpaceTokensResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetSpaceTokensResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfSpaceTokens = (struct srm2__ArrayOfString*) soap_malloc (soap,sizeof (struct srm2__ArrayOfString));
	resp->arrayOfSpaceTokens->__sizestringArray = 1;
	resp->arrayOfSpaceTokens->stringArray = (char **) soap_malloc (soap,sizeof (char *));
	resp->arrayOfSpaceTokens->stringArray[0] = (char *) soap_malloc (soap,sizeof (char));
	resp->arrayOfSpaceTokens->stringArray[0] = fixture_test_string;
	_param_18->srmGetSpaceTokensResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmGetSpaceTokens_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetSpaceTokensRequest *srmGetSpaceTokensRequest,
						struct srm2__srmGetSpaceTokensResponse_ *_param_18)
{
	return -1; // failure
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_getspacetokens
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_getspacetokens)
{
	int i;
	struct srm_getspacetokens_input input;
	struct srm_getspacetokens_output output;
	struct srm_context context;

	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_tokens1[] = {"token1",NULL};
	char *test_tokens2[] = {"token1","token2",NULL};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	input.spacetokendesc = NULL;
	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	input.spacetokendesc = "MySpaceTokens";

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test2;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test3;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test4;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test5;
	result = srmv2_getspacetokens(&context,&input,&output);
    fail_if ((result  != 0),
					"Expected Success!");
	fail_if ((strcmp(output.spacetokens[0],fixture_test_string)  != 0),
						"Expected the same string !");
	free(output.spacetokens[0]);
	free(output.spacetokens);

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test6;
	result = srmv2_getspacetokens(&context,&input,&output);
	free(output.spacetokens);
	fail_if ((result  != -1),
					"Expected Failure !");

}
END_TEST

int  soap_call_srm2__srmPing_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPingRequest *srmPingRequest,
						struct srm2__srmPingResponse_ *_param_18)
{
	struct srm2__srmPingResponse *resp  = (struct srm2__srmPingResponse *) soap_malloc (soap,sizeof (struct srm2__srmPingResponse));
	resp->versionInfo = fixture_test_string;
	_param_18->srmPingResponse = resp;


	return 0; // success
}
int  soap_call_srm2__srmPing_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPingRequest *srmPingRequest,
						struct srm2__srmPingResponse_ *_param_18)
{
	struct srm2__srmPingResponse *resp  = (struct srm2__srmPingResponse *) soap_malloc (soap,sizeof (struct srm2__srmPingResponse));
	resp->versionInfo = NULL; // FAIL
	_param_18->srmPingResponse = resp;

	return 0;
}
int  soap_call_srm2__srmPing_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPingRequest *srmPingRequest,
						struct srm2__srmPingResponse_ *_param_18)
{
	_param_18->srmPingResponse = NULL; // FAIL

	return 0;
}
int  soap_call_srm2__srmPing_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPingRequest *srmPingRequest,
						struct srm2__srmPingResponse_ *_param_18)
{
	struct srm2__srmPingResponse *resp  = (struct srm2__srmPingResponse *) soap_malloc (soap,sizeof (struct srm2__srmPingResponse));
	resp->versionInfo = fixture_test_string;
	_param_18->srmPingResponse = resp;

	return -1; // fail
}


//////////////////////////////////////////////////////////////////
// test test_srmv2_ping
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_ping)
{
	int i;
	struct srm_ping_output output;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);

	call_function.call_srm2__srmPing = soap_call_srm2__srmPing_test1;
	result = srmv2_ping(&context,&output);
    free(output.versioninfo);
	fail_if ((result  != 0),
					"Expected Success !");

	call_function.call_srm2__srmPing = soap_call_srm2__srmPing_test2;
	result = srmv2_ping(&context,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmPing = soap_call_srm2__srmPing_test3;
	result = srmv2_ping(&context,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmPing = soap_call_srm2__srmPing_test4;
	result = srmv2_ping(&context,&output);
	fail_if ((result  != -1),
					"Expected Failure !");
}
END_TEST

int  soap_call_srm2__srmSetPermission_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	struct srm2__srmSetPermissionResponse *resp  = (struct srm2__srmSetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmSetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmSetPermissionResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmSetPermission_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	_param_18->srmSetPermissionResponse = NULL; // FAILS
	return 0; // success
}
int  soap_call_srm2__srmSetPermission_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	struct srm2__srmSetPermissionResponse *resp  = (struct srm2__srmSetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmSetPermissionResponse));
	resp->returnStatus = NULL;// FAILS
	_param_18->srmSetPermissionResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmSetPermission_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	struct srm2__srmSetPermissionResponse *resp  = (struct srm2__srmSetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmSetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmSetPermissionResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmSetPermission_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	struct srm2__srmSetPermissionResponse *resp  = (struct srm2__srmSetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmSetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmSetPermissionResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmSetPermission_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmSetPermissionRequest *srmSetPermissionRequest,
						struct srm2__srmSetPermissionResponse_ *_param_18)
{
	return -1; // failure
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_set_permission
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_set_permission)
{
	int i;
	struct srm_setpermission_input input;
	struct srm_permission user_perm;
	struct srm_context context;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	context.version = VERSION_2_2;

	input.surl = test_surls[0];
	input.owner_permission = SRM_PERMISSION_RWX;
	input.other_permission = SRM_PERMISSION_RW;
	input.group_permissions_count = 0;
	input.group_permissions = NULL;

	user_perm.mode = SRM_PERMISSION_RWX;
	user_perm.name_id = "tmanev";

	input.user_permissions_count = 1;
	input.user_permissions = &user_perm;
	input.permission_type = SRM_PERMISSION_ADD;



	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test1;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != 0),
					"Expected Success !");

	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test2;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test3;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test4;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test5;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != 0),
					"Expected Success!");

	call_function.call_srm2__srmSetPermission = soap_call_srm2__srmSetPermission_test6;
	result = srmv2_set_permission(&context,&input);
	fail_if ((result  != -1),
					"Expected Failure !");
}
END_TEST

int  soap_call_srm2__srmGetPermission_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	struct srm2__srmGetPermissionResponse *resp  = (struct srm2__srmGetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfPermissionReturns = NULL;
	_param_18->srmGetPermissionResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmGetPermission_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	_param_18->srmGetPermissionResponse = NULL; // FAILS
	return 0; // success
}
int  soap_call_srm2__srmGetPermission_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	struct srm2__srmGetPermissionResponse *resp  = (struct srm2__srmGetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetPermissionResponse));
	resp->returnStatus = NULL;// FAILS
	resp->arrayOfPermissionReturns = NULL;
	_param_18->srmGetPermissionResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmGetPermission_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	struct srm2__srmGetPermissionResponse *resp  = (struct srm2__srmGetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfPermissionReturns = NULL;
	_param_18->srmGetPermissionResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmGetPermission_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	struct srm2__srmGetPermissionResponse *resp  = (struct srm2__srmGetPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmGetPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;


	resp->arrayOfPermissionReturns = (struct srm2__ArrayOfTPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPermissionReturn));
	resp->arrayOfPermissionReturns->__sizepermissionArray = 1;
	resp->arrayOfPermissionReturns->permissionArray = (struct srm2__TPermissionReturn**) soap_malloc (soap,sizeof (struct srm2__TPermissionReturn *));
	resp->arrayOfPermissionReturns->permissionArray[0] = (struct srm2__TPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__TPermissionReturn));
	resp->arrayOfPermissionReturns->permissionArray[0]->surl = fixture_test_string;
	resp->arrayOfPermissionReturns->permissionArray[0]->owner = fixture_test_string;
	resp->arrayOfPermissionReturns->permissionArray[0]->ownerPermission = NULL;
	resp->arrayOfPermissionReturns->permissionArray[0]->arrayOfGroupPermissions = NULL;
	resp->arrayOfPermissionReturns->permissionArray[0]->arrayOfUserPermissions = NULL;
	resp->arrayOfPermissionReturns->permissionArray[0]->otherPermission = NULL;
	resp->arrayOfPermissionReturns->permissionArray[0]->status = retstatus;

	_param_18->srmGetPermissionResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmGetPermission_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmGetPermissionRequest *srmGetPermissionRequest,
						struct srm2__srmGetPermissionResponse_ *_param_18)
{
	return -1; // failure
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_get_permission
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_get_permission)
{
	int i;
	struct srm_getpermission_input input;
	struct srm_getpermission_output output;
	struct srm_context context;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	context.version = VERSION_2_2;

	input.nbfiles = 1;
	input.surls =  test_surls;


	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test1;
	result = srmv2_get_permission(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test2;
	result = srmv2_get_permission(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test3;
	result = srmv2_get_permission(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test4;
	result = srmv2_get_permission(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test5;
	result = srmv2_get_permission(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != 1),
					"Expected Success!");

	call_function.call_srm2__srmGetPermission = soap_call_srm2__srmGetPermission_test6;
	result = srmv2_get_permission(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");
}
END_TEST
int  soap_call_srm2__srmCheckPermission_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	struct srm2__srmCheckPermissionResponse *resp  = (struct srm2__srmCheckPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmCheckPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfPermissions = NULL;
	_param_18->srmCheckPermissionResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmCheckPermission_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	_param_18->srmCheckPermissionResponse = NULL; // FAILS
	return 0; // success
}
int  soap_call_srm2__srmCheckPermission_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	struct srm2__srmCheckPermissionResponse *resp  = (struct srm2__srmCheckPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmCheckPermissionResponse));
	resp->returnStatus = NULL;// FAILS
	resp->arrayOfPermissions = NULL;
	_param_18->srmCheckPermissionResponse = resp;

	return 0; // success
}

int  soap_call_srm2__srmCheckPermission_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	struct srm2__srmCheckPermissionResponse *resp  = (struct srm2__srmCheckPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmCheckPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfPermissions = NULL;
	_param_18->srmCheckPermissionResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmCheckPermission_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	struct srm2__srmCheckPermissionResponse *resp  = (struct srm2__srmCheckPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmCheckPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	enum srm2__TPermissionMode *cur_permission = (enum srm2__TPermissionMode *) soap_malloc (soap,sizeof (enum srm2__TPermissionMode));;
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmCheckPermissionResponse = resp;
	*cur_permission = RWX;

	resp->arrayOfPermissions = (struct srm2__ArrayOfTSURLPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLPermissionReturn));
	resp->arrayOfPermissions->__sizesurlPermissionArray = 1;
	resp->arrayOfPermissions->surlPermissionArray = (struct srm2__TSURLPermissionReturn**) soap_malloc (soap,sizeof (struct srm2__TSURLPermissionReturn *));
	resp->arrayOfPermissions->surlPermissionArray[0] = (struct srm2__TSURLPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__TSURLPermissionReturn));
	resp->arrayOfPermissions->surlPermissionArray[0]->surl = fixture_test_string;
	resp->arrayOfPermissions->surlPermissionArray[0]->permission = cur_permission;
	resp->arrayOfPermissions->surlPermissionArray[0]->status = retstatus;


	return 0; // success
}
int  soap_call_srm2__srmCheckPermission_test6(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmCheckPermissionRequest *srmCheckPermissionRequest,
						struct srm2__srmCheckPermissionResponse_ *_param_18)
{
	struct srm2__srmCheckPermissionResponse *resp  = (struct srm2__srmCheckPermissionResponse *) soap_malloc (soap,sizeof (struct srm2__srmCheckPermissionResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	enum srm2__TPermissionMode *cur_permission = (enum srm2__TPermissionMode *) soap_malloc (soap,sizeof (enum srm2__TPermissionMode));;
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmCheckPermissionResponse = resp;
	*cur_permission = WX;

	resp->arrayOfPermissions = (struct srm2__ArrayOfTSURLPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLPermissionReturn));
	resp->arrayOfPermissions->__sizesurlPermissionArray = 1;
	resp->arrayOfPermissions->surlPermissionArray = (struct srm2__TSURLPermissionReturn**) soap_malloc (soap,sizeof (struct srm2__TSURLPermissionReturn *));
	resp->arrayOfPermissions->surlPermissionArray[0] = (struct srm2__TSURLPermissionReturn*) soap_malloc (soap,sizeof (struct srm2__TSURLPermissionReturn));
	resp->arrayOfPermissions->surlPermissionArray[0]->surl = fixture_test_string;
	resp->arrayOfPermissions->surlPermissionArray[0]->permission = cur_permission;
	resp->arrayOfPermissions->surlPermissionArray[0]->status = retstatus;


	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_check_permission
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_check_permission)
{
	int i;
	struct srm_checkpermission_input input;
	struct srmv2_filestatus *status;
	struct srm_context context;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	context.version = VERSION_2_2;

	input.amode = R_OK;
	input.nbfiles = 1;
	input.surls = test_surls;

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test1;
	result = srmv2_check_permission(&context,&input,&status); //failure empty fs
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test2;
	result = srmv2_check_permission(&context,&input,&status);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test3;
	result = srmv2_check_permission(&context,&input,&status);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test4;
	result = srmv2_check_permission(&context,&input,&status);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test5;
	result = srmv2_check_permission(&context,&input,&status);
	fail_if ((result  != 1),
					"Expected Success!");
	fail_if ((status[0].status != 0),
						"Expected Status 0!");

	call_function.call_srm2__srmCheckPermission = soap_call_srm2__srmCheckPermission_test6;
	result = srmv2_check_permission(&context,&input,&status);
	fail_if ((result  != 1),
					"Expected Success!");
	fail_if ((status[0].status == 0),
						"Expected Status EACCESS!");

}
END_TEST

int  soap_call_srm2__srmExtendFileLifeTime_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmExtendFileLifeTimeRequest *srmExtendFileLifeTimeRequest,
						struct srm2__srmExtendFileLifeTimeResponse_ *_param_18)
{
	struct srm2__srmExtendFileLifeTimeResponse *resp  = (struct srm2__srmExtendFileLifeTimeResponse *) soap_malloc (soap,sizeof (struct srm2__srmExtendFileLifeTimeResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmExtendFileLifeTimeResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmExtendFileLifeTime_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmExtendFileLifeTimeRequest *srmExtendFileLifeTimeRequest,
						struct srm2__srmExtendFileLifeTimeResponse_ *_param_18)
{
	//struct srm2__srmExtendFileLifeTimeResponse *resp  = (struct srm2__srmExtendFileLifeTimeResponse *) soap_malloc (soap,sizeof (struct srm2__srmExtendFileLifeTimeResponse));
	_param_18->srmExtendFileLifeTimeResponse = NULL; // FAILS
	//resp->arrayOfFileStatuses = NULL;
	//resp->returnStatus = NULL;
	return 0; // success
}
int  soap_call_srm2__srmExtendFileLifeTime_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmExtendFileLifeTimeRequest *srmExtendFileLifeTimeRequest,
						struct srm2__srmExtendFileLifeTimeResponse_ *_param_18)
{
	struct srm2__srmExtendFileLifeTimeResponse *resp  = (struct srm2__srmExtendFileLifeTimeResponse *) soap_malloc (soap,sizeof (struct srm2__srmExtendFileLifeTimeResponse));
	resp->returnStatus = NULL;// FAILS
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmExtendFileLifeTimeResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmExtendFileLifeTime_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmExtendFileLifeTimeRequest *srmExtendFileLifeTimeRequest,
						struct srm2__srmExtendFileLifeTimeResponse_ *_param_18)
{
	struct srm2__srmExtendFileLifeTimeResponse *resp  = (struct srm2__srmExtendFileLifeTimeResponse *) soap_malloc (soap,sizeof (struct srm2__srmExtendFileLifeTimeResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREFAILURE;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL;
	_param_18->srmExtendFileLifeTimeResponse = resp;
	return 0; // success
}

int  soap_call_srm2__srmExtendFileLifeTime_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmExtendFileLifeTimeRequest *srmExtendFileLifeTimeRequest,
						struct srm2__srmExtendFileLifeTimeResponse_ *_param_18)
{
	struct srm2__srmExtendFileLifeTimeResponse *resp  = (struct srm2__srmExtendFileLifeTimeResponse *) soap_malloc (soap,sizeof (struct srm2__srmExtendFileLifeTimeResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	int *pinlifetime = (int *) soap_malloc (soap,sizeof (int));
	retstatus->statusCode = SRM_USCORESUCCESS;  // FAILS
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmExtendFileLifeTimeResponse = resp;
	*pinlifetime = 100;

	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTSURLLifetimeReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLLifetimeReturnStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__TSURLLifetimeReturnStatus**) soap_malloc (soap,sizeof (struct srm2__TSURLLifetimeReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__TSURLLifetimeReturnStatus*) soap_malloc (soap,sizeof (struct srm2__TSURLLifetimeReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->surl = fixture_test_string;
	resp->arrayOfFileStatuses->statusArray[0]->fileLifetime = pinlifetime;
	resp->arrayOfFileStatuses->statusArray[0]->pinLifetime = pinlifetime;
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;

	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_extend_file_lifetime
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_extend_file_lifetime)
{
	int i;
	struct srm_extendfilelifetime_input input;
	struct srm_extendfilelifetime_output output;
	struct srmv2_pinfilestatus *status;
	struct srm_context context;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function
    srm_context_init(&context, "test", NULL, 0, 0);
	context.version = VERSION_2_2;

	input.pintime = 100;
	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";


	call_function.call_srm2__srmExtendFileLifeTime = soap_call_srm2__srmExtendFileLifeTime_test1;
	result = srmv2_extend_file_lifetime(&context,&input,&output); //failure empty fs
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmExtendFileLifeTime = soap_call_srm2__srmExtendFileLifeTime_test2;
	result = srmv2_extend_file_lifetime(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmExtendFileLifeTime = soap_call_srm2__srmExtendFileLifeTime_test3;
	result = srmv2_extend_file_lifetime(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmExtendFileLifeTime = soap_call_srm2__srmExtendFileLifeTime_test4;
	result = srmv2_extend_file_lifetime(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != -1),
					"Expected Failure !");

	call_function.call_srm2__srmExtendFileLifeTime = soap_call_srm2__srmExtendFileLifeTime_test5;
	result = srmv2_extend_file_lifetime(&context,&input,&output);
	srm_srm2__TReturnStatus_delete(output.retstatus);
	fail_if ((result  != 1),
					"Expected Success!");

}
END_TEST

START_TEST (test_srm_util_add_strings)
{
    const char* s1 = "s1";
    const char* s2 = "s2";
    char* res = NULL;

    res = srm_util_add_strings(s1, s2);
    fail_unless(strcmp("s1s2", res) == 0, "Expected equality");
    free(res);
    res = NULL;

    res = srm_util_add_strings("", s2);
    fail_unless(strcmp(s2, res) == 0, "Expected equality");
    free(res);
    res = NULL;

    res = srm_util_add_strings(s1, "");
    fail_unless(strcmp(s1, res) == 0, "Expected equality");
    free(res);
    res = NULL;

    res = srm_util_add_strings("", "");
    fail_unless(strcmp("", res) == 0, "Expected equality");
    free(res);
    res = NULL;
}
END_TEST


START_TEST (test_srm_util_consolidate_multiple_characters)
{
    char* res = NULL;

    fail_unless(NULL == srm_util_consolidate_multiple_characters(NULL, 'x', 0));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("abcd", 'a', 0);
    fail_unless(0 == strcmp("abcd", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("abcd", 'x', 0);
    fail_unless(0 == strcmp("abcd", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("abcd", '0', 0);
    fail_unless(0 == strcmp("abcd", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("", '0', 0); 
    fail_unless(0 == strcmp("", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("", 'a', 0); 
    fail_unless(0 == strcmp("", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("a", 'a', 0); 
    fail_unless(0 == strcmp("a", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aa", 'a', 0); 
    fail_unless(0 == strcmp("a", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaa", 'a', 0); 
    fail_unless(0 == strcmp("a", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaa", 'x', 0); 
    fail_unless(0 == strcmp("aaa", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("xaaa", 'a', 0); 
    fail_unless(0 == strcmp("xa", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("xaaay", 'a', 0); 
    fail_unless(0 == strcmp("xay", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaay", 'a', 0); 
    fail_unless(0 == strcmp("ay", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaaxaaay", 'a', 3); 
    fail_unless(0 == strcmp("aaaxay", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaaxaaay", 'a', 1); 
    fail_unless(0 == strcmp("aaxay", res));
    free(res);
    res = NULL;

    res = srm_util_consolidate_multiple_characters("aaaxaaay", 'a', 100); 
    fail_unless(0 == strcmp("aaaxaaay", res));
    free(res);
    res = NULL;
}
END_TEST


START_TEST (test_srm_util_normalize_surl)
{
    char* surl_1 = "srm://server:port/";
    char* surl_2 = "srm://server:port/dir1";
    char* surl_3 = "srm://server:port/dir1/";
    char* surl_4 = "srm://server:port/dir1//";
    char* surl_5 = "srm://server:port/dir1/dir2/";
    char* surl_6 = "srm://server:port/dir1/dir2";
    char* surl_7 = "srm://server:port/dir1//dir2/";
    char* surl_8 = "srm://server:port/dir1//dir2//";
    char* res = NULL;

    res = srm_util_normalize_surl(surl_1);
    fail_unless(strcmp(surl_1, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_2);
    fail_unless(strcmp(surl_3, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_3);
    fail_unless(strcmp(surl_3, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_4);
    fail_unless(strcmp(surl_3, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_5);
    fail_unless(strcmp(surl_5, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_6);
    fail_unless(strcmp(surl_5, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_7);
    fail_unless(strcmp(surl_5, res) == 0);   
    free(res);
    res = NULL;

    res = srm_util_normalize_surl(surl_8);
    fail_unless(strcmp(surl_5, res) == 0);   
    free(res);
    res = NULL;
}
END_TEST


START_TEST (test_srmv2_check_srm_root)
{
    const char* good_url = "srm://server.cern.ch/";
    const char* bad_url_1 = "srm://server.cern.ch/file";
    const char* bad_url_2 = "fake://server.cern.ch/file";
    const char* bad_url_3 = "srm:/server.cern.ch/file";
    const char* bad_url_4 = "srm//server.cern.ch/file";
    const char* bad_url_5 = "srm://server.cern.ch";

    fail_unless(1 == srmv2_check_srm_root(good_url));
    fail_unless(0 == srmv2_check_srm_root(NULL));
    fail_unless(0 == srmv2_check_srm_root(bad_url_1));
    fail_unless(0 == srmv2_check_srm_root(bad_url_2));
    fail_unless(0 == srmv2_check_srm_root(bad_url_3));
    fail_unless(0 == srmv2_check_srm_root(bad_url_4));
    fail_unless(0 == srmv2_check_srm_root(bad_url_5));

    return NULL;
}
END_TEST


Suite * test_suite (void)
{
    Suite *s = suite_create ("new srm interface unit test suit");

    gfal_unittest_srmls_test_suite(s);

    TCase *tc_case_1 = tcase_create ("srm function tests");

    tcase_add_checked_fixture (tc_case_1, NULL,NULL);
    tcase_add_test (tc_case_1, test_srmv2_extend_file_lifetime);
    tcase_add_test (tc_case_1, test_srmv2_set_permission);
    tcase_add_test (tc_case_1, test_srmv2_get_permission);
    tcase_add_test (tc_case_1, test_srmv2_check_permission);
    tcase_add_test (tc_case_1, test_wait_for_new_attempt);
    tcase_add_test (tc_case_1, test_back_off_logic);
    tcase_add_test (tc_case_1, test_srmv2_abort_request);
    tcase_add_test (tc_case_1, test_srmv2_rmdir);
    tcase_add_test (tc_case_1, test_srmv2_rm);
    tcase_add_test (tc_case_1, test_srmv2_mkdir);
    tcase_add_test (tc_case_1, test_srmv2_abort_files);
    tcase_add_test (tc_case_1, test_srmv2_put_done);
    tcase_add_test (tc_case_1, test_srmv2_release_files);
    tcase_add_test (tc_case_1, test_srmv2_bring_online_async);
    tcase_add_test (tc_case_1, test_srmv2_status_of_bring_online_async);
    tcase_add_test (tc_case_1, test_srmv2_prepare_to_get_async);
    tcase_add_test (tc_case_1, test_srmv2_status_of_get_request_async);
    tcase_add_test (tc_case_1, test_srmv2_prepare_to_put_async);
    tcase_add_test (tc_case_1, test_srmv2_status_of_put_request_async);
    tcase_add_test (tc_case_1, test_srmv2_getspacemd);
    tcase_add_test (tc_case_1, test_srmv2_getspacetokens);
    tcase_add_test (tc_case_1, test_srmv2_ping);

    suite_add_tcase (s, tc_case_1);

    TCase *tc_case_2 = tcase_create ("utility function tests");
    tcase_add_checked_fixture (tc_case_2, NULL,NULL);
    tcase_add_test (tc_case_2, test_srm_util_add_strings);
    tcase_add_test (tc_case_2, test_srm_util_normalize_surl);
    tcase_add_test (tc_case_2, test_srm_util_consolidate_multiple_characters);
    tcase_add_test (tc_case_2, test_srmv2_check_srm_root);

    suite_add_tcase (s, tc_case_2);

    return s;
}

int main(void)
{
	int number_failed;
	int i;

	Suite *s = test_suite ();
	SRunner *sr = srunner_create (s);
    srunner_set_fork_status (sr, CK_NOFORK); // for easier debug
	srunner_run_all (sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


