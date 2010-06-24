GSOAP_LOCATION=/home/tmanev/workspace/lcg_util/repository/externals/gsoap/2.7.6b/sl5_x86_64_gcc412

SOURCE_FOLDER=src
BUILD_FOLDER=build
CC=gcc
LD=ld
LDFLAGS=-shared

INCLUDE=-I $(GSOAP_LOCATION) \
		-I $(GSOAP_LOCATION)/include \
		-I $(GSOAP_LOCATION)/extras \
		-I $(SOURCE_FOLDER)

DEFINES=-D_LARGEFILE64_SOURCE \
		-D_GSOAP_VERSION=0x020706  \
		-D_GSOAP_WSDL2H_VERSION=0x020706 \
		-DUSEGSOAP_2_6 \
		-DUSEGSOAPWSDL2H_2_6 

LIBRARIES=-lc
CFLAGS=-ggdb -fPIC $(INCLUDE) $(DEFINES)
#    stdsoap2.c \
#    srmv2C.c \
#    srmv2Client.c \
#    srmSoapBinding.c \
#	srm_dependencies.c \
#	srm_version_wrapper.c \
#	srm_util.c \
#	srmv2_directory_functions.c \
#	srmv2_sync_wrapper.c \
#	srmv2_async_wrapper.c \
#	srmv2_space_management_functions.c \
#	srmv2_discovery_functions.c \
#	srmv2_permission_functions.c \
#	srmv2_data_transfer_functions.c
#srm_unittest_SOURCES = srm_unittest.c
#srm_unittest_LDADD = libsrm.so  
#srm_unittest_CFLAGS = $(SRM_FLAGS) $(GSOAP_CFLAGS) $(CGSI_GSOAP_CFLAGS) -D_LARGEFILE64_SOURCE
 

all: $(SOURCE_FOLDER)/stdsoap2.c  $(SOURCE_FOLDER)/srmv2C.c  $(SOURCE_FOLDER)/srmSoapBinding.c  $(SOURCE_FOLDER)/srmv2Client.c $(SOURCE_FOLDER)/srm_dependencies.c $(SOURCE_FOLDER)/srm_version_wrapper.c $(SOURCE_FOLDER)/srm_util.c $(SOURCE_FOLDER)/srmv2_directory_functions.c $(SOURCE_FOLDER)/srmv2_sync_wrapper.c $(SOURCE_FOLDER)/srmv2_async_wrapper.c $(SOURCE_FOLDER)/srmv2_space_management_functions.c $(SOURCE_FOLDER)/srmv2_discovery_functions.c $(SOURCE_FOLDER)/srmv2_permission_functions.c $(SOURCE_FOLDER)/srmv2_data_transfer_functions.c
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/stdsoap2.c -o $(BUILD_FOLDER)/stdsoap2.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2C.c -o $(BUILD_FOLDER)/srmv2C.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmSoapBinding.c -o $(BUILD_FOLDER)/srmSoapBinding.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2Client.c -o $(BUILD_FOLDER)/srmv2Client.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srm_dependencies.c -o $(BUILD_FOLDER)/srm_dependencies.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srm_version_wrapper.c -o $(BUILD_FOLDER)/srm_version_wrapper.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srm_util.c -o $(BUILD_FOLDER)/srm_util.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_directory_functions.c -o $(BUILD_FOLDER)/srmv2_directory_functions.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_sync_wrapper.c -o $(BUILD_FOLDER)/srmv2_sync_wrapper.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_async_wrapper.c -o $(BUILD_FOLDER)/srmv2_async_wrapper.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_space_management_functions.c -o $(BUILD_FOLDER)/srmv2_space_management_functions.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_discovery_functions.c -o $(BUILD_FOLDER)/srmv2_discovery_functions.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_permission_functions.c -o $(BUILD_FOLDER)/srmv2_permission_functions.o
	$(CC) $(CFLAGS) -c $(SOURCE_FOLDER)/srmv2_data_transfer_functions.c -o $(BUILD_FOLDER)/srmv2_data_transfer_functions.o
	$(LD) $(LDFLAGS)    $(BUILD_FOLDER)/stdsoap2.o \
						$(BUILD_FOLDER)/srmv2C.o \
						$(BUILD_FOLDER)/srmSoapBinding.o \
						$(BUILD_FOLDER)/srmv2Client.o \
						$(BUILD_FOLDER)/srm_dependencies.o \
						$(BUILD_FOLDER)/srm_version_wrapper.o \
						$(BUILD_FOLDER)/srm_util.o \
						$(BUILD_FOLDER)/srmv2_directory_functions.o \
						$(BUILD_FOLDER)/srmv2_sync_wrapper.o \
						$(BUILD_FOLDER)/srmv2_async_wrapper.o \
						$(BUILD_FOLDER)/srmv2_space_management_functions.o \
						$(BUILD_FOLDER)/srmv2_discovery_functions.o \
						$(BUILD_FOLDER)/srmv2_permission_functions.o \
						$(BUILD_FOLDER)/srmv2_data_transfer_functions.o \
					-o $(BUILD_FOLDER)/libsrm.so
	$(CC) $(DEFINES) $(SOURCE_FOLDER)/srm_unittest.c -lcheck -lsrm -L$(BUILD_FOLDER)/ -o $(BUILD_FOLDER)/srm_unittest
	$(CC) $(DEFINES) $(SOURCE_FOLDER)/srm_test.c     -lcheck -lsrm -L$(BUILD_FOLDER)/ -o $(BUILD_FOLDER)/srm_test

#	$(CC) $(CFLAGS) -c database.c -o database.o
#	$(CC) $(CFLAGS) -c modules/databases/file_fuzz/p_file_fuzz.c -c -o modules/databases/file_fuzz/p_file_fuzz.o
#	$(CC) $(CFLAGS) -c modules/databases/mode_fuzz/p_mode_fuzz.c -c -o modules/databases/mode_fuzz/p_mode_fuzz.o
#	$(CC) $(CFLAGS) -c modules/databases/path_fuzz/p_path_fuzz.c -c -o modules/databases/path_fuzz/p_path_fuzz.o
#	$(CC) $(CFLAGS) -c modules/templates/create_template.c -c -o modules/templates/create_template.o
#	$(CC) $(CFLAGS) -c modules/parsing_config_file/parse.c -c -o modules/parsing_config_file/parse.o
#	$(CC) $(CFLAGS) -c modules/compile/compile_module.c -c -o modules/compile/compile_module.o
#	$(CC) $(CFLAGS) -c modules/run_tests/run_module.c -c -o modules/run_tests/run_module.o
#	$(CC) $(CFLAGS) -c modules/log/log_me.c -c -o modules/log/log_me.o
#	$(CC) $(CFLAGS) database.o modules/databases/file_fuzz/p_file_fuzz.o modules/databases/mode_fuzz/p_mode_fuzz.o \
#	                modules/databases/path_fuzz/p_path_fuzz.o modules/templates/create_template.o \
#	                modules/parsing_config_file/parse.o modules/log/log_me.o modules/compile/compile_module.o \
#	                pi3fuzz.c -o pi3fuzz
#	$(CC) $(CFLAGS) modules/run_tests/run_module.o modules/log/log_me.o pi3run.c -o pi3run

clean:
	rm -f $(BUILD_FOLDER)/stdsoap2.os
