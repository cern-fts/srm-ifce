#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <check.h>
#include <errno.h>
#include "srmv2H.h"
#include "srm_types.h"
#include "srm_ifce.h"
#include "srm_util.h"
#include "srm_soap.h"
#include "srm_dependencies.h"


const char test_string[] = "test_string";
const char* test_strings[3]= { "test_string1","test_string2",NULL};
void PrintResult(struct srmv2_mdfilestatus* output);



//////////////////////////////////////////////////////////////////
// test wait_for_new_attempt function
//////////////////////////////////////////////////////////////////
unsigned int mock_sleep_time = 0;
unsigned int mock_sleep(unsigned int time)
{
	mock_sleep_time = time;
	return 0;
}

START_TEST (test_wait_for_new_attempt)
{
	struct srm_internal_context internal_context;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 11;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Wait for new attempt does not return timeout error for 11 attempts!\n");

	internal_context.attempt = 5;
	internal_context.end_time = time(NULL)-1;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Timeout error not received!\n");

	call_function.call_sleep = mock_sleep;
	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	fail_if (wait_for_new_attempt(&internal_context) != 0,
		   "Timeout should not occur!\n");
	fail_if (mock_sleep_time > 1, // be careful changing this number
		  "Random sleep time exceeded expected value !!!\n");


	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	internal_context.estimated_wait_time = 0;
	fail_if (wait_for_new_attempt(&internal_context) != -1,
		   "Timeout should occur!\n");


	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+100;
	internal_context.estimated_wait_time = -10;
	fail_if (mock_sleep_time > 1, // be careful changing this number
			  "Random sleep time exceeded expected value !!!\n");

	internal_context.estimated_wait_time = 10;
	wait_for_new_attempt(&internal_context);
	fail_if (mock_sleep_time != 10,
			  "Sleep time estimated wait time not equal to sleep time!\n");
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
			   "if internal_context->retstatus is null the function must return FAILURE!\n");

	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);
	fail_if ((internal_context.attempt   != 2),
				   "Wait new attempt!\n");

	fail_if ((result  != srm_call_status_INTERNAL_ERROR),
				   "Expected Internal Error!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 1!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 2!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 3!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 1!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 2!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCORESUCCESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_SUCCESS),
					   "Expected Success!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREFAILURE;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	fail_if ((result  != srm_call_status_FAILURE),
					   "Expected Failure!\n");
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
	_param_18->srmLsResponse = resp;
	_param_18->srmLsResponse->details = NULL;

	return -1; // failure
}

