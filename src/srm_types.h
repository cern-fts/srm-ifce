#ifndef _SRM_TYPES_H
#define _SRM_TYPES_H

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "gfal_types.h"


enum srm_file_locality_ {srm_file_locality_ONLINE = 0, srm_file_locality_NEARLINE = 1, srm_file_locality_ONLINE_USCOREAND_USCORENEARLINE = 2, srm_file_locality_LOST = 3, srm_file_locality_NONE = 4, srm_file_locality_UNAVAILABLE = 5};

typedef enum srm_file_locality_ srm_file_locality ;

enum srm_call_status_ {srm_call_status_SUCCESS = 0,srm_call_status_FAILURE = 1,srm_call_status_TIMEOUT = 2,srm_call_status_QUEUED = 3,srm_call_status_INTERNAL_ERROR = 4};

typedef enum srm_call_status_ srm_call_status;

typedef struct srm_context {
	enum se_type		version;
	char *				srm_endpoint;
	char * 				errbuf;
	int 				errbufsz;
	int 				verbose;
	int 				timeout;
};

typedef struct srm_internal_context {
	time_t 						end_time;
	struct srm2__TReturnStatus *retstatus;
	char 						*token;
	int 						attempt;
};

typedef struct srm_ls_input{
	int nbfiles;
	char **surls; // null terminated array of strings
	int numlevels;
	int offset;
	int count;
};


#if ! defined(linux) || defined(_LARGEFILE64_SOURCE)
struct srm_ls_output {
	char 	*surl;
	struct stat64	stat;
	int 	status;
	char 	*explanation;
	struct srm_ls_output *subpaths;
	int nbsubpaths;
	srm_file_locality locality;
	char *checksumtype;
	char *checksum;
    char **spacetokens;
    int nbspacetokens;
};
#endif

typedef struct srm_rm_input{
	int nbfiles;
	char **surls; // null terminated array of strings
};

struct srm_rm_output {
	char 	*surl;
	char 	*turl;
	int 	status;
	char 	*explanation;
};

typedef struct srm_rmdir_input{
	int recursive;
	char *surl;
};

struct srm_rmdir_output {
	char 	*surl;
	char 	*turl;
	int 	status;
	char 	*explanation;
};

typedef struct srm_mkdir_input{
	char 	*dir_name;
};

typedef struct srm_preparetoget_input{
	int nbfiles;
	char **surls;
	int desiredpintime;
	char *spacetokendesc;
	char **protocols;
	char **reqtoken;
};

struct srm_preparetoget_output {
	char 	*surl;
	char 	*turl;
	int 	status;
	char 	*explanation;
	int 	pinlifetime;
};

typedef struct srm_getspacetokens_input
{
	char *spacetokendesc;
};
struct srm_getspacetokens_output
{
	int nbtokens;
	char ***spacetokens;
};

#endif /* _SRM_TYPES_H */
