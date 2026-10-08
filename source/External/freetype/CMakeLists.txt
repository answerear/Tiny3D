#-------------------------------------------------------------------------------
# In-tree FreeType. Static, PIC, no optional codecs.
# Upstream sources live in freetype-2.13.3/.
#-------------------------------------------------------------------------------

set(BUILD_SHARED_LIBS OFF)
set(DISABLE_FORCE_DEBUG_POSTFIX TRUE)

set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

add_subdirectory(freetype-2.13.3)

if(TARGET freetype)
    set_target_properties(freetype PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        FOLDER "External/freetype")
endif()
