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
set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Include")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")


# Setup search header path for this project.
include_directories(
	"${TINY3D_PLATFORM_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${SDL2_INCLUDE_DIR}"
	)


# Setup header files for this project.
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Include/Kernel/ .h)

# Setup source files for this project.
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Source/Kernel/ .cpp)


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
			LINK_PRIVATE legacy_stdio_definitions
			LINK_PRIVATE ${SDL2_LIBRARY}
			)
	else ()
		# Visual Studio version is older than 2015
		target_link_libraries(
			${LIB_NAME}
			LINK_PRIVATE T3DPlatform
			LINK_PRIVATE ${SDL2_LIBRARY}
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
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X
	
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE T3DPlatform
		LINK_PRIVATE ${SDL2_LIBRARY}
		LINK_PRIVATE ${SDL2_OSX_FRAMEWORKS}
		)
elseif (TINY3D_OS_LINUX)
	# Linux
elseif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
	# iOS
#	target_link_libraries(
#        ${LIB_NAME}
#        LINK_PRIVATE T3DPlatform
#        LINK_PRIVATE ${SDL2_LIBRARY}
#        )
elseif (TINY3D_OS_ANDROID)
	# Android
	target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE ${SDL2_BINARY}
        )
endif (TINY3D_OS_WINDOWS)

message(STATUS "Generating project : ${BIN_NAME} - done")
