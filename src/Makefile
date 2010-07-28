CC=gcc
LD=ld
LDFLAGS=-shared

arch=$(shell uname -m)

# handling x86_64's lib64 directory
lib_m=lib
ifeq ($(arch), x86_64)
lib_m=lib64
endif


GLOBUS_LDFLAGS = -L$(GLOBUS_LOCATION)/lib
CCHECK_INCLUDES = -I$(CCHECK_LOCATION)/include  
CCHECK_LDFLAGS = -L$(CCHECK_LOCATION)/$(lib_m)



GLOBUS_INCLUDES = -I$(GLOBUS_LOCATION)/include/$(GLOBUS_FLAVOUR)
INCLUDE=-I $(GSOAP_LOCATION) \
		-I $(GSOAP_LOCATION)/include \
		-I $(GSOAP_LOCATION)/extras \
		-I $(SRCDIR) \
		-I $(CGSI_LOCATION)/include \
		$(GLOBUS_INCLUDES) \
		$(CCHECK_INCLUDES)

DEFINES=-D_LARGEFILE64_SOURCE \
		-D_GSOAP_VERSION=0x020706  \
		-D_GSOAP_WSDL2H_VERSION=0x020706 \
		-DUSEGSOAP_2_6 \
		-DUSEGSOAPWSDL2H_2_6 \
		-DGFAL_SECURE

LIBRARIES=	-lcgsi_plugin_gsoap_2.7 \
			-lglobus_gss_assist_$(GLOBUS_FLAVOUR) \
			-lglobus_gssapi_gsi_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_proxy_core_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_proxy_core_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_credential_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_callback_$(GLOBUS_FLAVOUR) \
			-lglobus_oldgaa_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_sysconfig_$(GLOBUS_FLAVOUR) \
			-lglobus_gsi_cert_utils_$(GLOBUS_FLAVOUR) \
			-lglobus_proxy_ssl_$(GLOBUS_FLAVOUR) \
			-lglobus_common_$(GLOBUS_FLAVOUR) \
			-lltdl_$(GLOBUS_FLAVOUR) \
			-lglobus_callout_$(GLOBUS_FLAVOUR) \
			-lglobus_openssl_error_$(GLOBUS_FLAVOUR) \
			-lglobus_openssl_$(GLOBUS_FLAVOUR) 
LIBRARIES_FOLDER=-L$(GLOBUS_LOCATION) \
				 -L$(BUILDDIR)/ \
				 -L$(STAGE_LOCATION)/lib \
				 -L$(STAGE_LOCATION)/$(lib_m) \
				 $(CCHECK_LDFLAGS)

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
#	$(CC) $(C2FLAGS) $(SRCDIR)/srm_testunittest.c $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lsrm  -o $(BUILDDIR)/srm_testunittest # -static -lpthread
clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(BUILDDIR)/*.so

