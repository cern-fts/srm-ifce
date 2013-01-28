/*
 * Copyright (c) Members of the EGEE Collaboration. 2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Todor Manev  IT-GT CERN
 */
#ifndef _SRM_TYPES_H
#define _SRM_TYPES_H


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <glib.h>


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
#define SRM_LONG64 gint64
#endif


#define SRM_SIZE_MARGIN          1048576     // 1MB

typedef struct srm_context* srm_context_t;

typedef struct srm_context_extension* srm_context_extension_t;

enum TFileLocality_
{
	GFAL_LOCALITY_UNKNOWN = 0,
	GFAL_LOCALITY_ONLINE_,	///< xs:string value="ONLINE"
	GFAL_LOCALITY_NEARLINE_,	///< xs:string value="NEARLINE"
	GFAL_LOCALITY_ONLINE_USCOREAND_USCORENEARLINE,	///< xs:string value="ONLINE_AND_NEARLINE"
	GFAL_LOCALITY_LOST,	///< xs:string value="LOST"
	GFAL_LOCALITY_NONE_,	///< xs:string value="NONE"
	GFAL_LOCALITY_UNAVAILABLE	///< xs:string value="UNAVAILABLE"
};
/// Typedef synonym for enum ns1__TFileLocality.
typedef enum TFileLocality_ TFileLocality;

enum TRetentionPolicy_
{
	GFAL_POLICY_UNKNOWN = 0,
	GFAL_POLICY_REPLICA,
	GFAL_POLICY_OUTPUT,
	GFAL_POLICY_CUSTODIAL
};
/// Typedef synonym for enum ns1__TRetentionPolicy.
typedef enum TRetentionPolicy_ TRetentionPolicy;

enum TAccessLatency_
{
	GFAL_LATENCY_UNKNOWN = 0,
	GFAL_LATENCY_ONLINE,
	GFAL_LATENCY_NEARLINE
};
/// Typedef synonym for enum ns1__TAccessLatency.
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

enum srm_version
{
	VERSION_1,
	VERSION_2_2,
};

enum srm_call_status_
{
	srm_call_status_SUCCESS = 0,
	srm_call_status_FAILURE = 1,
	srm_call_status_TIMEOUT = 2,
	srm_call_status_QUEUED = 3,
	srm_call_status_INTERNAL_ERROR = 4
};

typedef enum srm_call_status_ srm_call_status;

struct srm_filestatus {
	char	*surl;
	char	*turl;
	int		fileid;
	int		status;
};

struct srmv2_pinfilestatus
{
	char 	*surl; // surl of the file
	char 	*turl; // turl of the file
	int 	status; // status code
	char 	*explanation; // string for the status
	int 	pinlifetime; // pin lifetime
	int		estimated_wait_time;
};

struct srmv2_filestatus
{
	char 	*surl; // surl of the file
	char 	*turl; // turl of the file
	int 	status; // status code
	char 	*explanation; // string for the status
};

struct srm_context
{
	enum srm_version    version;
	char *				srm_endpoint;
	char * 				errbuf;
	int 				errbufsz;
	int 				verbose;
    int 				timeout;        // global timeout for asynchronous operations
    int                 timeout_conn;   // global timeout for SOAP connection
    int                 timeout_ops;    // global timeout for response on operation ( send/receive )
    srm_context_extension_t ext;
    char                keep_alive;
    void*               reserved[23];   // reserved field for futur usage
};

struct srm_internal_context
{
	srm_call_status 			current_status;
	time_t 						end_time;
    struct timespec             end_time_spec;
    struct timespec             current_waittime_spec;
	int 						attempt;
	int							estimated_wait_time; //remainingTotalRequestTime in srm documentation
    unsigned int                random_seed;
};

struct srm_ls_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	int numlevels;
	int *offset;
	int count;
};

struct srm_ping_output
{
	char *versioninfo; // srm server version information
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
	TFileLocality locality;
	char *checksumtype;
	char *checksum;
    char **spacetokens;
    int nbspacetokens;
};
#endif

struct srm_ls_output
{
	struct srmv2_mdfilestatus   *statuses;
	char 						*token; // request token
	struct srm2__TReturnStatus  *retstatus;
    int                         statuses_num;
};

struct srm_rm_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
};

struct srm_rm_output
{
	struct srm2__TReturnStatus  *retstatus; // status of the srm call
	struct srmv2_filestatus 	*statuses; // returned file statuses
};

struct srm_rmdir_input
{
	int recursive; // recursive(1) or not(0)
	char *surl; // directory surl
};

