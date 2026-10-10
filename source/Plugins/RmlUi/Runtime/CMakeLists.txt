#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DRmlUi)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DRMLUI_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

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

# 产物在构建期由 rpp 写出。rpp 目标要等根 CMakeLists 末尾才挂上：Plugins 在
# Tools 之前，这里还没有 ReflectionPreprocessor，干净生成时 bin 里也没有 rpp。
tiny3d_declare_generated_sources(Generated ${TINY3D_GENERATED_BASE_DIR}/Plugins/RmlUi/Generated/)
# Templates.generated.cpp 是引擎模板实例，已在 T3DCore 注册。插件只编自己的源文件产物。
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
        T3DCore
        rttr_core
        $<$<NOT:$<CONFIG:Release>>:rmlui_debugger>
    )

# Release 当作 Shipping：不链接调试器。其余配置用 F8 开关。
target_compile_definitions(${LIB_NAME} PRIVATE $<$<NOT:$<CONFIG:Release>>:T3D_RMLUI_DEBUGGER=1>)

if (TINY3D_OS_ANDROID)
    target_link_libraries(${LIB_NAME} PRIVATE android log)
endif (TINY3D_OS_ANDROID)

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/RmlUi")

add_dependencies(${LIB_NAME} T3DCore rmlui_core)
