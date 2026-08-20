#-------------------------------------------------------------------------------
# Find Metal framework on Apple platforms.
#
# Output:
#   METAL_FOUND
#   METAL_LIBRARIES
#-------------------------------------------------------------------------------

include(FindPkgMacros)
findpkg_begin(Metal)

if (APPLE)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    find_library(METALKIT_LIBRARY MetalKit)

    if (METAL_LIBRARY AND QUARTZCORE_LIBRARY)
        set(METAL_FOUND TRUE)
        set(METAL_LIBRARIES ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
        if (METALKIT_LIBRARY)
            list(APPEND METAL_LIBRARIES ${METALKIT_LIBRARY})
        endif ()
    endif ()
endif ()

if (METAL_FOUND)
    message(STATUS "Found Metal: ${METAL_LIBRARIES}")
else ()
    if (NOT Metal_FIND_QUIETLY)
        message(STATUS "Could not locate Metal framework")
    endif ()
    if (Metal_FIND_REQUIRED)
        message(FATAL_ERROR "Required framework Metal not found.")
    endif ()
endif ()

mark_as_advanced(METAL_LIBRARY QUARTZCORE_LIBRARY METALKIT_LIBRARY METAL_LIBRARIES)
