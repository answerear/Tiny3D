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
    # Windows
    message(STATUS "Platform : Windows")

    add_definitions(-DT3D_OS_WINDOWS)
    add_definitions(-DT3D_OS_DESKTOP)
    add_definitions(-D_HAS_EXCEPTIONS=0)
    
    set(TINY3D_OS_WINDOWS TRUE CACHE STRING "Windows")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")

    set(SDL2LIB_SUFFIXES "prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}")

    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif (ANDROID)
    # Android
    message(STATUS "Platform : Android")

    add_definitions(-DT3D_OS_ANDROID)
    add_definitions(-DT3D_OS_MOBILE)

    set(TINY3D_OS_ANDROID TRUE CACHE STRING "Android")
    set(TINY3D_OS_MOBILE TRUE CACHE STRING "Mobile")

    set(SDL2LIB_SUFFIXES "prebuilt/Android/${ANDROID_ABI}")

    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
elseif (APPLE AND IOS)
    # iOS
    message(STATUS "Platform : iOS")

    add_definitions(-DT3D_OS_IOS)
    add_definitions(-DT3D_OS_MOBILE)

    set(TINY3D_OS_IOS TRUE CACHE STRING "iOS")
    set(TINY3D_OS_MOBILE TRUE CACHE STRING "Mobile")

    set(CMAKE_OSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})

    set(SDL2LIB_SUFFIXES "prebuilt/iOS")

    set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-undefined-var-template")
elseif (APPLE AND NOT IOS)
    # Mac OS X
    message(STATUS "Platform : Mac OS X")

    add_definitions(-DT3D_OS_OSX)
    add_definitions(-DT3D_OS_DESKTOP)

    set(TINY3D_OS_MACOSX TRUE CACHE STRING "Mac OS X")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")

    set(CMAKE_MACOSX_RPATH true)
    set(SDL2LIB_SUFFIXES "prebuilt/OSX")

    set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-undefined-var-template")
elseif (UNIX AND NOT APPLE)
    # Linux
    message(STATUS "Platform : Linux")
    
    add_definitions(-DT3D_OS_LINUX)
    add_definitions(-DT3D_OS_DESKTOP)
    
    set(TINY3D_OS_LINUX TRUE CACHE STRING "Linux")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")
    set(SDL2LIB_SUFFIXES "prebuilt/Linux")

    set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-undefined-var-template")
endif (WIN32)

if (TINY3D_OS_IOS)
    # Because in iOS, there only support static library. 
    option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" FALSE)
else (TINY3D_OS_IOS)
    option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
endif (TINY3D_OS_IOS)

if (NOT TINY3D_OS_ANDROID)
    # In Android, this switch is setted by build.gradle in Android Studio. 
    option(TINY3D_BUILD_SAMPLES "Build samples" TRUE)
endif (NOT TINY3D_OS_ANDROID)

# Set all relative directory
set(TINY3D_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Tiny3D binary path")
set(TINY3D_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Tiny3D library path")
set(TINY3D_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Tiny3D include path")
set(TINY3D_RES_DIR "${CMAKE_INSTALL_PREFIX}/resources" CACHE PATH "Tiny3D resourcs path")

# Set this directory for debugging app
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" CACHE STRING "Tiny3D binaries files" FORCE)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} BUILTTYPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/bin/${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/lib/${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE} "${PROJECT_SOURCE_DIR}/lib/${OUTPUTCONFIG}")
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Set cmake module path.
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages"
    )


include(FindPkgMacros)
include(ProjectCMakeModule)

# Find SDL2 library for all projects.
set(SDL2DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/SDL2" CACHE PATH "SDL2 library path")
find_package(SDL2)

# Project Platform 
add_subdirectory(Platform)
# Project Core
add_subdirectory(Core)

# Set the dependencies for all projects.
add_dependencies(T3DCore T3DPlatform)

if (TINY3D_BUILD_SAMPLES)
    # Build samples. Set all projects of samples.
    add_subdirectory(Samples)
    add_dependencies(HelloApp T3DCore T3DPlatform)
endif (TINY3D_BUILD_SAMPLES)
