GSOAP_LOCATION=/home/tmanev/workspace/lcg_util/repository/externals/gsoap/2.7.6b/sl5_x86_64_gcc412
CGSI_LOCATION=/home/tmanev/workspace/lcg_util/org.glite.security.cgsi-gsoap-2.7/src
#/home/tmanev/workspace/lcg_util/stage/include 

SOURCE_FOLDER=src
BUILD_FOLDER=build
CC=gcc
LD=ld
LDFLAGS=-shared

INCLUDE=-I $(GSOAP_LOCATION) \
		-I $(GSOAP_LOCATION)/include \
		-I $(GSOAP_LOCATION)/extras \
		-I $(SOURCE_FOLDER) \
		-I $(CGSI_LOCATION)

DEFINES=-D_LARGEFILE64_SOURCE \
		-D_GSOAP_VERSION=0x020706  \
		-D_GSOAP_WSDL2H_VERSION=0x020706 \
		-DUSEGSOAP_2_6 \
		-DUSEGSOAPWSDL2H_2_6 \
		-DGFAL_SECURE

LIBRARIES=-lcgsi_plugin_gsoap_2.7 -lglobus_gss_assist_gcc64dbg -lglobus_gssapi_gsi_gcc64dbg -lglobus_gsi_proxy_core_gcc64dbg -lglobus_gsi_proxy_core_gcc64dbg -lglobus_gsi_credential_gcc64dbg -lglobus_gsi_callback_gcc64dbg -lglobus_oldgaa_gcc64dbg -lglobus_gsi_sysconfig_gcc64dbg -lglobus_gsi_cert_utils_gcc64dbg -lglobus_proxy_ssl_gcc64dbg -lglobus_common_gcc64dbg -lltdl_gcc64dbg -lglobus_callout_gcc64dbg -lglobus_openssl_error_gcc64dbg -lglobus_openssl_gcc64dbg
LIBRARIES_FOLDER=-L/home/tmanev/workspace/lcg_util/stage/lib64/ -L/home/tmanev/workspace/lcg_util/stage/lib64/ -L/home/tmanev/workspace/lcg_util/repository/vdt/globus/4.0.7-VDT-1.10.1/sl5_x86_64_gcc412/lib/
CFLAGS=-ggdb -fPIC $(INCLUDE) $(DEFINES) # $(LIBRARIES) $(LIBRARIES_FOLDER)
C2FLAGS=-ggdb $(INCLUDE) $(DEFINES) # $(LIBRARIES) $(LIBRARIES_FOLDER)
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
 

all:
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
#	$(LD) $(LDFLAGS)
#    $(CC) $(CFLAGS) -c  $(BUILD_FOLDER)/stdsoap2.o 
	ar rc $(BUILD_FOLDER)/libsrm.a  $(BUILD_FOLDER)/stdsoap2.o \
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
						$(BUILD_FOLDER)/srmv2_data_transfer_functions.o
	ranlib $(BUILD_FOLDER)/libsrm.a
	$(LD) $(LIBRARIES_FOLDER) $(LIBRARIES) -r $(BUILD_FOLDER)/stdsoap2.o \
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
 					 -o $(BUILD_FOLDER)/libsrm.o
	$(CC) -shared $(LIBRARIES_FOLDER) $(LIBRARIES) $(BUILD_FOLDER)/stdsoap2.o \
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
   
	$(CC) $(C2FLAGS) $(SOURCE_FOLDER)/srm_unittest.c $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm -L$(BUILD_FOLDER)/ -o $(BUILD_FOLDER)/srm_unittest # -static -lpthread
	$(CC) $(C2FLAGS) $(SOURCE_FOLDER)/srm_test.c     $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm -L$(BUILD_FOLDER)/ -o $(BUILD_FOLDER)/srm_test # -static -lpthread
	$(CC) $(C2FLAGS) $(SOURCE_FOLDER)/srm_testunittest.c -lsrm $(LIBRARIES_FOLDER) $(LIBRARIES) -L$(BUILD_FOLDER)/ -o $(BUILD_FOLDER)/srm_testunittest # -static -lpthread
clean:
	rm -f $(BUILD_FOLDER)/*.o
	rm -f $(BUILD_FOLDER)/*.so

