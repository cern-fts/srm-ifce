## cmake build script for srm-ifce
project (srm-ifce)
cmake_minimum_required (VERSION 2.6)
message("cmake source dir : ${CMAKE_SOURCE_DIR}")
# build type
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
# load module
include(DefineInstallationPaths REQUIRED)
include(MacroCopyFile REQUIRED)
include(ReleaseDebugAutoFlags REQUIRED)

find_package(PkgConfig REQUIRED)
find_package(gSOAP REQUIRED)

#define PROJECT vars
set(PROJECT_NAME "srm-ifce")
set(OUTPUT_NAME "gfal_srm_ifce")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 12)
set(VERSION_PATCH 0)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#define vars
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/)
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/)


# libs checks
pkg_check_modules(GSOAP_PKG REQUIRED gsoap)

set(CGSI_PKG_LIBRARIES "cgsi_plugin")



#install doc license
install(FILES "LICENSE" "RELEASE-NOTES" "VERSION" "README"
		DESTINATION ${SHARE_INSTALL_PREFIX}/doc/srm-access-library-for-lcg_util/)


add_subdirectory (src)
