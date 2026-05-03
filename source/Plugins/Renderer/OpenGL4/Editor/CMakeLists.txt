#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(GL4RendererEditor)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DGL4RENDERER_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

add_definitions(-DT3D_EDITOR)

set(TINY3D_GL4_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TINY3D_GL4_BASE_DIR ${TINY3D_GL4_ROOT_DIR}/Base)
set(TINY3D_GL4_RUNTIME_DIR ${TINY3D_GL4_ROOT_DIR}/Runtime)
set(TINY3D_GL4_EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# GLAD library path
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../dependencies/glad")

# glslang library path
set(GLSLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../dependencies/glslang")

# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_GL4_BASE_DIR}/Include"
    "${TINY3D_GL4_ROOT_DIR}/Include"
    "${TINY3D_GL4_RUNTIME_DIR}/Include"
    "${TINY3D_GL4_EDITOR_DIR}/Include"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${GLAD_DIR}/include"
    "${GLSLANG_DIR}/include"
    )


# Setup project header files - Base
set_project_files(Include\\\\Base ${TINY3D_GL4_BASE_DIR}/Include/ .h)

# Setup project source files - Base
set_project_files(Source\\\\Base ${TINY3D_GL4_BASE_DIR}/Source/ .cpp)

# Setup project header files - Shared
set_project_files(Include ${TINY3D_GL4_ROOT_DIR}/Include/ .h)

# Setup project source files - Shared
set_project_files(Source ${TINY3D_GL4_ROOT_DIR}/Source/ .cpp)

# Setup project header files - Editor
set_project_files(Include ${TINY3D_GL4_EDITOR_DIR}/Include/ .h)

# Setup project source files - Editor
set_project_files(Source ${TINY3D_GL4_EDITOR_DIR}/Source/ .cpp)

# Add GLAD source file
list(APPEND SOURCE_FILES ${GLAD_DIR}/src/glad.c)


if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (WIN32)
    set(GL4_PLATFORM_LIBS opengl32)
    set(GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Windows/x64")
elseif (UNIX AND NOT APPLE)
    set(GL4_PLATFORM_LIBS GL X11)
    set(GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Linux/x64")
elseif (APPLE)
    set(GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/OSX")
endif ()

# glslang static libraries
set(GLSLANG_LIBS
    debug "${GLSLANG_LIB_DIR}/Debug/glslangd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/MachineIndependentd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/GenericCodeGend.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/OSDependentd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/SPIRVd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/SPIRV-Toolsd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/SPIRV-Tools-optd.lib"
    debug "${GLSLANG_LIB_DIR}/Debug/glslang-default-resource-limitsd.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/glslang.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/MachineIndependent.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/GenericCodeGen.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/OSDependent.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/SPIRV.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/SPIRV-Tools.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/SPIRV-Tools-opt.lib"
    optimized "${GLSLANG_LIB_DIR}/Release/glslang-default-resource-limits.lib"
    )

target_link_libraries(
    ${LIB_NAME}
    ${OPENGL_LIBRARIES}
    ${GL4_PLATFORM_LIBS}
    T3DMath
    T3DLog
    T3DUtils
    T3DPlatform
    T3DSystem
    T3DCoreEditor
    rttr_core
    ${GLSLANG_LIBS}
    ${CMAKE_DL_LIBS}
    )

# Setup project folder
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
