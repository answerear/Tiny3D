#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

# FindGLAD.cmake
# ----------------
# Find the GLAD OpenGL loader library (source-based, not precompiled).
#
# Output variables:
#   GLAD_FOUND        - True if GLAD was found
#   GLAD_INCLUDE_DIR  - Include directory containing glad/glad.h and KHR/
#   GLAD_SOURCES      - Path to glad.c source file

include(FindPkgMacros)
findpkg_begin(GLAD)

find_path(GLAD_INCLUDE_DIR glad/glad.h
    HINTS
        "${GLAD_DIR}/include"
        "${CMAKE_SOURCE_DIR}/../dependencies/glad/include"
)

find_file(GLAD_SOURCES glad.c
    HINTS
        "${GLAD_DIR}/src"
        "${CMAKE_SOURCE_DIR}/../dependencies/glad/src"
)

if (GLAD_INCLUDE_DIR AND GLAD_SOURCES)
    set(GLAD_FOUND TRUE)
endif ()

if (NOT GLAD_FOUND)
    if (NOT GLAD_FIND_QUIETLY)
        message(STATUS "Could not locate GLAD")
    endif ()
    if (GLAD_FIND_REQUIRED)
        message(FATAL_ERROR "Required library GLAD not found! Set GLAD_DIR to the GLAD root directory.")
    endif ()
else ()
    if (NOT GLAD_FIND_QUIETLY)
        message(STATUS "Found GLAD: ${GLAD_INCLUDE_DIR}")
    endif ()
endif ()

mark_as_advanced(GLAD_INCLUDE_DIR GLAD_SOURCES)
