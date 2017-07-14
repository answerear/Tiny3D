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


set(TINY3D_MATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Math")
set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")
set(TINY3D_MEDIA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../media")

set(FREEIMAGE_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/freeimage" CACHE PATH "FreeImage library path")
find_package(FreeImage)

include_directories(
	"${TINY3D_MATH_DIR}/include"
	"${TINY3D_PLATFORM_DIR}/Include"
	"${TINY3D_LOG_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${TINY3D_DEP_DIR}/libpng/include"
	"${FREEIMAGE_INCLUDE_DIR}"
	)

if (TINY3D_OS_WINDOWS)
	if (MSVC11)
		link_directories(
			"${TINY3D_DEP_DIR}/libpng/prebuilt/win32/vc110/x86"
			"${TINY3D_DEP_DIR}/zlib/prebuilt/win32/vc110/x86"
			)
	elseif (MSVC14)
		link_directories(
			"${TINY3D_DEP_DIR}/libpng/prebuilt/win32/vc140/x86"
			"${TINY3D_DEP_DIR}/zlib/prebuilt/win32/vc140/x86"
			)
	endif ()
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
#set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .h)
#set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .inl)
set_project_files(Include\\\\SG ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/ .h)
set_project_files(Include\\\\SG ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/ .inl)
set_project_files(Include\\\\SG\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Node/ .h)
set_project_files(Include\\\\SG\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Node/ .inl)
set_project_files(Include\\\\SG\\\\Visual ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Visual/ .h)
set_project_files(Include\\\\SG\\\\Visual ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Visual/ .inl)
set_project_files(Include\\\\SG\\\\Renderable ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Renderable/ .h)
set_project_files(Include\\\\SG\\\\Renderable ${CMAKE_CURRENT_SOURCE_DIR}/Include/SG/Renderable/ .inl)
set_project_files(Include\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Include/Render/ .h)
set_project_files(Include\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Include/Render/ .inl)
set_project_files(Include\\\\Misc ${CMAKE_CURRENT_SOURCE_DIR}/Include/Misc/ .h)
set_project_files(Include\\\\Misc ${CMAKE_CURRENT_SOURCE_DIR}/Include/Misc/ .inl)
set_project_files(Include\\\\Bound ${CMAKE_CURRENT_SOURCE_DIR}/Include/Bound/ .h)
set_project_files(Include\\\\Bound ${CMAKE_CURRENT_SOURCE_DIR}/Include/Bound/ .inl)
set_project_files(Include\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/ .h)
set_project_files(Include\\\\Listener ${CMAKE_CURRENT_SOURCE_DIR}/Include/Listener/ .h)
set_project_files(Include\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Include/DataStruct/ .h)
set_project_files(Include\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Include/DataStruct/ .inl)
set_project_files(Include\\\\InputDevice ${CMAKE_CURRENT_SOURCE_DIR}/Include/InputDevice/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .h)
#set_project_files(Source\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Source/Math/ .cpp)
set_project_files(Source\\\\SG ${CMAKE_CURRENT_SOURCE_DIR}/Source/SG/ .cpp)
set_project_files(Source\\\\SG\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Source/SG/Node/ .cpp)
set_project_files(Source\\\\SG\\\\Visual ${CMAKE_CURRENT_SOURCE_DIR}/Source/SG/Visual/ .cpp)
set_project_files(Source\\\\SG\\\\Renderable ${CMAKE_CURRENT_SOURCE_DIR}/Source/SG/Renderable/ .cpp)
set_project_files(Source\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Source/Render/ .cpp)
set_project_files(Source\\\\Misc ${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/ .cpp)
set_project_files(Source\\\\Bound ${CMAKE_CURRENT_SOURCE_DIR}/Source/Bound/ .cpp)
set_project_files(Source\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Source/Resource/ .cpp)
set_project_files(Source\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Source/Resource/ .h)
set_project_files(Source\\\\Listener ${CMAKE_CURRENT_SOURCE_DIR}/Source/Listener/ .cpp)
set_project_files(Source\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Source/DataStruct/ .cpp)
set_project_files(Source\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Source/DataStruct/ .h)
set_project_files(Source\\\\InputDevice ${CMAKE_CURRENT_SOURCE_DIR}/Source/InputDevice/ .cpp)

# Internal project header files and source files
set_project_files(Source\\\\ImageCodec ${CMAKE_CURRENT_SOURCE_DIR}/Source/ImageCodec/ .h)
set_project_files(Source\\\\ImageCodec ${CMAKE_CURRENT_SOURCE_DIR}/Source/ImageCodec/ .cpp)

# tinyxml2
set_project_files(Source\\\\Support\\\\tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Support/tinyxml2/ .h)
set_project_files(Source\\\\Support\\\\tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Support/tinyxml2/ .cpp)


if (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE T3DMath
		LINK_PRIVATE T3DPlatform
		LINK_PRIVATE T3DLog
		LINK_PRIVATE libpng
		LINK_PRIVATE zlib
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

	install(DIRECTORY 
		${TINY3D_DEP_DIR}/libpng/prebuilt/win32/vc110/x86/ DESTINATION bin/Debug 
		CONFIGURATIONS Debug
		PATTERN "lib" EXCLUDE
		)

	install(DIRECTORY
		${TINY3D_MEDIA_DIR}/config/
		DESTINATION bin/Debug
		)

	install(DIRECTORY
		${TINY3D_MEDIA_DIR}/resources/
		DESTINATION bin/Debug/resources
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)
