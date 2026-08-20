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
elseif (TINY3D_OS_ANDROID)
    list(REMOVE_ITEM SOURCE_FILES
        "${COMMON_DIR}/SampleShaders_hlsl.h"
        "${COMMON_DIR}/SampleShaders_gl4.h")
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
elseif (TINY3D_OS_ANDROID)
    # Setup dynamic library (.so) project for Android. Because all libraries in Android is dynamics (.so).
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

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

    # Setup all files (.so) for Android APK
    set(T3D_ANDROID_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/libs/${ANDROID_ABI}")

    add_custom_command(TARGET ${BIN_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E make_directory ${T3D_ANDROID_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${T3D_ANDROID_LIB_PATH}
        )

    set(ANDROID_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/src/main/assets")

    # 交叉编译时 BundleBuilder 目标不存在，需由 gradle 在 host 端预构建并通过 -D 传入。
    # 在把 bundle 拷进 APK assets 之前，先用 host bundlebuilder 把 meshes 重打成 bundle。
    set(TINY3D_HOST_BUNDLEBUILDER "" CACHE FILEPATH
        "Path to host-built bundlebuilder executable for cross-compilation")

    if (TINY3D_HOST_BUNDLEBUILDER)
        get_filename_component(HOST_BB_DIR "${TINY3D_HOST_BUNDLEBUILDER}" DIRECTORY)
        add_custom_command(TARGET ${BIN_NAME}
            POST_BUILD
            COMMAND "${TINY3D_HOST_BUNDLEBUILDER}"
                --assets "${CMAKE_SOURCE_DIR}/../assets/samples/meshes"
                --out    "${CMAKE_SOURCE_DIR}/../assets/samples/bundle"
                --binary
            WORKING_DIRECTORY "${HOST_BB_DIR}"
            COMMENT "Building bundle (host bundlebuilder) before APK packaging")
    else()
        message(WARNING "TINY3D_HOST_BUNDLEBUILDER not set; "
            "APK will use existing assets/samples/bundle as-is.")
    endif()

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_ASSETS_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/../assets/config/Android/Tiny3D.cfg"
            "${ANDROID_ASSETS_DIR}/Tiny3D.cfg"
        # 把只读 bundle 资源拷进 APK 的 assets 根下，逻辑路径统一小写
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../assets/samples/bundle"
            "${ANDROID_ASSETS_DIR}/assets/samples/bundle"
        )

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        # 运行时插件 .so 不被 app 链接，需显式拷到 jniLibs 目录随 APK 打包
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:GLES3Renderer>
            ${T3D_ANDROID_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:BundleFSArchive>
            ${T3D_ANDROID_LIB_PATH}
        )
elseif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
    add_executable(
        ${BIN_NAME} MACOSX_BUNDLE
        ${SOURCE_FILES}
        )

    if (TINY3D_OS_IOS)
        message(FATAL_ERROR "SkyboxApp is not supported on iOS yet")
    elseif (TINY3D_OS_MACOSX)
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

        set_target_properties(${BIN_NAME}
            PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME ${BIN_NAME}
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/Info.plist"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.tiny3d.skybox"
            MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
            BUILD_WITH_INSTALL_RPATH 1
            MACOSX_RPATH 1
            BUILD_RPATH "@executable_path"
            INSTALL_RPATH "@executable_path"
            XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path"
            XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep"
            )

        tiny3d_setup_macos_sample(${BIN_NAME})
    endif (TINY3D_OS_IOS)
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
