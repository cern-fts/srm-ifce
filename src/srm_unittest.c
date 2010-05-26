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

	call_function.call_sleep = mock_sleep; // set mock sleep function


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
	return 0; // success but return statuses are null
}

int  soap_call_srm2__srmLs_test2(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmLsResponse = resp;
	retstatus->statusCode = SRM_USCOREFAILURE;

	return -1; // failure
}

int  soap_call_srm2__srmLs_test3(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmLsRequest *srmLsRequest, struct srm2__srmLsResponse_ *_param_18)
{
	struct srm2__srmLsResponse *resp  = (struct srm2__srmLsResponse *) soap_malloc (soap,sizeof (struct srm2__srmLsResponse));
	struct srm2__TReturnStatus *retstatus = (struct srm2__TReturnStatus *) soap_malloc (soap,sizeof (struct srm2__TReturnStatus));
	resp->returnStatus = retstatus;
	_param_18->srmLsResponse = resp;
	_param_18->srmLsResponse->details = NULL;
	retstatus->statusCode = SRM_USCOREPARTIAL_USCORESUCCESS;

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


	output.statuses = &filestatus;
	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test7;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	fail_if ((internal_context.current_status  != srm_call_status_SUCCESS)|| (result == -1),
					"Expected Success!\n");
}
END_TEST
int  soap_call_srm2__srmStatusOfLs_test1(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct srm2__srmStatusOfLsRequest *srmStatusOfLsRequest,
						struct srm2__srmStatusOfLsRequestResponse_ *_param_18)
{
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
START_TEST (test_srmv2_StatusOfLsRequest)
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
				   "Expected Queued in first call.!\n");
	for (i=0;i<15;i++)
	{
		result = srmv2_status_of_ls_request_async_internal(&context,&input,&output,&internal_context);
		fail_if ((internal_context.current_status   == srm_call_status_SUCCESS) || (internal_context.current_status   == srm_call_status_FAILURE),
					   "Do not fail/succeed if queued,expected timeout after 10 calls.!\n");
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
	output.statuses = &filestatus;
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

Suite * test_suite (void)
{
  Suite *s = suite_create ("New srm interface unit test suit");

  TCase *tc_case_1 = tcase_create ("T1");

  tcase_add_checked_fixture (tc_case_1, NULL,NULL);
  tcase_add_test (tc_case_1, test_wait_for_new_attempt);
  tcase_add_test (tc_case_1, test_back_off_logic);
  tcase_add_test (tc_case_1, test_srmv2_ls_async);
  tcase_add_test (tc_case_1, test_srmv2_StatusOfLsRequest);
  tcase_add_test (tc_case_1, test_srmv2_abort_request);
  tcase_add_test (tc_case_1, test_srmv2_rmdir);
  tcase_add_test (tc_case_1, test_srmv2_rm);
  tcase_add_test (tc_case_1, test_srmv2_mkdir);


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


//	TestMkdir();
//	TestRm();
//	TestStatusOfLs();
//	TestIt();
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


void TestLs()
{
	struct srm_mdfilestatus *filestatus;
	struct srm_ls_output output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	struct srm_ls_input input;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/"};
	srm_call_status result;

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
	//fail_if ((result  != srm_call_status_FAILURE),
				   //"Expected Failure 1!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test2;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_FAILURE),
	//				"Expected Failure 2!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test3;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_FAILURE),
	//				"Expected Failure 3!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test4;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_FAILURE),
	//				"Expected Failure 4!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test5;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_TIMEOUT),
	//				"Expected Timeout!\n");
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test6;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_QUEUED),
	//				"Expected Queued!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test7;
	result = srmv2_ls_async_internal(&context,&input,&output,&internal_context);
	//fail_if ((result  != srm_call_status_SUCCESS),
	//				"Expected Success!\n");
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
void TestRm()
{
	struct srmv2_filestatus *filestatus;
	struct srm_rm_input input;
	struct srm_rm_output output;
	const char *srmfunc = "testfunc";
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
				   //"Expected Failure 1!\n");

	output.statuses = &filestatus;
	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test2;
	result = srmv2_rm(&context,&input,&output);
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
