#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#-------------------------------------------------------------------------------

set_project_name(GLES3Renderer)

# Android .so export macro
add_definitions(-DGLES3RENDERER_EXPORT)

set(TINY3D_GLES3_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TINY3D_GLES3_BASE_DIR ${TINY3D_GLES3_ROOT_DIR}/Base)
set(TINY3D_GLES3_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_GLES3_BASE_DIR}/Include"
    "${TINY3D_GLES3_RUNTIME_DIR}/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${GLSLANG_INCLUDE_DIR}"
    )

# Setup project header files - Base
set_project_files(Include\\\\Base ${TINY3D_GLES3_BASE_DIR}/Include/ .h)

# Setup project source files - Base
set_project_files(Source\\\\Base ${TINY3D_GLES3_BASE_DIR}/Source/ .cpp)

# Setup project header files - Runtime
set_project_files(Include ${TINY3D_GLES3_RUNTIME_DIR}/Include/ .h)

# Setup project source files - Runtime
set_project_files(Source ${TINY3D_GLES3_RUNTIME_DIR}/Source/ .cpp)

# Android: always build as SHARED library (dynamically loaded plugin)
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

# Android NDK provides GLESv3 and EGL directly, no find_package needed
target_link_libraries(
    ${LIB_NAME}
    GLESv3
    EGL
    android
    log
    T3DMath
    T3DLog
    T3DUtils
    T3DPlatform
    T3DSystem
    T3DCore
    rttr_core
    ${GLSLANG_LIBRARIES}
    )

# Setup project folder
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
