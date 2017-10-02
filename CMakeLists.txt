cmake_minimum_required ( VERSION 2.6 )
project( arduino-esp8266-aes-encryption )
set( CMAKE_VERBOSE_MAKEFILE OFF )


################################################################################################################################
## OPTIONS SECTION
message( "****** ${PROJECT_NAME} Available Options:\n" )

#Examples
option (HAVING_EXAMPLES " Compiles examples " ON)
message( "-- -DHAVING_EXAMPLES Compiles examples DEFAULT: ${HAVING_EXAMPLES}\n\n")


## END OPTIONS SECTION ###########################################################################################################


##############################################################################################################################
## MACRO DEFINITION SECTION

macro ( install_exe exe )
install ( TARGETS ${exe} EXPORT opticalFlowIP-Targets
		RUNTIME DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/bin
         )
endmacro ( install_exe )

macro ( install_libs shared static )
install ( TARGETS ${shared} EXPORT opticalFlowIP-Targets
			LIBRARY DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib
			RUNTIME DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib )
install ( TARGETS ${static} EXPORT opticalFlowIP-Targets
			ARCHIVE DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib/static
			RUNTIME DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib/static )				
endmacro ( install_libs )

macro ( install_static static )
install ( TARGETS ${static} EXPORT opticalFlowIP-Targets
			ARCHIVE DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib/static
			RUNTIME DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/lib/static )				
endmacro ( install_static )

macro ( install_headers headers )
install ( FILES ${headers} DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/include )
endmacro ( install_headers )

## END MACRO SECTION ###########################################################################################################

set( PRJ_SOURCES Base64.cpp AES.cpp )
set( PRJ_INCLUDES arduino )
set( PRJ_DEPS ${PROJECT_NAME}-static )

set( CMAKE_BUILD_TYPE Debug )


message("###### CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("###### PROJECT_NAME     : ${PROJECT_NAME}")
message("###### PRJ_SOURCES      : ${PRJ_SOURCES}")
message("###### PRJ_INCLUDES     : ${PRJ_INCLUDES}")
message("###### PRJ_DEPS         : ${PRJ_DEPS}")

include_directories( ${PRJ_INCLUDES} )

add_library( ${PROJECT_NAME}-static ${PRJ_SOURCES} )

add_executable( test_unit test_unit.cpp )
target_link_libraries( test_unit ${PRJ_DEPS} )

add_executable( ${PROJECT_NAME}-example example.cpp )
target_link_libraries( ${PROJECT_NAME}-example ${PRJ_DEPS} )

#add_subdirectory( Raid )

#install( EXPORT ${PROJECTNAME}-Targets DESTINATION ${CMAKE_SYSTEM_PROCESSOR}/ )

