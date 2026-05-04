#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

# FindSpirvReflect.cmake
# ----------------------
# Find the SPIRV-Reflect library (source-based, not precompiled).
#
# Output variables:
#   SpirvReflect_FOUND            - True if SPIRV-Reflect was found
#   SPIRV_REFLECT_INCLUDE_DIR     - Include directory containing spirv_reflect.h
#   SPIRV_REFLECT_SOURCES         - Path to spirv_reflect.c source file

include(FindPkgMacros)
findpkg_begin(SpirvReflect)

find_path(SPIRV_REFLECT_INCLUDE_DIR spirv_reflect.h
    HINTS
        "${SPIRV_REFLECT_DIR}"
        "${CMAKE_SOURCE_DIR}/../dependencies/spirv-reflect"
)

find_file(SPIRV_REFLECT_SOURCES spirv_reflect.c
    HINTS
        "${SPIRV_REFLECT_DIR}"
        "${CMAKE_SOURCE_DIR}/../dependencies/spirv-reflect"
)

if (SPIRV_REFLECT_INCLUDE_DIR AND SPIRV_REFLECT_SOURCES)
    set(SpirvReflect_FOUND TRUE)
endif ()

if (NOT SpirvReflect_FOUND)
    if (NOT SpirvReflect_FIND_QUIETLY)
        message(STATUS "Could not locate SPIRV-Reflect")
    endif ()
    if (SpirvReflect_FIND_REQUIRED)
        message(FATAL_ERROR "Required library SPIRV-Reflect not found! Set SPIRV_REFLECT_DIR to the spirv-reflect root directory.")
    endif ()
else ()
    if (NOT SpirvReflect_FIND_QUIETLY)
        message(STATUS "Found SPIRV-Reflect: ${SPIRV_REFLECT_INCLUDE_DIR}")
    endif ()
endif ()

mark_as_advanced(SPIRV_REFLECT_INCLUDE_DIR SPIRV_REFLECT_SOURCES)
