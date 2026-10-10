#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DRmlUiEditor)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DRMLUI_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

add_definitions(-DT3D_EDITOR)

set(TINY3D_RMLUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Math/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Framework/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Platform/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../System/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Log/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Utils/Include"
    "${TINY3D_RMLUI_SOURCE_DIR}/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    )

set_project_files(Include ${TINY3D_RMLUI_SOURCE_DIR}/Include/ .h)
set_project_files(Source ${TINY3D_RMLUI_SOURCE_DIR}/Source/ .cpp)

# 与 Runtime 共用同一份产物。rpp 只跑一次，接线在根 CMakeLists 末尾。
tiny3d_declare_generated_sources(Generated ${TINY3D_GENERATED_BASE_DIR}/Plugins/RmlUi/Generated/)
list(FILTER SOURCE_FILES EXCLUDE REGEX "Templates\\.generated\\.cpp$")

if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)

target_link_libraries(${LIB_NAME}
    PUBLIC rmlui_core
    PRIVATE
        T3DPlatform
        T3DSystem
        T3DLog
        T3DUtils
        T3DMath
        T3DCoreEditor
        rttr_core
    )

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/RmlUi")

add_dependencies(${LIB_NAME} T3DCoreEditor rmlui_core)
