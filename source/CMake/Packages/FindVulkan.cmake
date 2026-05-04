#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

include(FindPkgMacros)
findpkg_begin(Vulkan)

# Get VULKAN_SDK environment variable
getenv_path(VULKAN_SDK)

if (ENV_VULKAN_SDK)
    set(Vulkan_PREFIX_PATH ${ENV_VULKAN_SDK})
endif ()

# Find Vulkan include directory
find_path(Vulkan_INCLUDE_DIR
    NAMES vulkan/vulkan.h
    HINTS
        ${Vulkan_PREFIX_PATH}/Include
        ${Vulkan_PREFIX_PATH}/include
    DOC "Path to Vulkan SDK header files"
)

# Find Vulkan library based on architecture
if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        find_library(Vulkan_LIBRARY
            NAMES vulkan-1
            HINTS
                ${Vulkan_PREFIX_PATH}/Lib
                ${Vulkan_PREFIX_PATH}/lib
        )
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        find_library(Vulkan_LIBRARY
            NAMES vulkan-1
            HINTS
                ${Vulkan_PREFIX_PATH}/Lib32
                ${Vulkan_PREFIX_PATH}/lib32
        )
    endif ()
elseif (ANDROID)
    find_library(Vulkan_LIBRARY
        NAMES vulkan
        HINTS ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/lib
    )
else ()
    find_library(Vulkan_LIBRARY
        NAMES vulkan vulkan-1
        HINTS
            ${Vulkan_PREFIX_PATH}/lib
    )
endif ()

findpkg_finish(Vulkan)