int  soap_call_srm2__srmLs_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmLsResponse = resp;
	_param_18->srmLsResponse->details = NULL;

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
	resp->requestToken = test_string;
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
	resp->details->pathDetailArray = (struct srm2__ArrayOfTMetaDataPathDetail**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail *));
	resp->details->pathDetailArray[0] = (struct srm2__ArrayOfTMetaDataPathDetail*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail));
	resp->details->pathDetailArray[0] = NULL;
	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_ls_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_ls_async)
{
	struct srm_ls_output output;
	struct srm_mdfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
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

	input.nbfiles = 1;
	input.count = 0;
	input.numlevels  = 1;
	input.surls = test_surls_ls;
	input.offset = 0;

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test1;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!\n");

	internal_context.attempt = 1;
	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test2;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test3;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test4;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test5;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test6;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test7;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!\n");
}
END_TEST
int  soap_call_srm2__srmStatusOfLs_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfLsRequestResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmStatusOfLs_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfLsRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfLs_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfLsRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfLs_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfLsRequestResponse *resp  = (struct srm2__srmStatusOfLsRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfLsRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->details = (struct srm2__ArrayOfTMetaDataPathDetail*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail));
	resp->details->__sizepathDetailArray = 1;
	resp->details->pathDetailArray = (struct srm2__ArrayOfTMetaDataPathDetail**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail *));
	resp->details->pathDetailArray[0] = (struct srm2__ArrayOfTMetaDataPathDetail*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataPathDetail));
	resp->details->pathDetailArray[0] = NULL;
	_param_18->srmStatusOfLsRequestResponse = resp;


	return 0; // success
}
//////////////////////////////////////////////////////////////////
// test test_srmv2_Rm_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_status_of_ls_request)
{
	int i;
	struct srm_mdfilestatus *filestatus;
	struct srm_ls_input input;
	struct srm_ls_output output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	int result;

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
				    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test2;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls!\n");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test3;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	//output.statuses = &filestatus;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test4;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!\n");


}
END_TEST
srm_call_status soap_call_srm2__abort_request_test1(struct srm_context *context,
		struct srm_internal_context *internal_context)
{
	return 0;
}
srm_call_status soap_call_srm2__abort_request_test2(struct srm_context *context,
		struct srm_internal_context *internal_context)
{
	return -1;
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
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test1;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != 0),
				   "Expected Success!\n");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test2;
	result = srmv2_abort_request(&context,token);
	fail_if ((result  != -1),
				   "Expected Failure 2!\n");
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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.surl = test_string;
	input.recursive = 0;

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test1;
	result = srmv2_rmdir(&context,&input,&output);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test2;
	result = srmv2_rmdir(&context,&input,&output);
	fail_if ((result  != -1),
				   "Expected Failure 2!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test3;
	result = srmv2_rmdir(&context,&input,&output);
	fail_if ((result  != 0),
				   "Expected Success!\n");
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;

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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.surls = test_strings;
	input.nbfiles = 1;

	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test1;
	result = srmv2_rm(&context,&input,&output);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test2;
	result = srmv2_rm(&context,&input,&output);
	fail_if ((result  != 0),
				   "Expected Success!\n");
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
//////////////////////////////////////////////////////////////////
// test test_srmv2_rm
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_mkdir)
{
	struct srm_mkdir_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.dir_name = "/dpm/cern.ch/home/dteam/1/2/3/4";

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test1;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test2;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != 0),
				   "Expected Success!\n");
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;

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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test1;
	result = srmv2_abort_files(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test2;
	result = srmv2_abort_files(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!\n");
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;


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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;


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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test1;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test2;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test3;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test4;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test5;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;


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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTSURLReturnStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTSURLReturnStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTSURLReturnStatus));
	resp->arrayOfFileStatuses->statusArray[0]->status = retstatus;
	resp->arrayOfFileStatuses->statusArray[0]->surl = test_string;


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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test1;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test2;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!\n");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test3;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test4;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");

	call_function.call_srm2__srmReleaseFiles = soap_call_srm2__srmReleaseFiles_test5;
	result = srmv2_release_files(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");
}
END_TEST
int  soap_call_srm2__srmBringOnline_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	_param_18->srmBringOnlineResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmBringOnline_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmBringOnlineResponse = resp;
	retstatus->statusCode = SRM_USCOREFAILURE;

	return -1; // failure
}

int  soap_call_srm2__srmBringOnline_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmBringOnlineRequest *srmBringOnlineRequest, struct srm2__srmBringOnlineResponse_ *_param_18)
{
	struct srm2__srmBringOnlineResponse *resp  = (struct srm2__srmBringOnlineResponse *) soap_malloc (soap,sizeof (struct srm2__srmBringOnlineResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmBringOnlineResponse = resp;
	_param_18->srmBringOnlineResponse->arrayOfFileStatuses = NULL;
	_param_18->srmBringOnlineResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;

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
	resp->requestToken = test_string;
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTBringOnlineRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus));
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test1;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test2;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test3;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test4;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test5;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test6;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test7;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!\n");
}
END_TEST
int  soap_call_srm2__srmStatusOfBringOnline_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfBringOnlineRequestResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmStatusOfBringOnline_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfBringOnline_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfBringOnline_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequest *srmStatusOfBringOnlineRequest,
						struct srm2__srmStatusOfBringOnlineRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfBringOnlineRequestResponse *resp  = (struct srm2__srmStatusOfBringOnlineRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfBringOnlineRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTBringOnlineRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTBringOnlineRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTBringOnlineRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfBringOnlineRequestResponse = resp;
	_param_18->srmStatusOfBringOnlineRequestResponse->remainingTotalRequestTime = NULL;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfBringOnline_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfBringOnlineRequest *srmStatusOfBringOnlineRequest,
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test1;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test2;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test3;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= &filestatus;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test4;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!\n");


	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test5;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!\n");
}
END_TEST

int  soap_call_srm2__srmPrepareToGet_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	_param_18->srmPrepareToGetResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmPrepareToGet_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmBringOnlineRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREFAILURE;

	return -1; // failure
}

int  soap_call_srm2__srmPrepareToGet_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToGetResponse = resp;
	_param_18->srmPrepareToGetResponse->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToGetResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;

	return 0; // success
}

