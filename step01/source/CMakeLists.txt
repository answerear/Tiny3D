#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)


set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../" CACHE PATH "Tiny3D install prefix")

project(Tiny3D)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)")
endif ()

if (CMAKE_SOURCE_DIR MATCHES " ")
	message("Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif()

if (CMAKE_BINARY_DIR MATCHES " ")
	message("Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif()


if (WIN32)
    message(STATUS "Platform : Windows")

	add_definitions(-DT3D_OS_WINDOWS)
	add_definitions(-DT3D_OS_DESKTOP)

	set(TINY3D_OS_WINDOWS TRUE CACHE STRING "Windows")

	set (SDL2LIB_SUFFIXES "prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}")
elseif (ANDROID)
    message(STATUS "Platform : Android")

	add_definitions(-DT3D_OS_ANDROID)
	add_definitions(-DT3D_OS_MOBILE)

	set(TINY3D_OS_ANDROID TRUE CACHE STRING "Android")

	set (SDL2LIB_SUFFIXES "prebuilt/Android/${ANDROID_ABI}")

	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
elseif (IOS)
    message(STATUS "Platform : iOS")

	add_definitions(-DT3D_OS_IOS)
	add_definitions(-DT3D_OS_MOBILE)

	set(TINY3D_OS_IOS TRUE CACHE STRING "iOS")
	
	set(CMAKE_OSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})

	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ObjC")
    set(SDL2LIB_SUFFIXES "prebuilt/iOS")
elseif (APPLE AND NOT IOS)
    message(STATUS "Platform : Mac OS X")

	add_definitions(-DT3D_OS_OSX)
	add_definitions(-DT3D_OS_DESKTOP)

	set(TINY3D_OS_MACOSX TRUE CACHE STRING "Mac OS X")
	set(SDL2LIB_SUFFIXES "prebuilt/OSX")
elseif (LINUX)
	message(STATUS "Platform : Linux")
	add_definitions(-DT3D_OS_LINUX)
	add_definitions(-DT3D_OS_DESKTOP)
	set(ITNY3D_OS_LINUX TRUE CACHE STRING "Linux")
endif (WIN32)

if (TINY3D_OS_IOS)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" FALSE)
else (TINY3D_OS_IOS)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
endif (TINY3D_OS_IOS)

if (NOT TINY3D_OS_ANDROID)
    option(TINY3D_BUILD_SAMPLES "Build samples" TRUE)
endif (NOT TINY3D_OS_ANDROID)

set(TINY3D_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Tiny3D binary path")
set(TINY3D_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Tiny3D library path")
set(TINY3D_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Tiny3D include path")
set(TINY3D_RES_DIR "${CMAKE_INSTALL_PREFIX}/resources" CACHE PATH "Tiny3D resourcs path")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" CACHE STRING "Tiny3D binaries files" FORCE)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} BUILTTYPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/bin/${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/lib/${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/lib/${OUTPUTCONFIG}")
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

set(CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake"
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils"
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages"
	)


include(FindPkgMacros)
include(ProjectCMakeModule)

set(SDL2DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/SDL2" CACHE PATH "SDL2 library path")
find_package(SDL2)

if (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
	if (TINY3D_OS_IOS)
	    set(TINY3D_FRAMEWORKS
			Foundation
	        QuartzCore
	        UIKit
			OpenGLES
			AVFoundation
			AudioToolbox
			ImageIO
			CoreGraphics
			MobileCoreServices
			GameController
			CoreMotion
			)
	elseif (TINY3D_OS_MACOSX)
		find_library(FWK_COREAUDIO CoreAudio)
		find_library(FWK_AUDIOTOOLBOX AudioToolBox)
		find_library(FWK_OPENAL OpenAL)
		find_library(FWK_COCOA Cocoa)
		find_library(FWK_OPENGL OpenGL)
		find_library(FWK_COREVIDEO CoreVideo)
		find_library(FWK_IOKIT IOKit)
		find_library(FWK_FORCEFEEDBACK ForceFeedback)
		find_library(FWK_CARBON Carbon)
		
#		set(TINY3D_FRAMEWORKS
#	        CoreAudio
#	        AudioToolBox
#	        OpenAL
#	        Cocoa
#	        OpenGL
#	        CoreVideo
#	        IOKit
#	        ForceFeedback
#	        Carbon
#	      	)
	endif (TINY3D_OS_IOS)
endif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)

add_subdirectory(Platform)
add_subdirectory(Core)
add_dependencies(T3DCore T3DPlatform)

if (TINY3D_BUILD_SAMPLES)
    add_subdirectory(Samples)
    add_dependencies(Demo_Hello T3DCore T3DPlatform)
endif (TINY3D_BUILD_SAMPLES)