struct srm_rmdir_output
{
	struct srm2__TReturnStatus  *retstatus; // status of the srm call
	struct srmv2_filestatus 	*statuses;
};

struct srm_mkdir_input
{
	char 	*dir_name; // directory surl
};

struct srm_preparetoget_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
    int desiredpintime; // desired pin time
    char *spacetokendesc; // space token descriptor
    char **protocols; // protocols used
};

struct srm_preparetoput_input
{
	SRM_LONG64 *filesizes; // filesize
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	int desiredpintime; // desired pin time
	char *spacetokendesc; // space token descriptor
	char **protocols;// protocols used
};

struct srm_preparetoget_output
{
	char 						*token; // request token
	struct srm2__TReturnStatus  *retstatus; // status of the srm call
	struct srmv2_pinfilestatus *filestatuses; // returned statuses
};

struct srm_preparetoput_output
{
	char 						*token;// request token
	struct srm2__TReturnStatus  *retstatus; // status of the srm call
	struct srmv2_pinfilestatus  *filestatuses;// returned statuses
};

struct srm_putdone_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	char *reqtoken; // request token
};

struct srm_releasefiles_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	char *reqtoken; // request token
};

struct srm_abort_files_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	char *reqtoken; // request token
};

struct srm_bringonline_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	int desiredpintime; // desired pin time
	char *spacetokendesc; // space token descriptor
	char **protocols; // protocols used
};

struct srm_bringonline_output
{
	char 						*token;// request token
	struct srm2__TReturnStatus  *retstatus;// status of the srm call
	struct srmv2_pinfilestatus  *filestatuses;// returned statuses
};

struct srm_getspacetokens_input
{
	char *spacetokendesc; // space token descriptor
};
struct srm_getspacetokens_output
{
	int nbtokens; // number of space tokens in the array
	char **spacetokens; // array of space tokens
};

struct srm_getspacemd_input
{
	int nbtokens; // number of space tokens in the array
	char **spacetokens; // array of space tokens
};

struct srm_getbestspacetokens_input
{
	char *spacetokendesc; // space token descriptor
	SRM_LONG64 neededsize; // required space
};
struct srm_permission
{
	char *name_id; // name id string
	TPermissionMode mode; // permission mode
};

struct srm_setpermission_input
{
	char *surl;  // surl to set the permission properties for
	TPermissionType permission_type; // add/change/remove
	TPermissionMode owner_permission; // owner permission
	TPermissionMode other_permission; // other users permission
	int user_permissions_count; // user permissions size of array
	struct srm_permission *user_permissions; // array of user permissions
	int group_permissions_count; // group permissions size of array
	struct srm_permission *group_permissions; // array of group permissions
};

struct srm_getpermission_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
};

struct srm_checkpermission_input
{
	int amode; 
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
};
struct srm_extendfilelifetime_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	char *reqtoken; // request token
	int pintime; // pin time 
};

struct srm_extendfilelifetime_output
{
	struct srm2__TReturnStatus  *retstatus;// status of the srm call
	struct srmv2_pinfilestatus  *filestatuses;// returned statuses
};

struct srm_filepermission
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

struct srm_getpermission_output
{
	struct srm2__TReturnStatus  *retstatus;// status of the srm call
	struct srm_filepermission 	*permissions;// array of file permissions
};


struct srm_spacemd
{
	char *spacetoken; // space token string
	char *owner; // name of the owner
	SRM_LONG64 totalsize; // total size of the space 
	SRM_LONG64 guaranteedsize;  // guaranteed size of the space
	SRM_LONG64 unusedsize; // unused space
	int lifetimeassigned; // life time assigned to the space
	int lifetimeleft; // total time left
	TRetentionPolicy retentionpolicy;
	TAccessLatency accesslatency;
};

struct srm_reservespace_input
{
	char *spacetokendescriptor; // space token descriptor
	int desired_lifetime; // desired lifetime
	SRM_LONG64 desired_size; // desired size
};

struct srm_reservespace_output
{
	struct srm2__TReturnStatus  *retstatus;
	char *spacetoken; // space token
	int lifetime; // lifetime of the space token
	SRM_LONG64 size_total; // total size of the space
	SRM_LONG64 size_guaranteed; // guaranteed size of the space
};

struct srm_purgefromspace_input
{
	int nbfiles; // number of surls in the array
	char **surls; // array of surls
	char *spacetoken; // space token
};

struct srm_purgefromspace_output
{
	struct srm2__TReturnStatus  *retstatus; // status of the srm call
	struct srmv2_filestatus 	*statuses;
};

#endif /* _SRM_TYPES_H */


