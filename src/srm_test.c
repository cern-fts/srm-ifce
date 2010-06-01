#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <errno.h>
#include "srm_types.h"
#include "srm_ifce.h"

void PrintResult(struct srmv2_mdfilestatus* output);
void PrintPinFileStatuses(struct srmv2_pinfilestatus *statuses, int count);
void TestLs();

int TestPutDone(char** surls,char *token)
{
	struct srmv2_pinfilestatus *filestatuses;
	int c;
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
void TestPrepareToPutPrepareToGet()
{
	int a,b,c;
	char *test_surls_get[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/test14"};
	char *test_surls_put[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/test600"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
	char *protocols[] = {"file","gsiftp",NULL}; //"rfio","dcap","gsidcap","kdcap",""
	struct srm_context context;
	struct srm_preparetoget_input input_get;
	struct srm_preparetoget_output output_get;
	struct srm_preparetoput_input input_put;
    struct srm_preparetoput_output output_put;
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


	TestBringOnline(test_surls_get,protocols);
    TestBringOnline(test_surls_put,protocols);

    a = srm_prepeare_to_get(&context,&input_get,&output_get);
	PrintPinFileStatuses(output_get.filestatuses,a);

    b = srm_prepeare_to_put(&context,&input_put,&output_put);
    PrintPinFileStatuses(output_put.filestatuses,b);



    if (b>0)
    {


    	//TestAbortFiles(test_surls_put,output_put.token);
    	if (a>0 && b>0)
    	{
    		TestGlobusUrlCopy(output_get.filestatuses[0].turl,output_put.filestatuses[0].turl);
    	}
    	//TestReleaseFiles(test_surls_get,output_get.token);
    	//TestPutDone(test_surls_put,output_put.token);
    	//

    	printf("Token Get: %s \nToken Put: %s\n",output_get.token,output_put.token);
    	//TestPutDone(test_surls_put,output_put.token);
        TestAbortRequest(output_put.token);
     	TestAbortRequest(output_get.token);


    }
}
void TestLs();
void Test();
int main(void)
{
	//TestPrepareToPutPrepareToGet();
	//TestLs();

	Test();
   //printf("%s \n",filestatuses->surl);
	return EXIT_SUCCESS;
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

void TestLs()
{
	int i;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1","srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
	struct srm_context context;
	struct srm_ls_input input_ls;
	struct srm_ls_output output_ls;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_ls.nbfiles = 2;
	input_ls.count = 0;
	input_ls.numlevels  = 1;
	input_ls.surls = test_surls_ls;
	input_ls.offset = 0;

//	system("lcg-ls --verbose --nobdii -D srmv2 srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1");
	//int srm_ls_async(struct srm_context *context,
		//	struct srm_ls_input *input,struct srm_ls_output *output)
    i = srm_ls_async(&context,&input_ls,&output_ls);

	if (i > 0)
	{
		PrintResult(output_ls.statuses);
	}
}

void Test()
{
	struct srm_context context;
	struct srm_ls_input input_ls;
	struct srm_ls_output* output_ls;
	struct srm_rm_input input_rm;
	struct srm_rm_output* output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output* output_rmdir;
	struct srm_mkdir_input input_mkdir;
	int i,j;
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	char *test_surls_rm[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba",
							 "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba2"};


	char *test_surl_dir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/3/4";
	char *test_surl_rmdir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2";

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;





	TestLs();

	input_mkdir.dir_name = test_surl_dir;
    i = srm_mkdir(&context,&input_mkdir);
    printf("Mkdir:%s %d \n",input_mkdir.dir_name,i);


    TestLs();
	input_rmdir.recursive = 1;
	input_rmdir.surl = test_surl_rmdir;
	i = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	printf("Remove dir:%s %d\n",input_rmdir.surl,i);

	TestLs();

	system("lcg-cp --verbose --nobdii -D srmv2 --vo dteam file:///etc/group srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba");

	input_rm.nbfiles = 2;
	input_rm.surls = test_surls_rm;
	i = srm_rm(&context,&input_rm,&output_rm);
	for(j=0;j<i;j++)
	{
		printf("Remove files:%s\n",input_rm.surls[j],i);
	}

	TestLs();
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
