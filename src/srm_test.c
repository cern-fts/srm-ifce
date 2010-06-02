#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <errno.h>
#include "srm_types.h"
#include "srm_ifce.h"


char *test_srm_endpoint =  "httpg://lxb7993.cern.ch:8446/srm/managerv2";
char *source_file = "file:///etc/group";
char *test_file1 = "srm://lxb7993.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/test_file1";
char *test_file2 = "srm://lxb7993.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/test_file2";
char *test_unexisting = "srm://lxb7993.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/unexisting";
char *test_dir = "srm://lxb7993.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test";

void PrintResult(struct srmv2_mdfilestatus* output);
void PrintPinFileStatuses(struct srmv2_pinfilestatus *statuses, int count);
void TestPrepareToPutPrepareToGet();

int TestAbortFiles(char **files,char *token);
int TestBringOnline(char **files,char **protocols);
int TestReleaseFiles(char **files,char *token);
int TestPing(char *endpoint);
int TestLs(char *surl);

void SetCopyCommand(char **command)
{
	asprintf (command, "lcg-cr --verbose --nobdii -D srmv2 --vo dteam  %s -d %s ", source_file,test_file1);
}
void SetDelCommand(char **command,char *file)
{
	asprintf (command, "lcg-del --verbose --nobdii -D srmv2 --vo dteam %s", file);
}
void SetRegisterFileCommand(char **command,char *file)
{
	asprintf (command, "lcg-rf --verbose --nobdii -D srmv2 --vo dteam %s", file);
}
void SetDelDirCommand(char **command,char *file)
{
	asprintf (command, "lcg-del --dir --verbose --nobdii -D srmv2 --vo dteam %s", file);
}
void TestDirectoryFunctions()
{
	char *command;
	int a;
	struct srm_context context;
	struct srm_ls_input input_ls;
	struct srm_ls_output* output_ls;
	struct srm_rm_input input_rm;
	struct srm_rm_output* output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output* output_rmdir;
	struct srm_mkdir_input input_mkdir;
	int j;

	char *test_surls_rm[] = {test_file1};


	char *test_surl_mkdir;
	char *test_surl_rmdir;
	asprintf(&test_surl_rmdir,"%s/test_dir",test_dir);
	asprintf(&test_surl_mkdir,"%s/test_dir/1/2",test_dir);

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	// delete file1
    SetDelCommand(&command,test_file1);
    system(command);

	// delete folder
	input_rmdir.recursive = 1;
	input_rmdir.surl = test_dir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	printf("Remove dir:%s %d\n",input_rmdir.surl,a); //fail if a != 0



	a = TestLs(test_dir);
	// fail if a != -1

	SetCopyCommand(&command);
	system(command);

	a = TestLs(test_dir);
	// fail if a != 1

	input_mkdir.dir_name = test_surl_mkdir;
	a = srm_mkdir(&context,&input_mkdir);
	printf("Mkdir:%s %d \n",input_mkdir.dir_name,a);
	// fail if a != 0

	a = TestLs(test_surl_rmdir);
	// fail if a != 1

	a = TestLs(test_dir);
	// fail if a != 2

	input_rmdir.recursive = 1;
	input_rmdir.surl = test_surl_rmdir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	// fail if a != 0

	a = TestLs(test_dir);
	// fail if a != 1


	input_rm.nbfiles = 1;
	input_rm.surls = test_surls_rm;
	a = srm_rm(&context,&input_rm,&output_rm);
	for(j=0;j<a;j++)
	{
		printf("Remove files:%s\n",input_rm.surls[j],a);
	}

	a = TestLs(test_dir);
	// fail if a != 0


	input_rmdir.recursive = 1;
	input_rmdir.surl = test_dir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	// fail if a != 0


/*
    system("lcg-cp --verbose --nobdii -D srmv2 --vo dteam file:///etc/group srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/proba");

    i = srm_ls(&context,&input_ls,&output_ls);

	if (!i)
	{
		PrintResult(output_ls);
	}

    i = srm_rm(&context,&input_rm,&output_rm);
    printf("Remove:%d\n",i);
    i = srm_rmdir(&context,&input_rmdir,&output_rmdir);
    printf("Remove dir:%d\n",i);

    i = srm_ls(&context,&input_ls,&output_ls);

    if (!i)
	{
		PrintResult(output_ls);
	}*/
}
void TestPrepareToPutPrepareToGet()
{
	int a,b,c;
	char *command;
	char *test_surls_get[] = {test_file1};
	char *test_surls_put[] = {test_file2};
	char *test_surls_unexisting[] = {test_unexisting};


	char *protocols[] = {"file","gsiftp",NULL}; //"rfio","dcap","gsidcap","kdcap",""
	struct srm_context context;
	struct srm_preparetoget_input input_get;
	struct srm_preparetoget_output output_get;
	struct srm_preparetoput_input input_put;
    struct srm_preparetoput_output output_put,output_put2,output_put3;
	struct srmv2_pinfilestatus *filestatuses;
	long int filesizes[1] ={ 1024 };

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_get.nbfiles = 1;
	input_get.desiredpintime = 1000;
	input_get.surls = test_surls_get;
	input_get.protocols = protocols;
	input_get.spacetokendesc = NULL;


	input_put.filesizes = filesizes;
	input_put.nbfiles = 1;
	input_put.desiredpintime = 1000;
	input_put.surls = test_surls_put;
	input_put.protocols = protocols;
	input_put.spacetokendesc = NULL;

	SetCopyCommand(&command);
	system(command);

	// delete file2
    SetRegisterFileCommand(&command,test_file2);
    system(command);
    SetDelCommand(&command,test_file2);
    system(command);;

	a = TestBringOnline(test_surls_get,protocols);
	// fail if a != 1

    b = TestBringOnline(test_surls_put,protocols);
    // fail if b != -1

    a = srm_prepeare_to_get(&context,&input_get,&output_get);
    // if a != 1 error
//	PrintPinFileStatuses(output_get.filestatuses,a);


    b = srm_prepeare_to_put(&context,&input_put,&output_put);
    // if b != 1 error

    b = srm_prepeare_to_put(&context,&input_put,&output_put2);
    // if b != -1 error
    //PrintPinFileStatuses(output_put.filestatuses,b);

	a = TestAbortFiles(test_surls_put,output_put.token);
	// fail if  a != 1

	// delete file
    b = srm_prepeare_to_put(&context,&input_put,&output_put);
    // if b != 1 error
   // PrintPinFileStatuses(output_put.filestatuses,b);

    if (b>0)
    {


    	//TestAbortFiles(test_surls_put,output_put.token);
    	if (a>0 && b>0)
    	{
    		TestGlobusUrlCopy(output_get.filestatuses[0].turl,output_put.filestatuses[0].turl);
    	}

    	a = TestReleaseFiles(test_surls_get,output_get.token);
    	// fail if a!=1

    	b = TestPutDone(test_surls_put,output_put.token);
    	// fail if b!=1

    	input_get.surls = test_surls_unexisting;
    	a = srm_prepeare_to_get(&context,&input_get,&output_get);
    	// fail if a!=-1



    /*	printf("Token Get: %s \nToken Put: %s\n",output_get.token,output_put.token);
    	//TestPutDone(test_surls_put,output_put.token);
        TestAbortRequest(output_put.token);
     	TestAbortRequest(output_get.token);*/


    }

    SetDelCommand(&command,test_file1);
    system(command);
    SetRegisterFileCommand(&command,test_file2);
    system(command);
    SetDelCommand(&command,test_file2);
    system(command);
    SetDelDirCommand(&command,test_dir);
    system(command);

}