int  soap_call_srm2__srmPrepareToGet_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToGetRequest *srmPrepareToGetRequest, struct srm2__srmPrepareToGetResponse_ *_param_18)
{
	struct srm2__srmPrepareToGetResponse *resp  = (struct srm2__srmPrepareToGetResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToGetResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
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
	resp->requestToken = test_string;
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTGetRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus));
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test1;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test2;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test3;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test4;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test5;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test6;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test7;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!\n");
}
END_TEST
int  soap_call_srm2__srmStatusOfGet_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfGetRequestResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmStatusOfGet_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfGet_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfGet_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequest *srmStatusOfGetRequest,
						struct srm2__srmStatusOfGetRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfGetRequestResponse *resp  = (struct srm2__srmStatusOfGetRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfGetRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTGetRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTGetRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTGetRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfGetRequestResponse = resp;
	_param_18->srmStatusOfGetRequestResponse->remainingTotalRequestTime = NULL;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfGet_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfGetRequest *srmStatusOfGetRequest,
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test1;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test2;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test3;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= &filestatus;
	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test4;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!\n");


	call_function.call_srm2__srmStatusOfGetRequest = soap_call_srm2__srmStatusOfGet_test5;
	result = srmv2_status_of_get_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!\n");
}
END_TEST
int  soap_call_srm2__srmPrepareToPut_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	_param_18->srmPrepareToPutResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmPrepareToPut_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmBringOnlineRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREFAILURE;

	return -1; // failure
}

int  soap_call_srm2__srmPrepareToPut_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmPrepareToPutResponse = resp;
	_param_18->srmPrepareToPutResponse->arrayOfFileStatuses = NULL;
	_param_18->srmPrepareToPutResponse->remainingTotalRequestTime = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;

	return 0; // success
}

int  soap_call_srm2__srmPrepareToPut_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->requestToken = NULL;
	_param_18->srmPrepareToPutResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmPrepareToPut_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmPrepareToPutRequest *srmPrepareToPutRequest, struct srm2__srmPrepareToPutResponse_ *_param_18)
{
	struct srm2__srmPrepareToPutResponse *resp  = (struct srm2__srmPrepareToPutResponse *) soap_malloc (soap,sizeof (struct srm2__srmPrepareToPutResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
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
	resp->requestToken = test_string;
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
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTPutRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus));
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
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test1;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test2;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 2!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test3;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 3!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test4;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
					"Expected Failure 4!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test5;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
					"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test6;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
					"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test7;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!\n");
}
END_TEST
int  soap_call_srm2__srmStatusOfPut_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	_param_18->srmStatusOfPutRequestResponse = NULL;
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmStatusOfPut_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfPutRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfPut_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	_param_18->srmStatusOfPutRequestResponse = resp;

	return 0; // success
}
int  soap_call_srm2__srmStatusOfPut_test4(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = (struct srm2__ArrayOfTPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus));
	resp->arrayOfFileStatuses->__sizestatusArray = 1;
	resp->arrayOfFileStatuses->statusArray = (struct srm2__ArrayOfTPutRequestFileStatus**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus *));
	resp->arrayOfFileStatuses->statusArray[0] = (struct srm2__ArrayOfTPutRequestFileStatus*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTPutRequestFileStatus));
	resp->arrayOfFileStatuses->statusArray[0] = NULL;
	_param_18->srmStatusOfPutRequestResponse = resp;


	return 0; // success
}

