#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DMath)

if (MSVC)
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")


# Setup project include files path
include_directories(
	"${TINY3D_LOG_DIR}/Include"
	"${TINY3D_PLATFORM_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/source"
	)


# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/include/ .h)
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/source/ .h)


# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/source/ .cpp)



if (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


target_link_libraries(
	${LIB_NAME}
	T3DPlatform
	T3DLog
	)

if (TINY3D_OS_WINDOWS)
	install(TARGETS ${LIB_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	install(DIRECTORY 
		${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/Math
		FILE_PERMISSIONS GROUP_READ OWNER_READ
		)
endif ()