//////////////////////////////////////////////////////////////////
// test test_srm_ping
//////////////////////////////////////////////////////////////////
START_TEST (test_data_transfer_functions)
{
	int a,b,c;
	char *command;
	char *test_surls_get[] = {test_file1};
	char *test_surls_put[] = {test_file2};
	char *test_surls_unexisting[] = {test_unexisting};


	char *protocols[] = {"file","gsiftp",NULL}; //"rfio","dcap","gsidcap","kdcap",""
	struct srm_context context;
	struct srm_preparetoget_input input_get;
	struct srm_preparetoget_output output_get;
	struct srm_preparetoput_input input_put;
	struct srm_preparetoput_output output_put,output_put2,output_put3;
	struct srmv2_pinfilestatus *filestatuses;
	long int filesizes[1] ={ 1024 };

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_get.nbfiles = 1;
	input_get.desiredpintime = 1000;
	input_get.surls = test_surls_get;
	input_get.protocols = protocols;
	input_get.spacetokendesc = NULL;


	input_put.filesizes = filesizes;
	input_put.nbfiles = 1;
	input_put.desiredpintime = 1000;
	input_put.surls = test_surls_put;
	input_put.protocols = protocols;
	input_put.spacetokendesc = NULL;

	SetCopyCommand(&command);
	system(command);

	// delete file2
	SetRegisterFileCommand(&command,test_file2);
	system(command);
	SetDelCommand(&command,test_file2);
	system(command);;

	a = TestBringOnline(test_surls_get,protocols);
	fail_if ((a != 1), "Expected Success !\n");
	// fail if a != 1

	b = TestBringOnline(test_surls_put,protocols);
	fail_if ((b != -1), "Expected Failure !\n");
	// fail if b != -1

	a = srm_prepeare_to_get(&context,&input_get,&output_get);
	fail_if ((a != 1), "Expected Success !\n");
	// if a != 1 error
//	PrintPinFileStatuses(output_get.filestatuses,a);


	b = srm_prepeare_to_put(&context,&input_put,&output_put);
	fail_if ((b != 1), "Expected Success !\n");
	// if b != 1 error

	b = srm_prepeare_to_put(&context,&input_put,&output_put2);
	fail_if ((b != -1), "Expected Failure !\n");
	// if b != -1 error
	//PrintPinFileStatuses(output_put.filestatuses,b);

	a = TestAbortFiles(test_surls_put,output_put.token);
	fail_if ((a != 1), "Expected Success !\n");
	// fail if  a != 1

	// delete file
	b = srm_prepeare_to_put(&context,&input_put,&output_put);
	fail_if ((b != 1), "Expected Success !\n");
	// if b != 1 error
   // PrintPinFileStatuses(output_put.filestatuses,b);

	if (b>0)
	{
		if (a>0 && b>0)
		{
			TestGlobusUrlCopy(output_get.filestatuses[0].turl,output_put.filestatuses[0].turl);
		}

		a = TestReleaseFiles(test_surls_get,output_get.token);
		fail_if ((a != 1), "Expected Success !\n");
		// fail if a!=1

		b = TestPutDone(test_surls_put,output_put.token);
		fail_if ((b != 1), "Expected Success !\n");
		// fail if b!=1

		input_get.surls = test_surls_unexisting;
		a = srm_prepeare_to_get(&context,&input_get,&output_get);
		fail_if ((a != -1), "Expected Failure !\n");
		// fail if a!=-1
	}

	SetDelCommand(&command,test_file1);
	system(command);
	SetRegisterFileCommand(&command,test_file2);
	system(command);
	SetDelCommand(&command,test_file2);
	system(command);
	SetDelDirCommand(&command,test_dir);
	system(command);
}
END_TEST
//////////////////////////////////////////////////////////////////
// test test_srm_ping
//////////////////////////////////////////////////////////////////
START_TEST (test_srm_ping)
{
	struct srm_ping_output output;
	struct srm_context context;
	int result;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.version = TYPE_SRMv2;


	context.srm_endpoint = test_srm_endpoint;
	result = srm_ping(&context,&output);
	fail_if ((result  != 0),
					"Expected Success !\n");


	srm_set_timeout_connect(1);
	context.srm_endpoint = "test";
	result = srm_ping(&context,&output); // returns 12?
	fail_if ((result  == 0),
					"Expected Failure !\n");
	srm_set_timeout_connect(60);

}
END_TEST
Suite * test_suite (void)
{
  Suite *s = suite_create ("New srm interface communication with real endpoint test suit");

  TCase *tc_case_1 = tcase_create ("T1");
  TCase *tc_case_2 = tcase_create ("T2");

  tcase_add_checked_fixture (tc_case_1, NULL,NULL);
  tcase_add_test (tc_case_1, test_srm_ping);
  suite_add_tcase (s, tc_case_1);

  tcase_add_checked_fixture (tc_case_2, NULL,NULL);
  tcase_add_test (tc_case_2, test_data_transfer_functions);
  tcase_set_timeout(tc_case_2, 60);
  suite_add_tcase (s, tc_case_2);

  return s;
}

