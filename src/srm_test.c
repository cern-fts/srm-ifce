#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <errno.h>
#include "srm_types.h"
#include "srm_ifce.h"

void PrintResult(struct srmv2_mdfilestatus* output);
void TestLs();

int main(void)
{
	/*int i;
	char *test_surls_get[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/test14"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
	struct srm_context context;
	struct srm_preparetoget_input input_get;
	struct srmv2_pinfilestatus *filestatuses;

	context.verbose = 1;
	context.errbufsz = 0;
	context.srm_endpoint = test_srm_endpoint;
	context.timeout = 3600;
	context.version = TYPE_SRMv2;

	input_get.nbfiles = 1;
	input_get.desiredpintime = 1000;
	input_get.surls = test_surls_get;




    i = srm_prepeare_to_get(&context,&input_get,&filestatuses);*/

	TestLs();

   //printf("%s \n",filestatuses->surl);
	return EXIT_SUCCESS;
}
void TestLs()
{
	int i;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";
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
    i = srm_ls_async(&context,&input_ls,&output_ls);

	if (!i)
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
	int i;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	char *test_surls_rm[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba",
							 "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/proba"};


	char *test_surl_dir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/3/4";
	char *test_surl_rmdir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2";

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

	input_rm.nbfiles = 2;
	input_rm.surls = test_surls_rm;

	input_rmdir.recursive = 1;
	input_rmdir.surl = test_surl_rmdir;

	input_mkdir.dir_name = test_surl_dir;

    i = srm_ls(&context,&input_ls,&output_ls);

    if (!i)
	{
		PrintResult(output_ls);
	}

    i = srm_mkdir(&context,&input_mkdir);
    printf("Mkdir:%s %d \n",input_mkdir.dir_name,i);

    i = srm_ls(&context,&input_ls,&output_ls);

	if (!i)
	{
		PrintResult(output_ls);
	}

	i = srm_rmdir(&context,&input_rmdir,&output_rmdir);
	printf("Remove dir:%s %d\n",input_rmdir.surl,i);

    i = srm_ls(&context,&input_ls,&output_ls);

	if (!i)
	{
		PrintResult(output_ls);
	}

    /*system("lcg-cp --verbose --nobdii -D srmv2 --vo dteam file:///etc/group srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba");
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
