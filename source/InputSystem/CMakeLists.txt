####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################


set(TINY3D_CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Core" CACHE PATH "Tiny3D core source path")
set(TINY3D_PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform" CACHE PATH "Tiny3D platform source path")
set(TINY3D_LOG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log" CACHE PATH "Tiny3D log source path")

if (TINY3D_BUILD_INPUT_SYSTEM_WINDOWS)
	add_subdirectory(Windows)
	add_dependencies(T3DInputSystem T3DCore T3DLog T3DPlatform)
endif (TINY3D_BUILD_INPUT_SYSTEM_WINDOWS)

if (TINY3D_BUILD_INPUT_SYSTEM_MACOSX)
	add_subdirectory(MacOSX)
	add_dependencies(T3DInputSystem T3DCore T3DLog T3DPlatform)
endif (TINY3D_BUILD_INPUT_SYSTEM_MACOSX)

if (TINY3D_BUILD_INPUT_SYSTEM_IOS)
	add_subdirectory(iOS)
	add_dependencies(T3DInputSystem T3DCore T3DLog T3DPlatform)
endif (TINY3D_BUILD_INPUT_SYSTEM_IOS)


if (TINY3D_BUILD_INPUT_SYSTEM_ANDROID)
	add_subdirectory(Android)
	add_dependencies(T3DInputSystem T3DCore T3DLog T3DPlatform)
endif (TINY3D_BUILD_INPUT_SYSTEM_ANDROID)
