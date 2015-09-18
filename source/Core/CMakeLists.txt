####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

set_project_name(T3DCore)

if (MSVC)
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")

include_directories(
	"${TINY3D_PLATFORM_DIR}/Include"
	"${TINY3D_LOG_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Math"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Node"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer"
	)

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .inl)
set_project_files(Include\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Include/Node/ .h)
set_project_files(Include\\\\Renderer ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Source/Math/ .cpp)
set_project_files(Source\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Source/Node/ .cpp)
set_project_files(Source\\\\Renderer ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ .cpp)


if (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
	target_link_libraries(
		${LIB_NAME}
		T3DPlatform
		T3DLog
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)