int  soap_call_srm2__srmStatusOfPut_test5(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfPutRequest *srmStatusOfPutRequest,
						struct srm2__srmStatusOfPutRequestResponse_ *_param_18)
{
	struct srm2__srmStatusOfPutRequestResponse *resp  = (struct srm2__srmStatusOfPutRequestResponse *) soap_malloc (soap,sizeof (struct srm2__srmStatusOfPutRequestResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	retstatus->statusCode = SRM_USCORESUCCESS;
	retstatus->explanation = NULL;
	resp->returnStatus = retstatus;
	resp->arrayOfFileStatuses = NULL; // FAILS
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... putbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test1;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test2;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
				   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
	}
	fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test3;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= &filestatus;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test4;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
				   "Expected Success!\n");


	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test5;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
				   "Expected Failure!\n");
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
	resp->arrayOfSpaceDetails->spaceDataArray = (struct srm2__ArrayOfTMetaDataSpace**) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataSpace *));
	resp->arrayOfSpaceDetails->spaceDataArray[0] = (struct srm2__ArrayOfTMetaDataSpace*) soap_malloc (soap,sizeof (struct srm2__ArrayOfTMetaDataSpace));
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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbtokens = 0;
	input.spacetokens = NULL;

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
				    "Expected Failure !\n");

	input.nbtokens = 1;
	input.spacetokens = NULL;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
				    "Expected Failure !\n");

	input.nbtokens = 1; // FAILS wrong count
	input.spacetokens = test_tokens2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	input.nbtokens = 2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test2;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test3;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test4;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != -1),
					"Expected Failure !\n");


	input.nbtokens = 1;
	input.spacetokens = test_tokens1;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test5;
	result = srmv2_getspacemd(&context,&input,&spaces);
	fail_if ((result  != 0),
					"Expected Success!\n");
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
	resp->arrayOfSpaceTokens->stringArray = (struct srm2__ArrayOfString**) soap_malloc (soap,sizeof (struct srm2__ArrayOfString *));
	resp->arrayOfSpaceTokens->stringArray[0] = (struct srm2__ArrayOfString*) soap_malloc (soap,sizeof (struct srm2__ArrayOfString));
	resp->arrayOfSpaceTokens->stringArray[0] = test_string;
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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";


	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	input.spacetokendesc = "MySpaceTokens";

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test2;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test3;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test4;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test5;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != 0),
					"Expected Success!\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test6;
	result = srmv2_getspacetokens(&context,&input,&output);
	fail_if ((result  != -1),
					"Expected Failure !\n");
	fail_if ((strcmp(output.spacetokens[0],test_string)  != 0),
						"Expected the same string !\n");

}
END_TEST

Suite * test_suite (void)
{
  Suite *s = suite_create ("New srm interface unit test suit");

  TCase *tc_case_1 = tcase_create ("T1");

  tcase_add_checked_fixture (tc_case_1, NULL,NULL);
  tcase_add_test (tc_case_1, test_wait_for_new_attempt);
  tcase_add_test (tc_case_1, test_back_off_logic);
  tcase_add_test (tc_case_1, test_srmv2_ls_async);
  tcase_add_test (tc_case_1, test_srmv2_status_of_ls_request);
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

  suite_add_tcase (s, tc_case_1);

  return s;
}

int main(void)
{
	int number_failed;
	int i;

	Suite *s = test_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

//	TestGet();
//	TestSpaceTokens();
//	TestSpaceMD();
//	TestStatusOfPut();
//	TestPut();
//	TestStatusOfBringOnline();
//	TestBringOnline();
//	TestPutDone();
//	TestAbortFiles();
//	TestMkdir();
//	TestRm();
//	TestStatusOfLs();
//////	TestIt();
//	TestBackoff();
//	TestLs();
//	TestAbortRequest();
//	TestRmdir();


	//printf("TEST\n");
	return EXIT_SUCCESS;
}
void PrintResult(struct srmv2_mdfilestatus* output)
{
	int i;
	printf("Directory: %s \n",output->surl);
	printf("Files:\n");
	for(i=0;i<output->nbsubpaths;i++)
	{
		printf("%s \n",output->subpaths[i].surl);
	}
}

