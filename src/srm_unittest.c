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


	/*internal_context.retstatus = NULL;
	internal_context.attempt = 1; // be careful changing this number
	internal_context.end_time = time(NULL)+10000;

	result = back_off_logic(&context,srmfunc,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE)||(errno != ECOMM),
			   "if internal_context->retstatus is null the function must return FAILURE!\n");

	internal_context.retstatus = &retstatus;
	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context);
	fail_if ((internal_context.attempt   != 2),
				   "Wait new attempt!\n");

	fail_if ((result  != srm_call_status_INTERNAL_ERROR),
				   "Expected Internal Error!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREINTERNAL_USCOREERROR;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 1!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 2!\n");

	internal_context.attempt = 11;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_TIMEOUT),
					   "Expected Timeout 3!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREQUEUED;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 1!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCOREREQUEST_USCOREINPROGRESS;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_QUEUED),
					   "Expected Queued 2!\n");

	internal_context.attempt = 1;
	retstatus.statusCode = SRM_USCORESUCCESS;
	result = back_off_logic(&context,srmfunc,&internal_context);

	fail_if ((result  != srm_call_status_SUCCESS),
					   "Expected Success!\n");

	internal_context.attempt = 1;
		retstatus.statusCode = SRM_USCOREFAILURE;
		result = back_off_logic(&context,srmfunc,&internal_context);

		fail_if ((result  != srm_call_status_FAILURE),
						   "Expected Failure!\n");*/
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
	struct srm_ls_output* output;
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
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test2;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
					"Expected Failure 2!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test3;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
					"Expected Failure 3!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test4;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
					"Expected Failure 4!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test5;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_TIMEOUT),
					"Expected Timeout!\n");
	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;


	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test6;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_QUEUED),
					"Expected Queued!\n");

	call_function.call_srm2__srmLs = soap_call_srm2__srmLs_test7;
	result = srmv2_ls_async(&context,&input,&output,&internal_context);
	fail_if ((result  != srm_call_status_SUCCESS),
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
	struct srm_Rm_output* output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	srm_call_status result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	// TODO TODO TODO internal_context.token = test_string;
	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test1;
	result = srmv2_status_of_ls_request(&context,&output,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
				   "Expected Failure 1!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test2;
	result = srmv2_status_of_ls_request(&context,&output,&internal_context);
	fail_if ((result  != srm_call_status_TIMEOUT),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test3;
	result = srmv2_status_of_ls_request(&context,&output,&internal_context);
	fail_if ((result  != srm_call_status_TIMEOUT),
				   "Expected Timeout!\n");

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;
	call_function.call_srm2__srmStatusOfLsRequest = soap_call_srm2__srmStatusOfLs_test4;
	result = srmv2_status_of_ls_request(&context,&output,&internal_context);
	fail_if ((result  != srm_call_status_SUCCESS),
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
// test test_srmv2_Rm_async
//////////////////////////////////////////////////////////////////
START_TEST (test_srmv2_abort_request)
{
	struct srm_Rm_output* output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm_internal_context internal_context;
	struct srm2__TReturnStatus retstatus;
	srm_call_status result;

	internal_context.attempt = 1;
	internal_context.end_time = time(NULL)+10000;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test1;
	result = srmv2_abort_request(&context,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
				   "Expected Failure 1!\n");

	// TODO TODO TODO internal_context.token = test_string;
	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test1;
	result = srmv2_abort_request(&context,&internal_context);
	fail_if ((result  != srm_call_status_SUCCESS),
				   "Expected Success!\n");

	call_function.call_srm2__srmAbortRequest = soap_call_srm2__abort_request_test2;
	result = srmv2_abort_request(&context,&internal_context);
	fail_if ((result  != srm_call_status_FAILURE),
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
	struct srm_rmdir_input input;
	struct srm_rmdir_output* output;
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
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmRmdir = soap_call_srm2__srmRmDir_test2;
	result = srmv2_rmdir(&context,&input,&output);
	fail_if ((result  != -1),
				   "Expected Failure 2!\n");

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
	struct srm_rm_input input;
	struct srm_rm_output* output;
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm2__TReturnStatus retstatus;
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

	call_function.call_srm2__srmRm = soap_call_srm2__srmRm_test2;
	result = srmv2_rm(&context,&input,&output);
	fail_if ((result  != 0),
				   "Expected Success!\n");
}
END_TEST

/*int srmv2_mkdir(struct srm_context *context,struct srm_mkdir_input *input)
{
	char file[1024];
	int flags;
	int sav_errno = 0;
	char *p, *endp;
	struct srm2__srmMkdirResponse_ rep;
	struct srm2__srmMkdirRequest req;
	struct srm2__TReturnStatus *repstatp;
	struct soap soap;
	const char srmfunc[] = "Mkdir";

	srm_soap_init(&soap);

	memset (&req, 0, sizeof (struct srm2__srmMkdirRequest));

	strncpy (file, input->dir_name, 1023);

	if ((p = endp = strrchr (file, '/')) == NULL) {
		srm_errmsg (context, "[SRM][srmv2_makedirp][EINVAL] %s: Invalid SURL",  input->dir_name);
		srm_soap_deinit(&soap);
		errno = EINVAL;
		return (-1);
	}

	// 1st cycle, trying to create directories ascendingly, until success
	do {
		*p = 0;
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir (&soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			srm_soup_call_err(context,&soap,srmfunc);
			srm_soap_deinit(&soap);
			return (-1);
		}

		repstatp = rep.srmMkdirResponse->returnStatus;
		sav_errno = statuscode2errno (repstatp->statusCode);

		if (sav_errno != 0 && sav_errno != EEXIST && sav_errno != EACCES && sav_errno != ENOENT)
		{
			if (repstatp->explanation && repstatp->explanation[0])
			{
				srm_errmsg (context, "[SE][%s][%s] %s: %s: %s",
						srmfunc, statuscode2errmsg(repstatp->statusCode),
						context->srm_endpoint, input->dir_name, repstatp->explanation);
			}else
			{
				srm_errmsg (context, "[SE][%s][%s] %s: %s: <none>",
						srmfunc, statuscode2errmsg(repstatp->statusCode),
						context->srm_endpoint, input->dir_name);
			}
			srm_soap_deinit(&soap);
			errno = sav_errno;
			return (-1);
		}
	} while (sav_errno == ENOENT && (p = strrchr (file, '/')) != NULL);

	if (p == NULL) {
		// should never happen, failure must appear in soap call
		srm_errmsg (context, "[SRM][srmv2_makedirp][EINVAL] %s: Invalid SURL", input->dir_name);
		srm_soap_deinit(&soap);
		errno = EINVAL;
		return (-1);
	}

	// 2nd cycle, creating directories descendingly as of the one created by previous cycle
	*p = '/';
	sav_errno = 0;
	while (sav_errno == 0 && p < endp && (p = strchr (p + 1, 0)) != NULL)
	{
		req.SURL = file;

		if (call_function.call_srm2__srmMkdir(&soap, context->srm_endpoint, srmfunc, &req, &rep))
		{
			srm_soup_call_err(context,&soap,srmfunc);
			srm_soap_deinit(&soap);
			errno = ECOMM;
			return (-1);
		}

		if (rep.srmMkdirResponse == NULL || (repstatp = rep.srmMkdirResponse->returnStatus) == NULL)
		{
			srm_errmsg (context, "[SE][%s][] %s: <empty response>",
					srmfunc, context->srm_endpoint);
			srm_soap_deinit(&soap);
			errno = ECOMM;
			return (-1);
		}

		sav_errno = statuscode2errno (repstatp->statusCode);

	*//*	if (sav_errno != 0) {
			if (repstatp->explanation && repstatp->explanation[0])
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: %s",
						gfal_remote_type, srmfunc, statuscode2errmsg(repstatp->statusCode),
						dest_file, repstatp->explanation);
			else
				gfal_errmsg (errbuf, errbufsz, GFAL_ERRLEVEL_ERROR, "[%s][%s][%s] %s: <none>",
						gfal_remote_type, srmfunc, statuscode2errmsg(repstatp->statusCode), dest_file);

			srm_soap_deinit(&soap);
			errno = sav_errno;
			return (-1);
		}*/
/*
		*p = '/';
	}

	srm_soap_deinit(&soap);
	//strncpy (lastcreated_dir, dest_file, 1024);
    errno = 0;
	return (0);
}*/
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
	const char *srmfunc = "testfunc";
	struct srm_context context;
	struct srm2__TReturnStatus retstatus;
	int result;

	call_function.call_sleep = mock_sleep; // set mock sleep function

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.srm_endpoint = "test";

	input.dir_name = test_strings;

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test1;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != -1),
				   "Expected Failure 1!\n");

	call_function.call_srm2__srmMkdir = soap_call_srm2__srmMkdir_test2;
	result = srmv2_mkdir(&context,&input);
	fail_if ((result  != -1),
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
int main(void)
{
	int number_failed;
	int i;

	Suite *s = test_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	TestIt();


	//printf("TEST\n");
	return EXIT_SUCCESS;
}


