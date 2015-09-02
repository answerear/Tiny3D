####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

set_project_name(T3DPlatform)

if (MSVC)
	if (TINY3D_BUILD_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_LIBS)
endif (MSVC)

# Setup project include files path
include_directories(
	"${CMAKE_SOURCE_DIR}/Include"
	"${CMAKE_SOURCE_DIR}/Include/Adapter"
	"${CMAKE_SOURCE_DIR}/Include/Codec"
	"${CMAKE_SOURCE_DIR}/Include/Console"
	"${CMAKE_SOURCE_DIR}/Include/Device"
	"${CMAKE_SOURCE_DIR}/Include/IO"
	"${CMAKE_SOURCE_DIR}/Include/Time"
	)

if (TINY3D_OS_WINDOWS)
	include_directories(
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Windows"
		)
elseif (TINY3D_OS_MACOSX)
	include_directories(
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_SOURCE_DIR}/Include/Adapter/OSX"
		)
elseif (TINY3D_OS_LINUX)
	include_directories(
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Linux"
		)
elseif (TINY3D_OS_IOS)
	include_directories(
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_SOURCE_DIR}/Include/Adapter/iOS"
		)
elseif (TINY3D_OS_ANDROID)
	include_directories(
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_SOURCE_DIR}/Include/Adapter/Android"
		)
endif (TINY3D_OS_WINDOWS)

# Setup project header files
set_project_files(Include ${CMAKE_SOURCE_DIR}/Include .h)
set_project_files(Include\\\\Adapter ${CMAKE_SOURCE_DIR}/Include/Adapter .h)
set_project_files(Include\\\\Codec ${CMAKE_SOURCE_DIR}/Include/Codec .h)
set_project_files(Include\\\\Console ${CMAKE_SOURCE_DIR}/Include/Console .h)
set_project_files(Include\\\\Device ${CMAKE_SOURCE_DIR}/Include/Device .h)
set_project_files(Include\\\\IO ${CMAKE_SOURCE_DIR}/Include/IO .h)
set_project_files(Include\\\\Time ${CMAKE_SOURCE_DIR}/Include/Time .h)

if (TINY3D_OS_WINDOWS)
	set_project_files(Include\\\\Adapter\\\\Windows ${CMAKE_SOURCE_DIR}/Include/Adapter/Windows .h)
elseif (TINY3D_OS_MACOSX)
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_SOURCE_DIR}/Include/Adapter/Unix .h)
	set_project_files(Include\\\\Adapter\\\\OSX ${CMAKE_SOURCE_DIR}/Include/Adapter/OSX .h)
elseif (TINY3D_OS_LINUX)
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_SOURCE_DIR}/Include/Adapter/Unix .h)
	set_project_files(Include\\\\Adapter\\\\Linux ${CMAKE_SOURCE_DIR}/Include/Adapter/Linux .h)
elseif (TINY3D_OS_IOS)
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_SOURCE_DIR}/Include/Adapter/Unix .h)
	set_project_files(Include\\\\Adapter\\\\iOS ${CMAKE_SOURCE_DIR}}/Include/Adapter/iOS .h)
elseif (TINY3D_OS_ANDROID)
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_SOURCE_DIR}/Include/Adapter/Unix .h)
	set_project_files(Include\\\\Adapter\\\\Android ${CMAKE_SOURCE_DIR}/Include/Adapter/Android .h)
endif (TINY3D_OS_WINDOWS)

# Setup project source files
set_project_files(Source ${CMAKE_SOURCE_DIR}/Source .cpp)
set_project_files(Source ${CMAKE_SOURCE_DIR}/Source/Adapter .cpp)

if (TINY3D_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
	target_link_libraries(
		${LIB_NAME}
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)
