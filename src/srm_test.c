#include <stdio.h>
#include <check.h>
#include <stdlib.h>
#include <errno.h>
#include "srm_types.h"
#include "srm_ifce.h"

void PrintResult(struct srm_ls_output* output);


int main(void) {

	struct srm_context context;
	struct srm_ls_input input_ls;
	struct srm_ls_output* output_ls;
	struct srm_rm_input input_rm;
	struct srm_rm_output* output_rm;
	struct srm_rmdir_input input_rmdir;
	struct srm_rmdir_output* output_rmdir;
	int i;
	char *test_surls_ls[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1"};
	char *test_srm_endpoint =  "httpg://lxbra1910.cern.ch:8446/srm/managerv2";

	char *test_surls_rm[] = {"srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba",
							 "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/proba"};


	char *test_surls_rmdir = "srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/";

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
	input_rmdir.surl = test_surls_rmdir;

    i = srm_ls(&context,&input_ls,&output_ls);

    if (!i)
	{
		PrintResult(output_ls);
	}

    system("lcg-cp --verbose --nobdii -D srmv2 --vo dteam file:///etc/group srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/proba");
    system("lcg-cp --verbose --nobdii -D srmv2 --vo dteam file:///etc/group srm://lxbra1910.cern.ch:8446/srm/managerv2?SFN=/dpm/cern.ch/home/dteam/1/2/proba");

    i = srm_rm(&context,&input_rm,&output_rm);
    printf("Remove:%d\n",i);
    i = srm_rmdir(&context,&input_rmdir,&output_rmdir);
    printf("Remove dir:%d\n",i);

    i = srm_ls(&context,&input_ls,&output_ls);

    if (!i)
	{
		PrintResult(output_ls);
	}

	return EXIT_SUCCESS;
}
void PrintResult(struct srm_ls_output* output)
{
	int i;
	printf("Directory: %s \n",output->surl);
	printf("Files:\n");
	for(i=0;i<output->nbsubpaths;i++)
	{
		printf("%s \n",output->subpaths[i].surl);
	}
}
