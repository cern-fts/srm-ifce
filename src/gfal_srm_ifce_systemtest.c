#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <errno.h>
#include "gfal_srm_ifce_types.h"
#include "gfal_srm_ifce.h"

char *test_srm_endpoint =  "srm://lxbra1910.cern.ch:8446/srm/managerv2";
char *source_file = "file:///etc/group";
char *test_file1 = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/test_file1";
char *test_file2 = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/test_file2";
char *test_unexisting = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/unexisting";
char *test_dir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/srm_test/";
char *test_spacedescriptor = "srm_test_space";

void PrintResult(struct srmv2_mdfilestatus *print_output);
void PrintPinFileStatuses(struct srmv2_pinfilestatus *statuses, int count);
void TestPrepareToPutPrepareToGet();

void GlobusUrlCopy(char *sourceturl,char *destinationturl);
int TestPutDone(char** surls,char *token);
int TestAbortFiles(char **files,char *token);
int TestBringOnline(char **files,char **protocols);
int TestReleaseFiles(char **files,char *token);
int TestPing(char *endpoint);
int TestLs(char *surl);
int DelSurl(int nbfiles,char **surls);
int MkDir(char *directory);

void CopyFile(char *file)
{
	char *command;
	asprintf (&command, "lcg-cp --nobdii -D srmv2 --vo dteam  %s %s ", source_file,file);
	//printf("%s \n",command);
	system(command);
}
void GlobusUrlCopy(char *sourceturl,char *destinationturl)
{
	char* globus_url_copy;
	asprintf(&globus_url_copy,"globus-url-copy %s %s ",sourceturl,destinationturl);
	//printf("%s \n",globus_url_copy);
	system(globus_url_copy);
}

int FakeCopy(char *surl) // does prepare to put and put done without copying a single byte
{
	char *surls[] = {surl};
	int ret;
	char *protocols[] = {"file","gsiftp",NULL}; //"rfio","dcap","gsidcap","kdcap",""
	struct srm_context context;
	struct srm_preparetoput_input input_put;
	struct srm_preparetoput_output output_put;
	SRM_LONG64 filesizes[1] ={ 0 };

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

	input_put.filesizes = filesizes;
	input_put.nbfiles = 1;
	input_put.desiredpintime = 1000;
	input_put.surls = surls;
	input_put.protocols = protocols;
	input_put.spacetokendesc = NULL;


	MkDir(test_dir);

	ret = srm_prepare_to_put(&context,&input_put,&output_put);

	if (ret!= -1)
	{
		ret = TestPutDone(surls,output_put.token);
	}
	return ret;
}
int DelDir(char *dir)
{
	struct srm_context context;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output output_rmdir;

	srm_context_init(&context,test_srm_endpoint);

	input_rmdir.recursive = 1;
	input_rmdir.surl = dir;

	return srm_rmdir(&context,&input_rmdir,&output_rmdir);
}
int DelSurl(int nbfiles,char **surls)
{
	struct srm_context context;
	struct srm_rm_input rm_input;
	struct srm_rm_output rm_output;

	srm_context_init(&context,test_srm_endpoint);
	rm_input.nbfiles = nbfiles;
	rm_input.surls = surls;

	return srm_rm(&context,&rm_input,&rm_output);
}
int MkDir(char *directory)
{
	char *command;
	int a;
	struct srm_context context;
	struct srm_rm_input input_rm;
	struct srm_rm_output output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output output_rmdir;
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
	context.version = VERSION_2_2;

	input_mkdir.dir_name = directory;
	return srm_mkdir(&context,&input_mkdir);
}

