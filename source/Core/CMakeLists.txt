####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

cmake_minimum_required(VERSION 2.6)
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
	add_definitions(-DTINY3D_OS_WIN32)
	set(TINY3D_PLATFORM_WINDOWS ON CACHE STRING "Windows")
elseif (APPLE)
	add_definitions(-DTINY3D_OS_MACOSX)
	set(TINY3D_PLATFORM_MACOSX ON CACHE STRING "Mac OSX")
endif (WIN32)


if (TINY3D_PLATFORM_WINDOWS)
	option(TINY3D_BUILD_RENDERSYSTEM_DIRECTX9 "Build render system useed DirectX 9" ON)
	option(TINY3D_BUILD_RENDERSYSTEM_DIRECTX11 "Build render system used DirectX 11" ON)
	option(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS "Build render system used OpenGL 3.0 +" ON)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" ON)
elseif (TINY3D_PLATFORM_MACOSX)
	option(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS "Build render system used OpenGL 3.0 +" ON)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" OFF)
endif (TINY3D_PLATFORM_WINDOWS)


set(CMAKE_MODULE_PATH 
	"${CMAKE_SOURCE_DIR}/CMake"
	"${CMAKE_SOURCE_DIR}/CMake/Utils"
	"${CMAKE_SOURCE_DIR}/CMake/Packages")


include(FindPkgMacro)


add_subdirectory(RenderSystem)
add_subdirectory(Core)


