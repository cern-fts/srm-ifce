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

GLIB_INCLUDES =  -I$(GLIB_LOCATION) -I$(GLIB_LOCATION)/include
GLIB_LDFLAGS = -L$(GLIB_LOCATION) 


GLOBUS_INCLUDES = -I$(GLOBUS_LOCATION)/include/$(GLOBUS_FLAVOUR)
INCLUDE=$(CCHECK_INCLUDES) $(GLIB_INCLUDES) -I$(SRCDIR)

DEFINES=-D_LARGEFILE64_SOURCE \
		-D_GSOAP_VERSION=0x020706  \
		-D_GSOAP_WSDL2H_VERSION=0x020706 \
		-DUSEGSOAP_2_6 \
		-DUSEGSOAPWSDL2H_2_6 \
		-DGFAL_SECURE

LIBRARIES=
LIBRARIES_FOLDER=-L$(GLOBUS_LOCATION) \
				 -L$(BUILDDIR) \
				 -L$(STAGE_LOCATION)/lib \
				 -L$(STAGE_LOCATION)/$(lib_m) \
				 $(CCHECK_LDFLAGS) \
				 $(GLIB_LDFLAGS)

CFLAGS=-ggdb -fPIC $(INCLUDE) $(DEFINES)
C2FLAGS=-ggdb $(INCLUDE) $(DEFINES) 
 

all:
	$(CC) $(CFLAGS) -c $(SRCDIR)/test.c -o $(BUILDDIR)/test.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/service_discovery.c -o $(BUILDDIR)/service_discovery.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/ldap_dependencies.c -o $(BUILDDIR)/ldap_dependencies.o

	ar rc $(BUILDDIR)/libis_interface.a  $(BUILDDIR)/test.o \
				$(BUILDDIR)/service_discovery.o \
				$(BUILDDIR)/ldap_dependencies.o 	
	ranlib $(BUILDDIR)/libis_interface.a
	$(LD) $(LIBRARIES_FOLDER) $(LIBRARIES) -r $(BUILDDIR)/test.o \
					$(BUILDDIR)/service_discovery.o  \
					$(BUILDDIR)/ldap_dependencies.o  \
 					-o $(BUILDDIR)/libis_interface.o
	$(CC) -shared $(LIBRARIES_FOLDER) $(LIBRARIES) $(BUILDDIR)/test.o \
					$(BUILDDIR)/service_discovery.o \
					$(BUILDDIR)/ldap_dependencies.o  \
 					-o $(BUILDDIR)/libis_interface.so

	$(CC) $(C2FLAGS) $(SRCDIR)/is_unittest.c $(LIBRARIES_FOLDER) $(LIBRARIES) -lcheck -lglib-2.0 -lldap -lis_interface -o $(BUILDDIR)/is_unittest # -static -lpthread

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(BUILDDIR)/*.so