//////////////////////////////////////////////////////////////////
// test test_directory_functions
//////////////////////////////////////////////////////////////////
START_TEST (test_directory_functions)
{
	char *command;
	int a;
	struct srm_context context;
	struct srm_rm_input input_rm;
	struct srm_rm_output output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output output_rmdir;
	struct srm_mkdir_input input_mkdir;
	int j;

	char *test_surls_rm[] = {test_file1};

	char *test_surls_cleanup[] = {test_file1,test_file2};

	char *test_surl_mkdir;
	char *test_surl_rmdir;
	asprintf(&test_surl_rmdir,"%s/test_dir",test_dir);
	asprintf(&test_surl_mkdir,"%s/test_dir/1/2",test_dir);

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

	// clean up files and folders
	DelSurl(2,test_surls_cleanup);
	a = DelDir(test_dir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);



	a = TestLs(test_dir);
	fail_if ((a != -1), "Expected Unexistent Folder!");

	CopyFile(test_file1);

	a = TestLs(test_dir);
	fail_if ((a != 1), "Expected One File!");

	input_mkdir.dir_name = test_surl_mkdir;
	a = srm_mkdir(&context,&input_mkdir);
	//printf("Mkdir:%s %d \n",input_mkdir.dir_name,a);
	fail_if ((a != 0), "Expected Success!");

	a = TestLs(test_surl_rmdir);
	fail_if ((a != 1), "Expected 1 File in this folder!");

	a = TestLs(test_dir);
	fail_if ((a != 2), "Expected 2 Files in this folder!");

	input_rmdir.recursive = 1;
	input_rmdir.surl = test_surl_rmdir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	fail_if ((a != 1), "Expected Success!");

	a = TestLs(test_dir);
	fail_if ((a != 1), "Expected 1 File in this folder!");

	input_rm.nbfiles = 1;
	input_rm.surls = test_surls_rm;
	a = srm_rm(&context,&input_rm,&output_rm);
	for(j=0;j<a;j++)
	{
		//printf("Remove files:%s\n",input_rm.surls[j],a);
	}

	a = TestLs(test_dir);
	fail_if ((a != 0), "Expected Empty Folder!");


	input_rmdir.recursive = 1;
	input_rmdir.surl = test_dir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	fail_if ((a != 1), "Expected Success!");
}
END_TEST

//////////////////////////////////////////////////////////////////
// test test_data_transfer_functions
//////////////////////////////////////////////////////////////////
START_TEST (test_data_transfer_functions)
{
	int a,b,c;
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
	SRM_LONG64 filesizes[1] ={ 1024 };

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

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

	// cleanup files
	DelSurl(1,test_surls_get);
	DelSurl(1,test_surls_put);

	CopyFile(test_file1);

	a = TestBringOnline(test_surls_get,protocols);
	fail_if ((a != 1), "Expected Success !");

	b = TestBringOnline(test_surls_put,protocols);
	fail_if ((b != -1), "Expected Failure !");

	a = srm_prepare_to_get(&context,&input_get,&output_get);
	fail_if ((a != 1), "Expected Success !");

	b = srm_prepare_to_put(&context,&input_put,&output_put);
	fail_if ((b != 1), "Expected Success !");

	b = srm_prepare_to_put(&context,&input_put,&output_put2);
	fail_if ((b != -1), "Expected Failure !");

	a = TestAbortFiles(test_surls_put,output_put.token);
	fail_if ((a != 1), "Expected Success !");

	b = srm_prepare_to_put(&context,&input_put,&output_put);
	fail_if ((b != 1), "Expected Success !");

	if (b>0)
	{
		if (a>0 && b>0)
		{
			GlobusUrlCopy(output_get.filestatuses[0].turl,output_put.filestatuses[0].turl);
		}

		a = TestReleaseFiles(test_surls_get,output_get.token);
		fail_if ((a != 1), "Expected Success !");

		b = TestPutDone(test_surls_put,output_put.token);
		fail_if ((b != 1), "Expected Success !");

		input_get.surls = test_surls_unexisting;
		a = srm_prepare_to_get(&context,&input_get,&output_get);
		fail_if ((a != -1), "Expected Failure !");
	}

	DelSurl(1,test_surls_get);
	DelSurl(1,test_surls_put);
	DelDir(test_dir);
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
	context.version = VERSION_2_2;


	context.srm_endpoint = test_srm_endpoint;
	result = srm_ping(&context,&output);
	fail_if ((result  != 0),
					"Expected Success !");


	srm_set_timeout_connect(1);
	context.srm_endpoint = "test";
	result = srm_ping(&context,&output); // returns 12?
	fail_if ((result  == 0),
					"Expected Failure !");
	srm_set_timeout_connect(60);

}
END_TEST

