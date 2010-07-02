GSOAP_LOCATION=/home/tmanev/workspace/lcg_util/repository/externals/gsoap/2.7.6b/sl5_x86_64_gcc412
GLOBUS_LOCATION=/home/tmanev/workspace/lcg_util/repository/vdt/globus/4.0.7-VDT-1.10.1/sl5_x86_64_gcc412/lib/
CGSI_LOCATION=/home/tmanev/workspace/lcg_util/org.glite.security.cgsi-gsoap-2.7/src
VOMS_LOCATION=/home/tmanev/workspace/lcg_util/stage
#/home/tmanev/workspace/lcg_util/stage/include 

CC=gcc
LD=ld
LDFLAGS=-shared


# handling x86_64's lib64 directory
lib_m=lib
ifeq ($(arch), x86_64)
lib_m=lib64
endif

# link with woms library
ifeq ($(VOMS_LOCATION), $(EMPTY))
VOMS_LOCATION=/opt/glite
endif
VOMS_INCLUDES = -I$(VOMS_LOCATION)/include/ -DUSE_VOMS -DVIRTUAL_ID
VOMS_LDFLAGS = -L$(VOMS_LOCATION)/lib -L$(VOMS_LOCATION)/$(lib_m)
VOMS_LIBS = -lvomsc

GLOBUS_LDFLAGS = -L$(GLOBUS_LOCATION)/lib



GLOBUS_INCLUDES = -I$(GLOBUS_LOCATION)/include/$(GLOBUS_FLAVOUR)
INCLUDE=-I $(GSOAP_LOCATION) \
		-I $(GSOAP_LOCATION)/include \
		-I $(GSOAP_LOCATION)/extras \
		-I $(SRCDIR) \
		-I $(CGSI_LOCATION) \
		$(GLOBUS_INCLUDES)

DEFINES=-D_LARGEFILE64_SOURCE \
		-D_GSOAP_VERSION=0x020706  \
		-D_GSOAP_WSDL2H_VERSION=0x020706 \
		-DUSEGSOAP_2_6 \
		-DUSEGSOAPWSDL2H_2_6 \
		-DGFAL_SECURE

LIBRARIES=	-lcgsi_plugin_gsoap_2.7 \
			-lglobus_gss_assist_gcc64dbg \
			-lglobus_gssapi_gsi_gcc64dbg \
			-lglobus_gsi_proxy_core_gcc64dbg \
			-lglobus_gsi_proxy_core_gcc64dbg \
			-lglobus_gsi_credential_gcc64dbg \
			-lglobus_gsi_callback_gcc64dbg \
			-lglobus_oldgaa_gcc64dbg \
			-lglobus_gsi_sysconfig_gcc64dbg \
			-lglobus_gsi_cert_utils_gcc64dbg \
			-lglobus_proxy_ssl_gcc64dbg \
			-lglobus_common_gcc64dbg \
			-lltdl_gcc64dbg \
			-lglobus_callout_gcc64dbg \
			-lglobus_openssl_error_gcc64dbg \
			-lglobus_openssl_gcc64dbg 
LIBRARIES_FOLDER=-L/home/tmanev/workspace/lcg_util/stage/lib64/ \
				 -L$(GLOBUS_LOCATION) \
				 -L$(BUILDDIR)/ 

CFLAGS=-ggdb -fPIC $(INCLUDE) $(DEFINES)
C2FLAGS=-ggdb $(INCLUDE) $(DEFINES) 
 

all:
	$(CC) $(CFLAGS) -c $(SRCDIR)/stdsoap2.c -o $(BUILDDIR)/stdsoap2.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2C.c -o $(BUILDDIR)/srmv2C.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmSoapBinding.c -o $(BUILDDIR)/srmSoapBinding.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2Client.c -o $(BUILDDIR)/srmv2Client.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srm_dependencies.c -o $(BUILDDIR)/srm_dependencies.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srm_version_wrapper.c -o $(BUILDDIR)/srm_version_wrapper.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srm_util.c -o $(BUILDDIR)/srm_util.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_directory_functions.c -o $(BUILDDIR)/srmv2_directory_functions.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_sync_wrapper.c -o $(BUILDDIR)/srmv2_sync_wrapper.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_async_wrapper.c -o $(BUILDDIR)/srmv2_async_wrapper.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_space_management_functions.c -o $(BUILDDIR)/srmv2_space_management_functions.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_discovery_functions.c -o $(BUILDDIR)/srmv2_discovery_functions.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_permission_functions.c -o $(BUILDDIR)/srmv2_permission_functions.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/srmv2_data_transfer_functions.c -o $(BUILDDIR)/srmv2_data_transfer_functions.o
