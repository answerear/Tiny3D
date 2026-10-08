# Prefer the in-tree FreeType built by source/External/freetype.
# RmlUi looks up Freetype::Freetype after find_package(Freetype).

if(TARGET freetype)
    if(NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()

    set(FREETYPE_FOUND TRUE)
    set(FREETYPE_VERSION_STRING "2.13.3")
    set(FREETYPE_LIBRARIES Freetype::Freetype)
    get_target_property(_t3d_freetype_includes freetype INTERFACE_INCLUDE_DIRECTORIES)
    if(_t3d_freetype_includes)
        set(FREETYPE_INCLUDE_DIRS "${_t3d_freetype_includes}")
        list(GET _t3d_freetype_includes 0 FREETYPE_INCLUDE_DIR)
    endif()
    unset(_t3d_freetype_includes)
    return()
endif()

set(FREETYPE_FOUND FALSE)
