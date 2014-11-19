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

find_package(PkgConfig REQUIRED)
find_package(gSOAP REQUIRED)
find_package(CGSI_GSOAP REQUIRED)
#define opts
set(UNIT_TESTS FALSE
        CACHE STRING "enable or disable unit test compilation")

#define PROJECT vars
set(PROJECT_NAME "srm-ifce")
set(OUTPUT_NAME "gfal_srm_ifce")

set(VERSION_MAJOR 1)
set(VERSION_MINOR 22)
set(VERSION_PATCH 1)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# libs checks
pkg_check_modules(GSOAP_PKG REQUIRED gsoap)
pkg_check_modules(GLIB2_PKG REQUIRED glib-2.0)

#install doc license
install(FILES "LICENSE" "RELEASE-NOTES" "VERSION" "README"
		DESTINATION ${DOC_INSTALL_DIR})


add_subdirectory (src)
