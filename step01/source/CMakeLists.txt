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
	add_definitions(-DT3D_OS_WINDOWS)
	set(TINY3D_OS_WINDOWS TRUE CACHE STRING "Windows")
elseif (ANDROID)
	add_definitions(-DT3D_OS_ANDROID)
	set(TINY3D_OS_ANDROID TRUE CACHE STRING "Android")
elseif (iOS)
	add_definitions(-DT3D_OS_IOS)
	set(TINY3D_OS_IOS TRUE CACHE STRING "iOS")
elseif (APPLE)
	add_definitions(-DT3D_OS_MACOSX)
	set(TINY3D_OS_MACOSX TRUE CACHE STRING "Mac OS X")
elseif (LINUX)
	add_definitions(-DT3D_OS_LINUX)
	set(ITNY3D_OS_LINUX TRUE CACHE STRING "Linux")
endif (WIN32)

if (TINY3D_OS_IOS)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" FALSE)
else (TINY3D_OS_IOS)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
endif (TINY3D_OS_IOS)


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
	

add_subdirectory(Platform)
add_subdirectory(Core)
add_subdirectory(Samples)
add_dependencies(Demo_Hello T3DCore T3DPlatform)


