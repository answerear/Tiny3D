####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

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
elseif (APPLE)
	add_definitions(-DT3D_OS_MACOSX)
	set(TINY3D_OS_MACOSX TRUE CACHE STRING "Mac OS X")
endif (WIN32)


if (TINY3D_OS_WINDOWS)
	option(TINY3D_BUILD_RENDERSYSTEM_D3D9 "Build render system used DirectX 9" TRUE)
	option(TINY3D_BUILD_RENDERSYSTEM_D3D11 "Build render system used DirectX 11" TRUE)
	option(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS "Build render system used OpenGL 3.0 +" TRUE)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
	option(TINY3D_BUILD_INPUT_SYSTEM_WINDOWS "Build input system on Windows" TRUE)
elseif (TINY3D_OS_MACOSX)
	option(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS "Build render system used OpenGL 3.0 +" TRUE)
	option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" FALSE)
	option(TINY3D_BUILD_INPUT_SYSTEM_MACOSX "Build input system on Mac OS X" TRUE)
endif (TINY3D_OS_WINDOWS)

set(TINY3D_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Tiny3D binary path")
set(TINY3D_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Tiny3D library path")
set(TINY3D_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Tiny3D include path")
set(TINY3D_RES_DIR "${CMAKE_INSTALL_PREFIX}/resources" CACHE PATH "Tiny3D resourcs path")


set(CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake"
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils"
	"${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages"
	)


include(FindPkgMacros)
include(ProjectCMakeModule)
	

add_subdirectory(Platform)
add_subdirectory(Log)
add_subdirectory(Core)
add_subdirectory(Renderer)
add_subdirectory(InputSystem)
add_subdirectory(Samples)

if (TINY3D_OS_WINDOWS)
	add_subdirectory(Tools)
endif (TINY3D_OS_WINDOWS)


#add_dependencies(T3DPlatform setup_build)
add_dependencies(T3DLog T3DPlatform)
add_dependencies(T3DCore T3DLog T3DPlatform)
add_dependencies(HelloWorld T3DInputSystem T3DCore T3DLog T3DPlatform)

if (TINY3D_OS_WINDOWS)
	add_dependencies(mesh-conv T3DLog T3DPlatform)
endif (TINY3D_OS_WINDOWS)
