#ifndef _SRM_TYPES_H
#define _SRM_TYPES_H

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>


#ifndef SRM_LONG64_FORMAT
#if defined(__ia64__) || defined(__x86_64)
#define SRM_LONG64_FORMAT "%ld"
#elif defined(_WIN32)
#define SRM_LONG64_FORMAT "%I64d"
#else
#define SRM_LONG64_FORMAT "%lld"
#endif
#endif
#ifndef SRM_LONG64
#if defined(__ia64__) || defined(__x86_64)
#define SRM_LONG64 long
#elif defined(_WIN32)
#define SRM_LONG64 __i64
#else
#define SRM_LONG64 long long
#endif
#endif

/*
// used to check wether there is enough space on the SE
#define GFAL_NEWFILE_SIZE         1024

#define GFAL_OPEN_MAX             1024
#define GFAL_DEFAULT_CATALOG_TYPE "lfc"
#define GFAL_FQAN_MAX             64
#define GFAL_GUID_LEN             37
#define GFAL_VO_MAXLEN            255
#define GFAL_FQAN_MAXLEN          511
#define GFAL_HOSTNAME_MAXLEN      255
#define GFAL_PATH_MAXLEN          1024
#define GFAL_ERRMSG_LEN           1024
#define GFAL_BDII_TIMEOUT         60*/
#define SRM_SIZE_MARGIN          1048576     // 1MB
/*#define GFAL_SRM_MAXRETRIES       10

// GFAL error level for gfal_errmsg
#define GFAL_ERRLEVEL_ERROR       0
#define GFAL_ERRLEVEL_WARN        1
#define GFAL_ERRLEVEL_INFO        2

#define GFAL_VERBOSE_QUIET       -1    // API-mode, no messages is printed
#define GFAL_VERBOSE_NORMAL       0    // only errors are printed
#define GFAL_VERBOSE_VERBOSE      1    // a bit more verbose information is printed
#define GFAL_VERBOSE_DEBUG        2    // extra information is printed

#define GFAL_CKSM_DEFAULT         GFAL_CKSM_ADLER32   // MD5 is the default algo
*/

enum TRetentionPolicy_
{
	SRM_POLICY_UNKNOWN = 0,
	SRM_POLICY_REPLICA,
	SRM_POLICY_OUTPUT,
	SRM_POLICY_CUSTODIAL
};
typedef enum TRetentionPolicy_ TRetentionPolicy;

enum TAccessLatency_
{
	SRM_LATENCY_UNKNOWN = 0,
	SRM_LATENCY_ONLINE,
	SRM_LATENCY_NEARLINE
};
typedef enum TAccessLatency_ TAccessLatency;

enum TPermissionType_
{
	SRM_PERMISSION_ADD = 0,
	SRM_PERMISSION_REMOVE = 1,
	SRM_PERMISSION_CHANGE = 2
};

typedef enum TPermissionType_ TPermissionType;

enum TPermissionMode_
{
	SRM_PERMISSION_NONE = 0,
	SRM_PERMISSION_X = 1,
	SRM_PERMISSION_W = 2,
	SRM_PERMISSION_WX = 3,
	SRM_PERMISSION_R = 4,
	SRM_PERMISSION_RX = 5,
	SRM_PERMISSION_RW = 6,
	SRM_PERMISSION_RWX = 7
};

typedef enum TPermissionMode_ TPermissionMode;

enum se_type
{
	TYPE_NONE = 0,
	TYPE_SRM,
	TYPE_SRMv2,
	TYPE_SE
};

enum srm_file_locality_
{
	srm_file_locality_ONLINE = 0,
	srm_file_locality_NEARLINE = 1,
	srm_file_locality_ONLINE_USCOREAND_USCORENEARLINE = 2,
	srm_file_locality_LOST = 3,
	srm_file_locality_NONE = 4,
	srm_file_locality_UNAVAILABLE = 5
};

typedef enum srm_file_locality_ srm_file_locality ;

enum srm_call_status_
{
	srm_call_status_SUCCESS = 0,
	srm_call_status_FAILURE = 1,
	srm_call_status_TIMEOUT = 2,
	srm_call_status_QUEUED = 3,
	srm_call_status_INTERNAL_ERROR = 4
};

typedef enum srm_call_status_ srm_call_status;

struct srmv2_pinfilestatus
{
	char 	*surl;
	char 	*turl;
	int 	status;
	char 	*explanation;
	int 	pinlifetime;
};

struct srmv2_filestatus
{
	char 	*surl;
	char 	*turl;
	int 	status;
	char 	*explanation;
};

typedef struct srm_context
{
	enum se_type		version;
	char *				srm_endpoint;
	char * 				errbuf;
	int 				errbufsz;
	int 				verbose;
	int 				timeout;
};

typedef struct srm_internal_context
{
	srm_call_status 			current_status;
	time_t 						end_time;
	int 						attempt;
	int							estimated_wait_time;
};

typedef struct srm_ls_input
{
	int nbfiles;
	char **surls; // null terminated array of strings
	int numlevels;
	int *offset;
	int count;
};

