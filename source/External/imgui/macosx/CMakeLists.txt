#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(imgui_osx)

message(STATUS "Generating project : ${BIN_NAME}")

if (TINY3D_BUILD_SHARED_LIBS)
    add_definitions(-DIMGUI_EXPORT)
endif (TINY3D_BUILD_SHARED_LIBS)

set (IMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set (IMGUI_BACKENDS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../backends")

include_directories(
    "${IMGUI_SOURCE_DIR}"
    "${IMGUI_BACKENDS_DIR}"
    "${SDL2_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    )

set_project_files(Include ${IMGUI_SOURCE_DIR}/ .h)
set_project_files(Source ${IMGUI_SOURCE_DIR}/ .cpp)

list(APPEND SOURCE_FILES
    ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.h
    ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp
    )

if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)

set_target_properties (${LIB_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH 1
    MACOSX_RPATH 1
    INSTALL_NAME_DIR "@rpath"
    )

target_link_libraries(
    ${LIB_NAME}
    LINK_PRIVATE ${SDL2_LIBRARY}
    LINK_PRIVATE ${SDL2_OSX_FRAMEWORKS}
    )

set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "External/imgui")

message(STATUS "Generating project : ${BIN_NAME} - done")
