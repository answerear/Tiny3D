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
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/SG"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Render"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Misc"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Bound"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Listener"
	)

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .inl)
set_project_files(Include\\\\SG ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/ .h)
set_project_files(Include\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Include/Render/ .h)
set_project_files(Include\\\\Misc ${CMAKE_CURRENT_SOURCE_DIR}/Include/Misc/ .h)
set_project_files(Include\\\\Bound ${CMAKE_CURRENT_SOURCE_DIR}/Include/Bound/ .h)
set_project_files(Include\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/ .h)
set_project_files(Include\\\\Listener ${CMAKE_CURRENT_SOURCE_DIR}/Include/Listener/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Source/Math/ .cpp)
set_project_files(Source\\\\SG ${CMAKE_CURRENT_SOURCE_DIR}/Source/SG/ .cpp)
set_project_files(Source\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Source/Render/ .cpp)
set_project_files(Source\\\\Misc ${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/ .cpp)
set_project_files(Include\\\\Bound ${CMAKE_CURRENT_SOURCE_DIR}/Source/Bound/ .cpp)
set_project_files(Source\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Source/Resource/ .cpp)
set_project_files(Source\\\\Listener ${CMAKE_CURRENT_SOURCE_DIR}/Source/Listener/ .cpp)


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

	install(TARGETS ${LIB_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	install(DIRECTORY 
		${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION include/Core
		FILE_PERMISSIONS GROUP_READ OWNER_READ
		PATTERN "Android" EXCLUDE
		PATTERN "iOS" EXCLUDE
		PATTERN "Linux" EXCLUDE
		PATTERN "MacOSX" EXCLUDE
		PATTERN "Unix" EXCLUDE
		PATTERN "Windows" EXCLUDE
		PATTERN "Direct3D9" EXCLUDE
		PATTERN "Direct3D11" EXCLUDE
		PATTERN "OpenGL3Plus" EXCLUDE
		PATTERN "OpenGLES2" EXCLUDE
		PATTERN "OpenGLES3" EXCLUDE
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)