//////////////////////////////////////////////////////////////////
// test test_srm_space_management
//////////////////////////////////////////////////////////////////
START_TEST (test_srm_space_management)
{
	struct srm_getspacemd_input input_metadata;
	struct srm_spacemd *spaces;

	struct srm_getbestspacetokens_input input_bestspacetoken;

	struct srm_getspacetokens_input input_get;
	struct srm_getspacetokens_output output_get;

	struct srm_reservespace_input input_reserve;
	struct srm_reservespace_output output_reserve1;
	struct srm_reservespace_output output_reserve2;
	struct srm_context context;
	int result,i;
	char *test_spacetoken_descriptor = test_spacedescriptor;
	char *best_spacetoken;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.version = VERSION_2_2;
	context.srm_endpoint =  test_srm_endpoint;

	input_reserve.desired_lifetime = 100;
	input_reserve.desired_size = 1048576*2; // 2MB
	input_reserve.spacetokendescriptor = "srm_test_space";

	input_get.spacetokendesc = test_spacetoken_descriptor;

	result = srmv2_getspacetokens (&context,
			&input_get,
			&output_get);


	for(i=0;i<output_get.nbtokens;i++)
	{
		//printf("GetSpaceTokenResult[%d] : %s\n",i,output_get.spacetokens[i]);
		result = srmv2_releasespace_test_function(&context,output_get.spacetokens[i]);
	}

	result = srmv2_reservespace_test_function(&context,
			&input_reserve,
			&output_reserve1); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");

	result = srmv2_reservespace_test_function(&context,
			&input_reserve,
			&output_reserve2); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");

	input_get.spacetokendesc = "srm_test_space";

	result = srmv2_getspacetokens (&context,
			&input_get,
			&output_get); // fail if output_get.nbtokens != 2 || result != 0
	fail_if ((result != 0 || output_get.nbtokens != 2), "Expected Success !");


	for(i=0;i<output_get.nbtokens;i++)
	{
		//printf("GetSpaceTokenResult[%d] : %s\n",i,output_get.spacetokens[i]);
		fail_if ((output_get.spacetokens[i] == NULL), "Expected token !");
	}

	input_bestspacetoken.neededsize = 10;
	input_bestspacetoken.spacetokendesc = "srm_test_space";
	best_spacetoken = srmv2_getbestspacetoken (&context,&input_bestspacetoken);
	//printf("GetBestSpaceToken : %s\n",best_spacetoken); // fail if best_spacetoken == NULL

	input_metadata.nbtokens = output_get.nbtokens;
	input_metadata.spacetokens = output_get.spacetokens;
	result = srmv2_getspacemd (&context,&input_metadata,&spaces);
	for(i=0;i<input_metadata.nbtokens;i++)
	{
		//printf("GetSpaceMetadata[%d].token : %s\n",i,spaces[i].spacetoken); // fail if  == NULL
		//printf("GetSpaceMetadata[%d].owner : %s\n",i,spaces[i].owner); // fail if  == NULL
		fail_if ((spaces[i].owner == NULL), "Expected owner not null !");
		fail_if ((spaces[i].spacetoken == NULL), "Expected spacetoken not null !");
	}

	result = srmv2_releasespace_test_function(&context,output_reserve1.spacetoken); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");
	result = srmv2_releasespace_test_function(&context,output_reserve2.spacetoken); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");
}
END_TEST



