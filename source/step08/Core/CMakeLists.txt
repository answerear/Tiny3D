#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DCore)

message(STATUS "Generating project : ${BIN_NAME}")

if (MSVC)
	# Add definition only for Windows.
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


# Setup all cmake variables for this project.
set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_MATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Math")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")
set(TINY3D_FRAMEWORK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Framework")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")


# Setup search header path for this project.
include_directories(
	"${TINY3D_PLATFORM_DIR}/Include"
	"${TINY3D_MATH_DIR}/Include"
	"${TINY3D_LOG_DIR}/Include"
	"${TINY3D_FRAMEWORK_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${SDL2_INCLUDE_DIR}"
	"${TINY3D_DEP_DIR}/zlib/include"	# only for windows
	)

if (TINY3D_OS_WINDOWS)
	set(TINY3D_ZLIB_LIBRARY "${TINY3D_DEP_DIR}/zlib/prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}/zlibstatic.lib")
endif (TINY3D_OS_WINDOWS)

# Setup header files for this project.
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Include/Kernel/ .h)
set_project_files(Include\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Include/Resource/ .h)
set_project_files(Include\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Include/DataStruct/ .h)
set_project_files(Include\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Include/DataStruct/ .inl)
set_project_files(Include\\\\Memory ${CMAKE_CURRENT_SOURCE_DIR}/Include/Memory/ .h)
set_project_files(Include\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Include/Render/ .h)

# Setup source files for this project.
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Source/Kernel/ .cpp)
set_project_files(Source\\\\Resource ${CMAKE_CURRENT_SOURCE_DIR}/Source/Resource/ .cpp)
set_project_files(Source\\\\DataStruct ${CMAKE_CURRENT_SOURCE_DIR}/Source/DataStruct/ .cpp)
set_project_files(Source\\\\Memory ${CMAKE_CURRENT_SOURCE_DIR}/Source/Memory/ .cpp)
set_project_files(Source\\\\Render ${CMAKE_CURRENT_SOURCE_DIR}/Source/Render/ .cpp)

# tinyxml2
set_project_files(Source\\\\Support\\\\tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Support/tinyxml2/ .h)
set_project_files(Source\\\\Support\\\\tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Support/tinyxml2/ .cpp)



# Setup all files for building this project.
if (TINY3D_BUILD_SHARED_LIBS)
	# Make dynamic library for this project.
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	# Make static library for this project.
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)



if (TINY3D_OS_WINDOWS)
	# Windows

	# Setup link libraries for this project.
	if (MSVC_VERSION GREATER_EQUAL 1900)
		# Visual Studio version is 2015 or later.
		target_link_libraries(
			${LIB_NAME}
			LINK_PRIVATE T3DPlatform
			LINK_PRIVATE T3DLog
			LINK_PRIVATE T3DFramework
			LINK_PRIVATE T3DMath
			LINK_PRIVATE legacy_stdio_definitions
			LINK_PRIVATE ${SDL2_LIBRARY}
#			LINK_PRIVATE ${TINY3D_ZLIB_LIBRARY}
			)
	else ()
		# Visual Studio version is older than 2015
		target_link_libraries(
			${LIB_NAME}
			LINK_PRIVATE T3DPlatform
			LINK_PRIVATE T3DLog
			LINK_PRIVATE T3DFramework
			LINK_PRIVATE T3DMath
			LINK_PRIVATE ${SDL2_LIBRARY}
#			LINK_PRIVATE ${TINY3D_ZLIB_LIBRARY}
			)
	endif ()


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

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		)
	
	add_custom_command(TARGET ${BIN_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../assets/config/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Tiny3D.cfg
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../assets/config/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Tiny3D.cfg
		#COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../../assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../assets
		)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X

	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE T3DPlatform
		LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DFramework
		LINK_PRIVATE T3DMath
		LINK_PRIVATE ${SDL2_LIBRARY}
		LINK_PRIVATE ${SDL2_OSX_FRAMEWORKS}
#		LINK_PRIVATE z
		)
elseif (TINY3D_OS_LINUX)
	# Linux
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE T3DPlatform
		LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DFramework
		LINK_PRIVATE T3DMath
		LINK_PRIVATE ${SDL2_LIBRARY}
#		LINK_PRIVATE z
		)

        add_custom_command(TARGET ${BIN_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../assets/config/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tiny3D.cfg
		)
elseif (TINY3D_OS_IOS)
	# iOS
	target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DLog
        LINK_PRIVATE T3DFramework
        LINK_PRIVATE T3DMath
        )
elseif (TINY3D_OS_ANDROID)
	# Android
	target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DLog
        LINK_PRIVATE T3DFramework
		LINK_PRIVATE T3DMath
        LINK_PRIVATE ${SDL2_BINARY}
#		LINK_PRIVATE z
        )
	
	add_custom_command(TARGET ${BIN_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../assets/config/Tiny3D.cfg ${CMAKE_CURRENT_BINARY_DIR}/../../../../../src/main/assets/Tiny3D.cfg
	#	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../assets/config/Tiny3D.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Tiny3D.cfg
	#	#COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../../assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../assets
		)
endif (TINY3D_OS_WINDOWS)

message(STATUS "Generating project : ${BIN_NAME} - done")