void TestSpaceTokens()
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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";


	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");

	input.spacetokendesc = "MySpaceTokens";

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test1;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test2;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test3;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test4;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test5;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != 0),
		//			"Expected Success!\n");

	call_function.call_srm2__srmGetSpaceTokens = soap_call_srm2__srmGetSpaceTokens_test6;
	result = srmv2_getspacetokens(&context,&input,&output);
	//fail_if ((result  != -1),
		//			"Expected Failure !\n");
}
void TestLs()
{
	struct srm_ls_output output;
	struct srm_mdfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
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

	input.nbfiles = 1;
	input.count = 0;
	input.numlevels  = 1;
	input.surls = test_surls_ls;
	input.offset = 0;

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test1;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
		//		   "Expected Failure 1!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test2;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
		//			"Expected Failure 2!\n");
}
void TestStatusOfLs()
{
	struct srm_mdfilestatus *filestatus;
	struct srm_ls_input input;
	struct srm_ls_output output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	srm_call_status result;

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
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
	//				"Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test2;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
	//			   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test3;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
	//			   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.statuses = &filestatus;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test4;
	result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
	//			   "Expected Success!\n");
}
void TestAbortRequest()
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
	//fail_if ((result  != srm_call_status_FAILURE),
		//		   "Expected Failure 1!\n");

	// TODO TODO TODO internal_context.token = test_string;
	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test1;
	result = srmv2_abort_request(&context,token);
	//fail_if ((result  != srm_call_status_SUCCESS),
		//		   "Expected Success!\n");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test2;
	result = srmv2_abort_request(&context,token);
	//fail_if ((result  != srm_call_status_FAILURE),
		//		   "Expected Failure 2!\n");
}
void TestIt()
{
	struct srm_rmdir_input input;
	struct srm_rmdir_output* output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm2__TReturnStatus retstatus;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 1;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.surl = test_string;
	input.recursive = 1;


	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test3;
	result = srmv2_rmdir(&context,&input,&output);
}
void TestRmdir()
{
	struct srmv2_filestatus *filestatus;
	struct srm_rmdir_input input;
	struct srm_rmdir_output output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm2__TReturnStatus retstatus;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.surl = test_string;
	input.recursive = 0;

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test1;
	result = srmv2_rmdir(&context,&input,&output);
	//fail_if ((result  != -1),
		//		   "Expected Failure 1!\n");

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test2;
	result = srmv2_rmdir(&context,&input,&output);
//	fail_if ((result  != -1),
	//			   "Expected Failure 2!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test3;
	result = srmv2_rmdir(&context,&input,&output);
	//fail_if ((result  != 0),
		//		   "Expected Success!\n");
}
void TestMkdir()
{
	struct srm_mkdir_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.dir_name = "/dpm/cern.ch/home/dteam/1/2/3/4";

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test1;
	result = srmv2_mkdir(&context,&input);
//	fail_if ((result  != -1),
//				   "Expected Failure 1!\n");

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test2;
	result = srmv2_mkdir(&context,&input);
//	fail_if ((result  != 0),
//				   "Expected Success!\n");
}
void TestAbortFiles()
{
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	struct srmv2_filestatus *statuses;
	struct srm_abort_files_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test1;
	result = srmv2_abort_files(&context,&input,&statuses);
	//fail_if ((result  != -1),
				   //"Expected Failure 1!\n");

	call_function.call_srm2__srmAbortFiles = soap_call_srm2__srmAbortFiles_test2;
	result = srmv2_abort_files(&context,&input,&statuses);
	//fail_if ((result == -1),
		//		   "Expected Success!\n");
}
void TestPutDone()
{
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	struct srmv2_filestatus *statuses;
	struct srm_putdone_input input;
	struct srm_context context;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.surls = test_surls;
	input.reqtoken = "test";

	/*call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test1;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test2;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result == -1),
				   "Expected Success!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test3;
	result = srmv2_put_done(&context,&input,&statuses);
	fail_if ((result != -1),
				   "Expected Failure!\n");*/

	/*call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test4;
	result = srmv2_put_done(&context,&input,&statuses);*/
	//fail_if ((result != -1),
		//		   "Expected Failure!\n");

	call_function.call_srm2__srmPutDone = soap_call_srm2__srmPutDone_test5;
	result = srmv2_put_done(&context,&input,&statuses);
//	fail_if ((result != -1),
	//			   "Expected Failure!\n");
}
void TestBringOnline()
{
	struct srm_bringonline_input input;
	struct srm_bringonline_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test1;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//			   "Expected Failure 1!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test2;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 2!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test3;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 3!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test4;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 4!\n");

	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test5;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
	//				"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test6;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
	//				"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmBringOnline = soap_call_srm2__srmBringOnline_test7;
	result = srmv2_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
	//				"Expected Success!\n");*/
}
void TestRm()
{
	struct srmv2_filestatus *filestatus;
	struct srm_rm_input input;
	struct srm_rm_output output;
	struct srm_context context;

	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.surls = test_strings;
	input.nbfiles = 1;

	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test1;
	result = srmv2_rm(&context,&input,&output);
	//fail_if ((result  != -1),
		//		   "Expected Failure 1!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test2;
	result = srmv2_rm(&context,&input,&output);
	//fail_if ((result  != 0),
				   //"Expected Success!\n");
}