//////////////////////////////////////////////////////////////////
// test test_srm_permissions
//////////////////////////////////////////////////////////////////
START_TEST (test_srm_permissions)
{
	char *surls[] = {test_file1};

	struct srm_getpermission_input input;
	struct srm_getpermission_output output;

	struct srm_checkpermission_input input_checkpermission;
	struct srmv2_filestatus *filestatuses;

	struct srm_setpermission_input input_set;
	struct srm_permission group_perm;

	struct srm_context context;

	int result;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.version = VERSION_2_2;
	context.srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	input.nbfiles = 1;
	input.surls =  surls;

	input_checkpermission.nbfiles = 1;
	input_checkpermission.surls = surls;
	input_checkpermission.amode = SRM_PERMISSION_X;

	CopyFile(test_file1);

	result = srmv2_get_permission(&context,&input,&output); // fail if result != 1
	fail_if ((result != 1), "Expected Success !");

	result = srmv2_check_permission(&context,&input_checkpermission,&filestatuses); // fail if result != 1
	fail_if ((result != 1), "Expected Success !");
	fail_if ((filestatuses[0].status != EACCES), "Expected permission error !");


	input_set.surl = surls[0];
	input_set.owner_permission = SRM_PERMISSION_RWX;
	input_set.other_permission = SRM_PERMISSION_RW;
	input_set.group_permissions_count = 0;
	input_set.group_permissions = NULL;

	input_set.user_permissions_count = 0;
    input_set.user_permissions = NULL;
	input_set.permission_type = SRM_PERMISSION_CHANGE;

	result = srmv2_set_permission(&context,&input_set); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");

	result = srmv2_check_permission(&context,&input_checkpermission,&filestatuses); // fail if result != 1
	fail_if ((result != 1), "Expected Success !");
	fail_if ((filestatuses[0].status == EACCES), "Expected permission ok !");


	group_perm.mode = SRM_PERMISSION_RW;
	group_perm.name_id = "-";

	input_set.group_permissions_count = 1;
	input_set.group_permissions = &group_perm;
	input_set.permission_type = SRM_PERMISSION_ADD;

	result = srmv2_set_permission(&context,&input_set); // fail if result != 0
	fail_if ((result != 0), "Expected Success !");


	result = srmv2_get_permission(&context,&input,&output); // fail if owner_permission != RWX and other permission RW
	fail_if ((result != 1)||(output.permissions->owner_permission != SRM_PERMISSION_RWX) , "Expected Success !");
	fail_if ((output.permissions->group_permissions_count != 1) , "Expected Success !");

	DelSurl(1,surls);
}
END_TEST

Suite * test_suite (void)
{
	Suite *s = suite_create ("New srm interface communication with real endpoint test suit");

	TCase *tc_case_1 = tcase_create ("T1");
	TCase *tc_case_2 = tcase_create ("T2");
	TCase *tc_case_3 = tcase_create ("T3");
	TCase *tc_case_4 = tcase_create ("T4");
	TCase *tc_case_5 = tcase_create ("T5");

	tcase_add_checked_fixture (tc_case_1, NULL,NULL);
	tcase_add_test (tc_case_1, test_srm_ping);
	suite_add_tcase (s, tc_case_1);

	tcase_add_checked_fixture (tc_case_2, NULL,NULL);
	tcase_add_test (tc_case_2, test_data_transfer_functions);
	tcase_set_timeout(tc_case_2, 60);
	suite_add_tcase (s, tc_case_2);

	tcase_add_checked_fixture (tc_case_3, NULL,NULL);
	tcase_add_test (tc_case_3, test_directory_functions);
	tcase_set_timeout(tc_case_3, 60);
	suite_add_tcase (s, tc_case_3);

	tcase_add_checked_fixture (tc_case_4, NULL,NULL);
	tcase_add_test (tc_case_4, test_srm_space_management);
	tcase_set_timeout(tc_case_4, 60);
	suite_add_tcase (s, tc_case_4);

	tcase_add_checked_fixture (tc_case_5, NULL,NULL);
	tcase_add_test (tc_case_5, test_srm_permissions);
	tcase_set_timeout(tc_case_5, 60);
	suite_add_tcase (s, tc_case_5);

	return s;
}


