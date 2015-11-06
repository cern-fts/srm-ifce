## cmake build script for srm-ifce
project (srm-ifce)
cmake_minimum_required (VERSION 2.6)
message("cmake source dir : ${CMAKE_SOURCE_DIR}")
# build type
set(CMAKE_BUILD_TYPE "RelWithDebInfo"
        CACHE STRING "type of build")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

#enable testing
INCLUDE(CTest)

# load module
include(DefineInstallationPaths REQUIRED)
include(MacroCopyFile REQUIRED)
include(ReleaseDebugAutoFlags REQUIRED)
include(CMakeGeneratePkgConfig REQUIRED)

find_package(gSOAP REQUIRED)
find_package(CGSI_GSOAP REQUIRED)
find_package(GLIB2 REQUIRED)

# Link against Globus
# Need to be redundant as cgsi-gsoap is a static library in MacOSX
find_package(Globus_COMMON)
find_package(Globus_GSI_CERT_UTILS REQUIRED)
find_package(Globus_GSI_CREDENTIAL REQUIRED)
find_package(Globus_GSS_ASSIST REQUIRED)
find_package(Globus_GSSAPI_GSI REQUIRED)
find_package(OpenSSL REQUIRED)

set (GLOBUS_LIBRARIES
    ${GLOBUS_COMMON_LIBRARIES}
    ${GLOBUS_GSI_CERT_UTILS_LIBRARIES}
    ${GLOBUS_GSI_CREDENTIAL_LIBRARIES}
    ${GLOBUS_GSS_ASSIST_LIBRARIES}
    ${GLOBUS_GSSAPI_GSI_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

#define opts
set(UNIT_TESTS FALSE
        CACHE STRING "enable or disable unit test compilation")

#define PROJECT vars
set(PROJECT_NAME "srm-ifce")
set(OUTPUT_NAME "gfal_srm_ifce")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 23)
set(VERSION_PATCH 3)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#install doc license
install(FILES "LICENSE" "RELEASE-NOTES" "VERSION" "README" "readme.html"
		DESTINATION ${DOC_INSTALL_DIR})


add_subdirectory (src)