void TestPut()
{
	struct srm_preparetoput_input input;
	struct srm_preparetoput_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test1;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//			   "Expected Failure 1!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test2;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 2!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test3;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 3!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test4;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 4!\n");

	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test5;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
	//				"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test6;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
	//				"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmPrepareToPut = soap_call_srm2__srmPrepareToPut_test7;
	result = srmv2_prepare_to_put_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
	//				"Expected Success!\n");

}
void TestStatusOfPut()
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... putbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test1;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
		//		    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test2;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
		//		   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
		//fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
			//		   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
	}
	//fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
		//		   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test3;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
		//		   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= &filestatus;
	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test4;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
		//		   "Expected Success!\n");


	call_function.call_srm2__srmStatusOfPutRequest = soap_call_srm2__srmStatusOfPut_test5;
	result = srmv2_status_of_put_request_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
		//		   "Expected Failure!\n");
}
void TestSpaceMD()
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

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbtokens = 0;
	input.spacetokens = NULL;

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//		    "Expected Failure 1!\n");

	input.nbtokens = 1;
	input.spacetokens = NULL;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//		    "Expected Failure 1!\n");

	input.nbtokens = 1; // FAILS wrong count
	input.spacetokens = test_tokens2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//			"Expected Failure 1!\n");

	input.nbtokens = 2;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test1;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//			"Expected Failure 1!\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test2;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//			"Expected Failure 1!\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test3;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//			"Expected Failure 1!\n");

	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test4;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != -1),
		//			"Expected Failure 1!\n");


	input.nbtokens = 1;
	input.spacetokens = test_tokens1;
	call_function.call_srm2__srmGetSpaceMetaData = soap_call_srm2__srmGetSpaceMetaData_test5;
	result = srmv2_getspacemd(&context,&input,&spaces);
	//fail_if ((result  != 0),
		//			"Expected Success!\n");
}

void TestStatusOfBringOnline()
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
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test1;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE) || (result  != -1),
		//		    "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test2;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status   != srm_call_status_QUEUED)|| (result  == -1),
		//		   "Expected Queued in first call!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
		//fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
			//		   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
	}
	//fail_if ((internal_context.current_status   != srm_call_status_TIMEOUT) || (result  != -1),
				   //"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test3;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
//	fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT) || (result  != -1),
	//			   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	output.filestatuses= &filestatus;
	call_function.call_srm2__srmStatusOfBringOnlineRequest = soap_call_srm2__srmStatusOfBringOnline_test4;
	result = srmv2_status_of_bring_online_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_SUCCESS) || (result  == -1),
		//		   "Expected Success!\n");
}
void TestGet()
{
	struct srm_preparetoget_input input;
	struct srm_preparetoget_output output;
	struct srmv2_pinfilestatus *filestatus;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	char *test_surls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	char *test_protocols[] = {"protocol1","protocol2"};
	int result;

	internal_context.estimated_wait_time = -1;
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.nbfiles = 1;
	input.desiredpintime = 1000;
	input.spacetokendesc  = NULL;
	// TODO test ... getbestspacetoken input.spacetokendesc = "TEST_SPACE_TOKEN_DESC";
	input.surls = test_surls;
	input.protocols = test_protocols;

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test1;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//			   "Expected Failure 1!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test2;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 2!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test3;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 3!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test4;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_FAILURE)|| (result != -1),
	//				"Expected Failure 4!\n");

	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test5;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_TIMEOUT)|| (result != -1),
//					"Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test6;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_QUEUED)|| (result == -1),
		//			"Expected Queued!\n");


	output.filestatuses= &filestatus;
	call_function.call_srm2__srmPrepareToGet = soap_call_srm2__srmPrepareToGet_test7;
	result = srmv2_prepare_to_get_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
		//			"Expected Success!\n");
}
void TestBackoff()
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
	//fail_if ((result  != srm_call_status_FAILURE)||(errno != ECOMM),
		//	   "if internal_context->retstatus is null the function must return FAILURE!\n");

	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);
	//fail_if ((internal_context.attempt   != 2),
		//		   "Wait new attempt!\n");

	//fail_if ((result  != srm_call_status_INTERNAL_ERROR),
				   //"Expected Internal Error!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_TIMEOUT),
		//			   "Expected Timeout 1!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_TIMEOUT),
		//			   "Expected Timeout 2!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_TIMEOUT),
		//			   "Expected Timeout 3!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_QUEUED),
		//			   "Expected Queued 1!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	///fail_if ((result  != srm_call_status_QUEUED),
		//			   "Expected Queued 2!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCORESUCCESS;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_SUCCESS),
		//			   "Expected Success!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREFAILURE;
	result = back_off_logic(&context,srmfunc,&internal_context,&retstatus);

	//fail_if ((result  != srm_call_status_FAILURE),
		//			   "Expected Failure!\n");
}