int DoTests()
{
	int number_failed;
	Suite *s = test_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void TestPermissions()
{
	char *surls[] = {test_file1};

	struct srm_getpermission_input input;
	struct srm_getpermission_output output;

	struct srm_checkpermission_input input_checkpermission;
	struct srmv2_filestatus *filestatuses;

	struct srm_setpermission_input input_set;
	struct srm_permission group_perm;

	struct srm_context context;

	int result;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.version = VERSION_2_2;
	context.srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	input.nbfiles = 1;
	input.surls =  surls;

	input_checkpermission.nbfiles = 1;
	input_checkpermission.surls = surls;
	input_checkpermission.amode = SRM_PERMISSION_X;

	CopyFile(test_file1);

	result = srmv2_get_permission(&context,&input,&output); // fail if result != 1

	result = srmv2_check_permission(&context,&input_checkpermission,&filestatuses); // fail if result != 1


	input_set.surl = surls[0];
	input_set.owner_permission = SRM_PERMISSION_RWX;
	input_set.other_permission = SRM_PERMISSION_RW;
	input_set.group_permissions_count = 0;
	input_set.group_permissions = NULL;

	input_set.user_permissions_count = 0;
    input_set.user_permissions = NULL;
	input_set.permission_type = SRM_PERMISSION_CHANGE;

	result = srmv2_set_permission(&context,&input_set); // fiail if result != 0

	result = srmv2_check_permission(&context,&input_checkpermission,&filestatuses); // fail if result != 1

	group_perm.mode = SRM_PERMISSION_RW;
	group_perm.name_id = "test";

	input_set.group_permissions_count = 1;
	input_set.group_permissions = &group_perm;
	input_set.permission_type = SRM_PERMISSION_ADD;

	result = srmv2_set_permission(&context,&input_set); // fail if result != 0



	result = srmv2_get_permission(&context,&input,&output); // fail if owner_permission != RWX and other permission RW
	// fail if output.permissions->group_permissions_count != 1
	DelSurl(1,surls);

}
void TestReserveSpace()
{
	struct srm_getspacemd_input input_metadata;
	struct srm_spacemd *spaces;

	struct srm_getbestspacetokens_input input_bestspacetoken;

	struct srm_getspacetokens_input input_get;
	struct srm_getspacetokens_output output_get;

	struct srm_reservespace_input input_reserve;
	struct srm_reservespace_output output_reserve1;
	struct srm_reservespace_output output_reserve2;
	struct srm_context context;
	int result,i;
	char *test_spacetoken_descriptor = test_spacetoken_descriptor;//"tmanev_test_space";;
	char *best_spacetoken;

	context.verbose = 0;
	context.errbuf = NULL;
	context.errbufsz = 0;
	context.version = VERSION_2_2;
	context.srm_endpoint = test_srm_endpoint;// "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	input_reserve.desired_lifetime = 100;
	input_reserve.desired_size = 1048576*2; // 2MB
	input_reserve.spacetokendescriptor = test_spacetoken_descriptor;

	input_get.spacetokendesc = test_spacetoken_descriptor;

	result = srmv2_getspacetokens (&context,
			&input_get,
			&output_get);


	for(i=0;i<output_get.nbtokens;i++)
	{
		//printf("GetSpaceTokenResult[%d] : %s\n",i,output_get.spacetokens[i]);
		result = srmv2_releasespace_test_function(&context,output_get.spacetokens[i]);
	}

	result = srmv2_reservespace_test_function(&context,
			&input_reserve,
			&output_reserve1); // fail if result != 0

	result = srmv2_reservespace_test_function(&context,
			&input_reserve,
			&output_reserve2); // fail if result != 0

	input_get.spacetokendesc = test_spacetoken_descriptor;

	result = srmv2_getspacetokens (&context,
			&input_get,
			&output_get); // fail if output_get.nbtokens != 2 || result != 0


	for(i=0;i<output_get.nbtokens;i++)
	{
		//printf("GetSpaceTokenResult[%d] : %s\n",i,output_get.spacetokens[i]);
	}

	input_bestspacetoken.neededsize = 10;
	input_bestspacetoken.spacetokendesc = test_spacetoken_descriptor;
	best_spacetoken = srmv2_getbestspacetoken (&context,&input_bestspacetoken);
	//printf("GetBestSpaceToken : %s\n",best_spacetoken); // fail if best_spacetoken == NULL

	input_metadata.nbtokens = output_get.nbtokens;
	input_metadata.spacetokens = output_get.spacetokens;
	result = srmv2_getspacemd (&context,&input_metadata,&spaces);

	for(i=0;i<input_metadata.nbtokens;i++)
	{
		//printf("GetSpaceMetadata[%d].token : %s\n",i,spaces[i].spacetoken); // fail if  == NULL
		//printf("GetSpaceMetadata[%d].owner : %s\n",i,spaces[i].owner); // fail if  == NULL
	}
	result = srmv2_releasespace_test_function(&context,output_reserve1.spacetoken); // fail if result != NULL
	result = srmv2_releasespace_test_function(&context,output_reserve2.spacetoken); // fail if result != NULL
}
void TestDataTransferFunctions()
{
	int a,b,c;
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
	SRM_LONG64 filesizes[1] ={ 1024 };

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

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

	// cleanup files
	DelSurl(1,test_surls_get);
	DelSurl(1,test_surls_put);

	/*a = MkDir(test_dir); // create dir
*/
	CopyFile(test_file1);

	//a = FakeCopy(test_file1);

	a = TestBringOnline(test_surls_get,protocols);
	//fail_if ((a != 1), "Expected Success !");
	// fail if a != 1

	b = TestBringOnline(test_surls_put,protocols);
	//fail_if ((b != -1), "Expected Failure !");
	// fail if b != -1

	a = srm_prepare_to_get(&context,&input_get,&output_get);
	//fail_if ((a != 1), "Expected Success !");
	// if a != 1 error
//	PrintPinFileStatuses(output_get.filestatuses,a);


	b = srm_prepare_to_put(&context,&input_put,&output_put);
	//fail_if ((b != 1), "Expected Success !");
	// if b != 1 error

	b = srm_prepare_to_put(&context,&input_put,&output_put2);
	//fail_if ((b != -1), "Expected Failure !");
	// if b != -1 error
	//PrintPinFileStatuses(output_put.filestatuses,b);

	a = TestAbortFiles(test_surls_put,output_put.token);
	//fail_if ((a != 1), "Expected Success !");
	// fail if  a != 1

	// delete file
	b = srm_prepare_to_put(&context,&input_put,&output_put);
	//fail_if ((b != 1), "Expected Success !");
	// if b != 1 error
   // PrintPinFileStatuses(output_put.filestatuses,b);

	if (b>0)
	{
		if (a>0 && b>0)
		{
			GlobusUrlCopy(output_get.filestatuses[0].turl,output_put.filestatuses[0].turl);
		}

		a = TestReleaseFiles(test_surls_get,output_get.token);
		//fail_if ((a != 1), "Expected Success !");

		b = TestPutDone(test_surls_put,output_put.token);
		//fail_if ((b != 1), "Expected Success !");

		input_get.surls = test_surls_unexisting;
		a = srm_prepare_to_get(&context,&input_get,&output_get);
		//fail_if ((a != -1), "Expected Failure !");
	}

	// Cleanup
	DelSurl(1,test_surls_get);
	DelSurl(1,test_surls_put);
	DelDir(test_dir);
}
void TestDirectoryFunctions()
{
	char *command;
	int a;
	struct srm_context context;
	struct srm_rm_input input_rm;
	struct srm_rm_output output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output output_rmdir;
	struct srm_mkdir_input input_mkdir;
	int j;

	char *test_surls_rm[] = {test_file1};
	char *test_surls_cleanup[] = {test_file1,test_file2};

	char *test_surl_mkdir;
	char *test_surl_rmdir;
	asprintf(&test_surl_rmdir,"%s/test_dir",test_dir);
	asprintf(&test_surl_mkdir,"%s/test_dir/1/2",test_dir);

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

	// cleanup files
	DelSurl(2,test_surls_cleanup);

	// delete folder
	input_rmdir.recursive = 1;
	input_rmdir.surl = test_dir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);



	a = TestLs(test_dir);
	//fail_if ((a != -1), "Expected Unexistent Folder!");

	a = TestLs(test_dir);
	//fail_if ((a != 1), "Expected One File!");
	// fail if a != 1

	input_mkdir.dir_name = test_surl_mkdir;
	a = srm_mkdir(&context,&input_mkdir);
	//printf("Mkdir:%s %d \n",input_mkdir.dir_name,a);
	//fail_if ((a != 0), "Expected Success!");
	// fail if a != 0

	a = TestLs(test_surl_rmdir);
	//fail_if ((a != 1), "Expected 1 File in this folder!");
	// fail if a != 1

	a = TestLs(test_dir);
	//fail_if ((a != 2), "Expected 2 Files in this folder!");
	// fail if a != 2

	input_rmdir.recursive = 1;
	input_rmdir.surl = test_surl_rmdir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	//fail_if ((a != 0), "Expected Success!");
	// fail if a != 0

	a = TestLs(test_dir);
	//fail_if ((a != 1), "Expected 1 File in this folder!");
	// fail if a != 1


	input_rm.nbfiles = 1;
	input_rm.surls = test_surls_rm;
	a = srm_rm(&context,&input_rm,&output_rm);
	for(j=0;j<a;j++)
	{
		//printf("Remove files:%s\n",input_rm.surls[j],a);
	}

	a = TestLs(test_dir);
	//fail_if ((a != 0), "Expected Empty Folder!");
	// fail if a != 0


	input_rmdir.recursive = 1;
	input_rmdir.surl = test_dir;
	a = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	//printf("Remove dir:%s %d\n",input_rmdir.surl,a);
	//fail_if ((a != 0), "Expected Success!");
	// fail if a != 0
}