#	$(LD) $(LDFLAGS)
#    $(CC) $(CFLAGS) -c  $(BUILDDIR)/stdsoap2.o 
	ar rc $(BUILDDIR)/libsrm.a  $(BUILDDIR)/stdsoap2.o \
						$(BUILDDIR)/srmv2C.o \
						$(BUILDDIR)/srmSoapBinding.o \
						$(BUILDDIR)/srmv2Client.o \
						$(BUILDDIR)/srm_dependencies.o \
						$(BUILDDIR)/srm_version_wrapper.o \
						$(BUILDDIR)/srm_util.o \
						$(BUILDDIR)/srmv2_directory_functions.o \
						$(BUILDDIR)/srmv2_sync_wrapper.o \
						$(BUILDDIR)/srmv2_async_wrapper.o \
						$(BUILDDIR)/srmv2_space_management_functions.o \
						$(BUILDDIR)/srmv2_discovery_functions.o \
						$(BUILDDIR)/srmv2_permission_functions.o \
						$(BUILDDIR)/srmv2_data_transfer_functions.o
	ranlib $(BUILDDIR)/libsrm.a
	$(LD) $(LIBRARIES_FOLDER) $(LIBRARIES) -r $(BUILDDIR)/stdsoap2.o \
						$(BUILDDIR)/srmv2C.o \
						$(BUILDDIR)/srmSoapBinding.o \
						$(BUILDDIR)/srmv2Client.o \
						$(BUILDDIR)/srm_dependencies.o \
						$(BUILDDIR)/srm_version_wrapper.o \
						$(BUILDDIR)/srm_util.o \
						$(BUILDDIR)/srmv2_directory_functions.o \
						$(BUILDDIR)/srmv2_sync_wrapper.o \
						$(BUILDDIR)/srmv2_async_wrapper.o \
						$(BUILDDIR)/srmv2_space_management_functions.o \
						$(BUILDDIR)/srmv2_discovery_functions.o \
						$(BUILDDIR)/srmv2_permission_functions.o \
						$(BUILDDIR)/srmv2_data_transfer_functions.o \
 					 -o $(BUILDDIR)/libsrm.o
	$(CC) -shared $(LIBRARIES_FOLDER) $(LIBRARIES) $(BUILDDIR)/stdsoap2.o \
						$(BUILDDIR)/srmv2C.o \
						$(BUILDDIR)/srmSoapBinding.o \
						$(BUILDDIR)/srmv2Client.o \
						$(BUILDDIR)/srm_dependencies.o \
						$(BUILDDIR)/srm_version_wrapper.o \
						$(BUILDDIR)/srm_util.o \
						$(BUILDDIR)/srmv2_directory_functions.o \
						$(BUILDDIR)/srmv2_sync_wrapper.o \
						$(BUILDDIR)/srmv2_async_wrapper.o \
						$(BUILDDIR)/srmv2_space_management_functions.o \
						$(BUILDDIR)/srmv2_discovery_functions.o \
						$(BUILDDIR)/srmv2_permission_functions.o \
						$(BUILDDIR)/srmv2_data_transfer_functions.o \
 					 -o $(BUILDDIR)/libsrm.so

	$(CC) $(C2FLAGS) $(SRCDIR)/srm_unittest.c $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm  -o $(BUILDDIR)/srm_unittest # -static -lpthread
	$(CC) $(C2FLAGS) $(SRCDIR)/srm_test.c     $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm  -o $(BUILDDIR)/srm_test # -static -lpthread
	$(CC) $(C2FLAGS) $(SRCDIR)/srm_testunittest.c $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm  -o $(BUILDDIR)/srm_testunittest # -static -lpthread
clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(BUILDDIR)/*.so

