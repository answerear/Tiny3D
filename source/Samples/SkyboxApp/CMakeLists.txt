#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(SkyboxApp)


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif (MSVC)

# Setup project include files path
include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_SYSTEM_INC_DIR}"
    "${TINY3D_MATH_INC_DIR}"
    "${TINY3D_FRAMEWORK_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
    "${TINY3D_CORE_INC_DIR}"
    "${TINY3D_UTILS_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    )

# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .h)

# Filter shader header files by platform
set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Common)
if (TINY3D_OS_WINDOWS)
    list(REMOVE_ITEM SOURCE_FILES "${COMMON_DIR}/SampleShaders_gles3.h")
elseif (TINY3D_OS_LINUX)
    list(REMOVE_ITEM SOURCE_FILES
        "${COMMON_DIR}/SampleShaders_hlsl.h"
        "${COMMON_DIR}/SampleShaders_gl4.h"
        "${COMMON_DIR}/SampleShaders_gles3.h"
        "${COMMON_DIR}/SampleShaders_vk.h")
elseif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
    list(REMOVE_ITEM SOURCE_FILES
        "${COMMON_DIR}/SampleShaders_hlsl.h"
        "${COMMON_DIR}/SampleShaders_gl4.h"
        "${COMMON_DIR}/SampleShaders_gles3.h"
        "${COMMON_DIR}/SampleShaders_vk.h")
endif ()

# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .cpp)


if (TINY3D_OS_WINDOWS)
    # Setup executable project for Windows.
    add_executable(
        ${BIN_NAME} WIN32
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        T3DPlatform
        T3DSystem
        T3DLog
        T3DUtils
        T3DMath
        T3DFramework
        T3DCore
        rttr_core
        )

    # Setup install files and path for Windows.
    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )

    # bundlebuilder 及其运行时按 pluginPath="." 加载的编辑器插件需先于本工程构建完成
    add_dependencies(${BIN_NAME}
        BundleBuilder
        FileSystemArchiveEditor
        MetaFSArchiveEditor
        NullRendererEditor)

    set(SAMPLES_MESHES_DIR "${CMAKE_SOURCE_DIR}/../assets/samples/meshes")
    set(SAMPLES_BUNDLE_DIR "${CMAKE_SOURCE_DIR}/../assets/samples/bundle")

    # 与 ResourceApp 共用同一份 bundle：构建后先重打包，再复制到执行目录
    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND $<TARGET_FILE:BundleBuilder>
            --assets "${SAMPLES_MESHES_DIR}"
            --out    "${SAMPLES_BUNDLE_DIR}"
            --binary
        WORKING_DIRECTORY $<TARGET_FILE_DIR:BundleBuilder>
        COMMENT "Building bundle from assets/samples/meshes")

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SAMPLES_BUNDLE_DIR}"
            "$<TARGET_FILE_DIR:${BIN_NAME}>/Assets/samples/bundle"
        COMMENT "Copying bundle to runtime directory")
elseif (TINY3D_OS_LINUX)
    # Setup executable project for Linux.
    add_executable(
        ${BIN_NAME}
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        T3DPlatform
        T3DSystem
        T3DLog
        T3DUtils
        T3DFramework
        T3DCore
        rttr_core
        )

    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )
endif (TINY3D_OS_WINDOWS)


# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Samples")