///////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////

int main(void)
{
	//TestPing(test_srm_endpoint);
	//TestPrepareToPutPrepareToGet();
	//TestReserveSpace();
	//TestPermissions();
	//TestDirectoryFunctions();
    //TestDataTransferFunctions();
	//TestPermissions();
	return DoTests();
	//return 0;
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
	context.version = VERSION_2_2;

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
		//PrintResult(output_ls.statuses);
		return ((output_ls.statuses)->nbsubpaths);
	}
	return i;

}
void PrintResult(struct srmv2_mdfilestatus *print_output)
{
	int i;
	printf("Directory: %s \n",print_output->surl);
	printf("Files:\n");
	for(i=0;i<print_output->nbsubpaths;i++)
	{
		printf("%s \n",print_output->subpaths[i].surl);
	}
}
int TestPutDone(char** surls,char *token)
{
	struct srmv2_filestatus *filestatuses;
	int c;
	struct srm_putdone_input input_putdone;
	struct srm_context context;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = VERSION_2_2;

   	input_putdone.nbfiles = 1;
	input_putdone.surls = surls;
	input_putdone.reqtoken = token;
	c = srm_put_done(&context,&input_putdone,&filestatuses);
	//printf("Put Done\nToken: %s \nSurl: %s\nResult: %d\n",token,surls[0],c);

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
	context.version = VERSION_2_2;


	c = srm_abort_request(&context,token);
	//printf("Abort request\nToken: %s \nResult: %d\n",token,c);

	return c;
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
	context.version = VERSION_2_2;

	input.nbfiles = 1;
	input.surls = files;
	input.reqtoken = token;

	c = srm_abort_files(&context,&input,&filestatuses);
	//printf("Abort files\nFile: %s\nToken: %s \nResult: %d\n",files[0],token,c);

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
	context.version = VERSION_2_2;

	input.nbfiles = 1;
	input.surls = files;
	input.reqtoken = token;

	a = srm_release_files(&context,&input,&filestatuses);
	//printf("Release files\nFile: %s\nToken: %s \nResult: %d\n",files[0],token,a);

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
	context.version = VERSION_2_2;

	input_bringonline.desiredpintime = 1000;
	input_bringonline.nbfiles = 1;
	input_bringonline.protocols = protocols;
	input_bringonline.surls = files;
	input_bringonline.spacetokendesc = NULL;



	a = srm_bring_online(&context,&input_bringonline,&output_bringonline);
	//printf("BRING ONLINE \n");
	//PrintPinFileStatuses(output_bringonline.filestatuses,a);

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
	context.version = VERSION_2_2;

	input_space_tokens.spacetokendesc = "NULL";

	//output_space_tokens.
	a = srm_getspacetokens(&context,&input_space_tokens,&output_space_tokens);

	//input_md pseudo = output_space_tokens
	input_md.nbtokens = 0;
	input_md.spacetokens = NULL;
	a = srm_getspacemd(&context,&input_md,&spacemd);

	input_bestspacetokens.neededsize = 1000;
	//input_bestspacetokens.spacetokendesc = ....;
	srm_getbestspacetoken(&context,&input_bestspacetokens);



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
	context.version = VERSION_2_2;

	a = srm_ping(&context,&output);

	if (a == 0)
	{
		//printf("Ping result: %s\n",output.versioninfo);
	}else
	{
		//printf("Ping error \n",output.versioninfo);
	}
	return a;
}
