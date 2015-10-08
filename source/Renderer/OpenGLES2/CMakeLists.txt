####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

set_project_name(T3DGLES2Renderer)

if (MSVC)
	if (TINY3D_BUILD_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_LIBS)
endif (MSVC)

# Setup project include files path
include_directories(
	"${TINY3D_CORE_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${OPENGLES2_INCLUDE_DIR}"
	)


# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)


if (TINY3D_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_SHARED_LIBS)


target_link_libraries(
	${LIB_NAME}
	${OPENGLES2_LIBRARIES}
	T3DPlatform
	)
