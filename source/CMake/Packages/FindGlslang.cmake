#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

# FindGlslang.cmake
# ------------------
# Find the glslang shader compiler library (prebuilt).
#
# Output variables:
#   Glslang_FOUND          - True if glslang was found
#   GLSLANG_INCLUDE_DIR    - Include directory for glslang headers
#   GLSLANG_LIBRARIES      - List of libraries (debug + optimized)

include(FindPkgMacros)
findpkg_begin(Glslang)

find_path(GLSLANG_INCLUDE_DIR glslang/Public/ShaderLang.h
    HINTS
        "${GLSLANG_DIR}/include"
        "${CMAKE_SOURCE_DIR}/../dependencies/glslang/include"
)

# Determine platform-specific prebuilt library directory
if (WIN32)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Windows/x64")
elseif (ANDROID)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Android/${ANDROID_ABI}")
elseif (UNIX AND NOT APPLE)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Linux/x64")
elseif (APPLE)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/OSX")
endif ()

# Define the list of glslang component libraries
set(_GLSLANG_LIB_NAMES
    glslang
    MachineIndependent
    GenericCodeGen
    OSDependent
    SPIRV
    SPIRV-Tools
    SPIRV-Tools-opt
    glslang-default-resource-limits
)

# Build library list (platform-specific naming)
set(GLSLANG_LIBRARIES "")
if (ANDROID)
    # Android: glslang is pre-merged into a single fat .a, plus the resource-limits lib
    list(APPEND GLSLANG_LIBRARIES "${_GLSLANG_LIB_DIR}/libglslang.a")
    list(APPEND GLSLANG_LIBRARIES "${_GLSLANG_LIB_DIR}/libglslang-default-resource-limits.a")
else ()
    # Desktop: debug/optimized separation
    foreach(_lib ${_GLSLANG_LIB_NAMES})
        list(APPEND GLSLANG_LIBRARIES debug "${_GLSLANG_LIB_DIR}/Debug/${_lib}d.lib")
        list(APPEND GLSLANG_LIBRARIES optimized "${_GLSLANG_LIB_DIR}/Release/${_lib}.lib")
    endforeach()
endif ()

if (GLSLANG_INCLUDE_DIR AND _GLSLANG_LIB_DIR)
    set(Glslang_FOUND TRUE)
endif ()

if (NOT Glslang_FOUND)
    if (NOT Glslang_FIND_QUIETLY)
        message(STATUS "Could not locate Glslang")
    endif ()
    if (Glslang_FIND_REQUIRED)
        message(FATAL_ERROR "Required library Glslang not found! Set GLSLANG_DIR to the glslang root directory.")
    endif ()
else ()
    if (NOT Glslang_FIND_QUIETLY)
        message(STATUS "Found Glslang: ${GLSLANG_INCLUDE_DIR}")
    endif ()
endif ()

mark_as_advanced(GLSLANG_INCLUDE_DIR GLSLANG_LIBRARIES)
