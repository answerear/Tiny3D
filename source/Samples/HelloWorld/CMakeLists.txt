################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
################################################################################

set_project_name(HelloWorld)


set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Platform")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Log")

# Setup project include files path
include_directories(
	"${TINY3D_PLATFORM_DIR}/Include"
	"${TINY3D_LOG_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	)


# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)


# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

if (TINY3D_OS_WINDOWS)
	set_project_files(Include\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Windows/ .h)
	set_project_files(Source\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Windows/ .cpp)
endif ()



add_executable(
	${BIN_NAME} WIN32 
	${SOURCE_FILES}
	)


target_link_libraries(
	${LIB_NAME}
	T3DPlatform
	T3DLog
	T3DCore
	)