int DoTests()
{
	int number_failed;
	Suite *s = test_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

///////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////
int main(void)
{
	TestDirectoryFunctions();

	//return DoTests();
}

void PrintPinFileStatuses(struct srmv2_pinfilestatus *statuses, int count)
{
	int i;
	for(i=0;i<count;i++)
	{
		printf("Surl: %s \n",statuses[i].surl);
		printf("Turl: %s \n",statuses[i].turl);
		printf("Pin lifetime: %d \n",statuses[i].pinlifetime);

	}
	if (count <=0)
	{
		printf("Count: %d \n",count);
	}
}

int TestLs(char *surl)
{
	int i;
	char *test_surls_ls[] = {surl};
	struct srm_context context;
	struct srm_ls_input input_ls;
	struct srm_ls_output output_ls;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_ls.nbfiles = 1;
	input_ls.count = 0;
	input_ls.numlevels  = 1;
	input_ls.surls = test_surls_ls;
	input_ls.offset = 0;

	//	system("lcg-ls --verbose --nobdii -D srmv2 srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1");
	//int srm_ls_async(struct srm_context *context,
	//	struct srm_ls_input *input,struct srm_ls_output *output)
    i = srm_ls(&context,&input_ls,&output_ls);

	if (i > 0)
	{
		PrintResult(output_ls.statuses);
		return output_ls.statuses->nbsubpaths;
	}
	return i;

}
void PrintFilestatuses(struct srmv2_pinfilestatus* output)
{

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
int TestPutDone(char** surls,char *token)
{
	struct srmv2_pinfilestatus *filestatuses;
	int c;
	struct srm_putdone_input input_putdone;
	struct srm_context context;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

   	input_putdone.nbfiles = 1;
	input_putdone.surls = surls;
	input_putdone.reqtoken = token;
	c = srm_put_done(&context,&input_putdone,&filestatuses);
	printf("Put Done\nToken: %s \nSurl: %s\nResult: %d\n",token,surls[0],c);

	return c;
}

int TestAbortRequest(char *token)
{
	struct srmv2_pinfilestatus *filestatuses;
	int c;
	struct srm_context context;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;


	c = srm_abort_request(&context,token);
	printf("Abort request\nToken: %s \nResult: %d\n",token,c);

	return c;
}
void TestGlobusUrlCopy(char *sourceturl,char *destinationturl)
{
	char* globus_url_copy;
	asprintf(&globus_url_copy,"globus-url-copy %s %s ",sourceturl,destinationturl);
	printf("%s \n",globus_url_copy);
	system(globus_url_copy);
}
int TestAbortFiles(char **files,char *token)
{
	struct srmv2_filestatus *filestatuses;
	int c;
	struct srm_context context;
	struct srm_abort_files_input input;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input.nbfiles = 1;
	input.surls = files;
	input.reqtoken = token;

	c = srm_abort_files(&context,&input,&filestatuses);
	printf("Abort files\nFile: %s\nToken: %s \nResult: %d\n",files[0],token,c);

	return c;
}
int TestReleaseFiles(char **files,char *token)
{
	struct srmv2_filestatus *filestatuses;
	int a;
	struct srm_context context;
	struct srm_releasefiles_input input;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input.nbfiles = 1;
	input.surls = files;
	input.reqtoken = token;

	a = srm_release_files(&context,&input,&filestatuses);
	printf("Release files\nFile: %s\nToken: %s \nResult: %d\n",files[0],token,a);

	return a;
}
int TestBringOnline(char **files,char **protocols)
{
	struct srm_context context;
	struct srm_bringonline_input input_bringonline;
	struct srm_bringonline_output output_bringonline;
	int a;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_bringonline.desiredpintime = 1000;
	input_bringonline.nbfiles = 1;
	input_bringonline.protocols = protocols;
	input_bringonline.surls = files;
	input_bringonline.spacetokendesc = NULL;



	a = srm_bring_online(&context,&input_bringonline,&output_bringonline);
	printf("BRING ONLINE \n");
	PrintPinFileStatuses(output_bringonline.filestatuses,a);

	return a;
}

void TestSpaceTokensSpaceMetadata()
{
	int a;
	struct srm_context context;

	struct srm_getbestspacetokens_input input_bestspacetokens;

	struct srm_getspacemd_input input_md;
	struct srm_spacemd  *spacemd;

	struct srm_getspacetokens_input input_space_tokens;
	struct srm_getspacetokens_output output_space_tokens;



	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_space_tokens.spacetokendesc = "NULL";

	//output_space_tokens.
	a = srm_getspacetokens(&context,&input_space_tokens,&output_space_tokens);

	//input_md pseudo = output_space_tokens
	input_md.nbtokens = 0;
	input_md.spacetokens = NULL;
	a = srm_getspacemd(&context,&input_md,&spacemd);

	input_bestspacetokens.neededsize = 1000;
	//input_bestspacetokens.spacetokendesc = ....;
	a = srm_getbestspacetoken(&context,&input_bestspacetokens);



}
int TestPing(char *endpoint)
{
	int a;
	struct srm_context context;
	struct srm_ping_output output;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	a = srm_ping(&context,&output);

	if (a == 0)
	{
		printf("Ping result: %s\n",output.versioninfo);
	}else
	{
		printf("Ping error \n",output.versioninfo);
	}
	return a;
}