typedef struct srm_ping_output
{
	char *versioninfo;
};

#if ! defined(linux) || defined(_LARGEFILE64_SOURCE)
struct srm_mdfilestatus
{
	char 	*surl;
	struct stat64	stat;
	int		fileid;
	int 	status;
};

struct srmv2_mdfilestatus
{
	char 	*surl;
	struct stat64	stat;
	int 	status;
	char 	*explanation;
	struct srmv2_mdfilestatus *subpaths;
	int nbsubpaths;
	srm_file_locality locality;
	char *checksumtype;
	char *checksum;
    char **spacetokens;
    int nbspacetokens;
};
#endif

typedef struct srm_ls_output
{
	struct srmv2_mdfilestatus   *statuses;
	char 						*token;
	struct srm2__TReturnStatus  *retstatus;
};

typedef struct srm_rm_input{
	int nbfiles;
	char **surls; // null terminated array of strings
};

typedef struct srm_rm_output
{
	struct srm2__TReturnStatus  *retstatus;
	struct srmv2_filestatus 	*statuses;
};

typedef struct srm_rmdir_input
{
	int recursive;
	char *surl;
};

typedef struct srm_rmdir_output
{
	struct srm2__TReturnStatus  *retstatus;
};

typedef struct srm_mkdir_input
{
	char 	*dir_name;
};

typedef struct srm_preparetoget_input
{
    int nbfiles;
    char **surls;
    int desiredpintime;
    char *spacetokendesc;
    char **protocols;
};

typedef struct srm_preparetoput_input
{
	SRM_LONG64 *filesizes;
	int nbfiles;
	char **surls;
	int desiredpintime;
	char *spacetokendesc;
	char **protocols;
};

typedef struct srm_preparetoget_output
{
	char 						*token;
	struct srm2__TReturnStatus  *retstatus;
	struct srmv2_pinfilestatus *filestatuses;
};

typedef struct srm_preparetoput_output
{
	char 						*token;
	struct srm2__TReturnStatus  *retstatus;
	struct srmv2_pinfilestatus  *filestatuses;
};

typedef struct srm_putdone_input{
	int nbfiles;
	char **surls;
	char *reqtoken;
};

typedef struct srm_releasefiles_input
{
	int nbfiles;
	char **surls;
	char *reqtoken;
};

typedef struct srm_abort_files_input
{
	int nbfiles;
	char **surls;
	char *reqtoken;
};

typedef struct srm_bringonline_input
{
	int nbfiles;
	char **surls;
	int desiredpintime;
	char *spacetokendesc;
	char **protocols;
};

typedef struct srm_bringonline_output
{
	char 						*token;
	struct srm2__TReturnStatus  *retstatus;
	struct srmv2_pinfilestatus  *filestatuses;
};

typedef struct srm_getspacetokens_input
{
	char *spacetokendesc;
};
struct srm_getspacetokens_output
{
	int nbtokens;
	char **spacetokens;
};

typedef struct srm_getspacemd_input
{
	int nbtokens;
	char **spacetokens;
};

typedef struct srm_getbestspacetokens_input
{
	char *spacetokendesc;
	SRM_LONG64 neededsize;
};
typedef struct srm_permission
{
	char *name_id;
	TPermissionMode mode;
};

typedef struct srm_setpermission_input
{
	char *surl;
	TPermissionType permission_type;
	TPermissionMode owner_permission;
	TPermissionMode other_permission;
	int user_permissions_count;
	struct srm_permission *user_permissions;
	int group_permissions_count;
	struct srm_permission *group_permissions;
};

typedef struct srm_getpermission_input
{
	int nbfiles;
	char **surls;
};

typedef struct srm_checkpermission_input
{
	int amode;
	int nbfiles;
	char **surls;
};

typedef struct srm_filepermission
{
	char 	*surl;
	int 	status;
	char 	*explanation;
	char 	*owner;
	TPermissionMode owner_permission;
	TPermissionMode other_permission;
	int user_permissions_count;
	struct srm_permission *user_permissions;
	int group_permissions_count;
	struct srm_permission *group_permissions;
};

typedef struct srm_getpermission_output
{
	struct srm2__TReturnStatus  *retstatus;
	struct srm_filepermission 	*permissions;
};


typedef struct srm_spacemd{
	char *spacetoken;
	char *owner;
	SRM_LONG64 totalsize;
	SRM_LONG64 guaranteedsize;
	SRM_LONG64 unusedsize;
	int lifetimeassigned;
	int lifetimeleft;
	TRetentionPolicy retentionpolicy;
	TAccessLatency accesslatency;
};

typedef struct srm_reservespace_input
{
	char *spacetokendescriptor;
	int desired_lifetime;
	unsigned int desired_size;
};

typedef struct srm_reservespace_output
{
	struct srm2__TReturnStatus  *retstatus;
	char *spacetoken;
	int lifetime;
	unsigned int size_total;
	unsigned int size_guaranteed;
};

#endif /* _SRM_TYPES_H */


