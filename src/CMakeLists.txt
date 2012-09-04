## src cmake build script for srm-ifce


FILE(GLOB src_libs1 "*function*.c")
FILE(GLOB src_libs2 "*dependencies*.c")
FILE(GLOB src_libs3 "*wrapper*.c")
FILE(GLOB src_libs4 "*util*.c")

FILE(GLOB src_program "gfal_srm_ifce_version.c")

FILE(GLOB src_unit_tests "*unittest*.c")
FILE(GLOB src_system_tests "*systemtest*.c")

LIST(APPEND src_libs ${src_libs1} ${src_libs2} ${src_libs3} ${src_libs4})



## gsoap generation
# srmv1
set(srmv1_typemap "typemap-srmv1.dat")
set(srmv1_wsdl "srm.v1.1.wsdl")
set(srm1_header_files "srm.v1.1.h")
set(srm1_source_files "srmC.c")
set(srm1_dir "srmv1")


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${srm1_dir})

add_custom_command(	OUTPUT ${srm1_header_files} 
					DEPENDS ${srmv1_typemap} ${srmv1_wsdl}
					COMMAND ${GSOAP_WSDL2H} ${GSOAP_276_COMPAT_FLAGS} -c -e -y -z -t ${CMAKE_CURRENT_SOURCE_DIR}/${srmv1_typemap}  ${CMAKE_CURRENT_SOURCE_DIR}/${srmv1_wsdl}  -o ${srm1_header_files})
add_custom_command(	OUTPUT ${srm1_source_files} ${srm1_map_files} 
					DEPENDS ${srm1_header_files} 
					COMMAND ${GSOAP_SOAPCPP2} -c -d ${CMAKE_CURRENT_BINARY_DIR}/${srm1_dir} -p srm ${srm1_header_files} )


#srmv2


set(srm2_dir "${CMAKE_CURRENT_BINARY_DIR}/srmv2")
set(srmv2_typemap "typemap-srmv2.dat")
set(srmv2_wsdl "srm.v2.2.wsdl")
set(srm_map_files "${srm2_dir}/srmSoapBinding.nsmap")
set(srm2_header_files "${srm2_dir}/srm.v2.2.h")
set(srm2_source_file1 "${srm2_dir}/srmv2C.c")
set(srm2_source_file2 "${srm2_dir}/srmv2Client.c")


file(MAKE_DIRECTORY ${srm2_dir})


add_custom_command(	OUTPUT  ${srm2_header_files}
					DEPENDS  ${srmv2_typemap} ${srmv2_wsdl} 
					COMMAND ${GSOAP_WSDL2H} ${GSOAP_276_COMPAT_FLAGS} -c -e -y  -t ${CMAKE_CURRENT_SOURCE_DIR}/${srmv2_typemap}  ${CMAKE_CURRENT_SOURCE_DIR}/${srmv2_wsdl}  -o ${srm2_header_files})
add_custom_command(	OUTPUT ${srm2_source_file1} ${srm2_source_file2}
					DEPENDS ${srm2_header_files} 
					COMMAND ${GSOAP_SOAPCPP2} -c -d ${srm2_dir} -p srmv2 ${srm2_header_files} )

					
# transform typemap
set(srm_map_files_c "srmSoapBinding.c")
add_custom_command(OUTPUT  ${srm_map_files_c}  DEPENDS ${srm2_source_file1} COMMAND cp ${srm_map_files} ${CMAKE_CURRENT_BINARY_DIR}/${srm_map_files_c} )

set_source_files_properties(${srm_map_files_c} ${srm2_source_file1} ${srm2_source_file2}
                           PROPERTIES GENERATED TRUE)




add_definitions(-D_LARGEFILE64_SOURCE -DVERSION="${VERSION_STRING}" -DCMAKE_BUILD -D_GNU_SOURCE -DGFAL_SECURE)
include_directories(${srm2_dir} ${CGSI_GSOAP_INCLUDE_DIRS} ${GLIB2_PKG_INCLUDE_DIRS})
link_directories( ${CGSI_GSOAP_LIBRARY_DIRS} )

## main libs
add_library(srm-ifce  SHARED ${src_libs} ${srm2_source_file1} ${srm2_source_file2} ${CMAKE_CURRENT_BINARY_DIR}/${srm_map_files_c})
target_link_libraries(srm-ifce ${GSOAP_PKG_LIBRARIES} ${CGSI_GSOAP_LIBRARIES} ${GLIB2_PKG_LIBRARIES})
set_target_properties(srm-ifce 	PROPERTIES
								VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
								SOVERSION ${VERSION_MAJOR}
								CLEAN_DIRECT_OUTPUT 1
								OUTPUT_NAME ${OUTPUT_NAME} 
	                          
)

# add a package config file
add_PkgConfigFile_for_Library(srm-ifce.pc srm-ifce)




## version control
add_executable(gfal_srm_ifce_version 
									${src_program})
target_link_libraries(gfal_srm_ifce_version )								

IF (UNIT_TESTS)

## unit tests
add_executable(gfal_srm_ifce_unittest 
									${src_unit_tests} )
target_link_libraries(gfal_srm_ifce_unittest ${OUTPUT_NAME} check srm-ifce)
add_executable(gfal_srm_ifce_systemtest 
									${src_system_tests} )
target_link_libraries(gfal_srm_ifce_systemtest ${OUTPUT_NAME} check srm-ifce)

add_test(base_unit_test ${CMAKE_BINARY_DIR}/gfal_srm_ifce_unittest)
#add_test(base_system_test ${CMAKE_BINARY_DIR}/gfal_srm_ifce_systemtest)

ENDIF (UNIT_TESTS)

install(TARGETS srm-ifce 
	    RUNTIME       DESTINATION ${CMAKE_INSTALL_PREFIX}
	    LIBRARY       DESTINATION ${LIB_INSTALL_DIR} )
	    
install(TARGETS gfal_srm_ifce_version
	    RUNTIME       DESTINATION ${BIN_INSTALL_DIR})
	    
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/srm-ifce.pc" DESTINATION ${PKGCONFIG_FILES_DIR})

install(FILES "gfal_srm_ifce_types.h" "gfal_srm_ifce.h"
		DESTINATION ${INCLUDE_INSTALL_DIR})

