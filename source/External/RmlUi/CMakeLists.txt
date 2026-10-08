#-------------------------------------------------------------------------------
# In-tree RmlUi 6.3. Shared library so T3DRmlUi and game code share one Rml:: state.
# BUILD_SHARED_LIBS is set in this directory only; it does not leak to later targets.
#-------------------------------------------------------------------------------

set(BUILD_SHARED_LIBS ON)

set(RMLUI_SAMPLES OFF CACHE BOOL "" FORCE)
set(RMLUI_FONT_ENGINE "freetype" CACHE STRING "" FORCE)
set(RMLUI_LUA_BINDINGS OFF CACHE BOOL "" FORCE)
set(RMLUI_SVG_PLUGIN OFF CACHE BOOL "" FORCE)
set(RMLUI_LOTTIE_PLUGIN OFF CACHE BOOL "" FORCE)
set(RMLUI_TRACY_PROFILING OFF CACHE BOOL "" FORCE)
set(RMLUI_PRECOMPILED_HEADERS ON CACHE BOOL "" FORCE)
set(RMLUI_THIRDPARTY_CONTAINERS ON CACHE BOOL "" FORCE)
set(RMLUI_MATRIX_ROW_MAJOR OFF CACHE BOOL "" FORCE)

add_subdirectory(RmlUi-6.3)

foreach(_t3d_rmlui_target rmlui rmlui_core rmlui_debugger)
    if(TARGET ${_t3d_rmlui_target})
        set_target_properties(${_t3d_rmlui_target} PROPERTIES FOLDER "External/RmlUi")
    endif()
endforeach()
unset(_t3d_rmlui_target)
