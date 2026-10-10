#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(RmlUiApp)

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif (MSVC)

include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_SYSTEM_INC_DIR}"
    "${TINY3D_MATH_INC_DIR}"
    "${TINY3D_FRAMEWORK_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
    "${TINY3D_CORE_INC_DIR}"
    "${TINY3D_UTILS_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Plugins/RmlUi/Include"
    "${CMAKE_SOURCE_DIR}/External/RmlUi/RmlUi-6.3/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    )

set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .h)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .cpp)


set(_rmlui_app_libs
    T3DPlatform
    T3DSystem
    T3DLog
    T3DUtils
    T3DMath
    T3DFramework
    T3DCore
    T3DRmlUi
    rttr_core
    )

if (TINY3D_OS_WINDOWS)
    add_executable(${BIN_NAME} WIN32 ${SOURCE_FILES})
    target_link_libraries(${LIB_NAME} ${_rmlui_app_libs})
elseif (TINY3D_OS_ANDROID)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
    target_link_libraries(${LIB_NAME} ${_rmlui_app_libs})

    set(T3D_ANDROID_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/libs/${ANDROID_ABI}")
    set(ANDROID_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/src/main/assets")

    add_custom_command(TARGET ${BIN_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E make_directory ${T3D_ANDROID_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${T3D_ANDROID_LIB_PATH}
        )

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_ASSETS_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/../assets/config/Android/Tiny3D.cfg"
            "${ANDROID_ASSETS_DIR}/Tiny3D.cfg"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:GLES3Renderer> ${T3D_ANDROID_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:T3DRmlUi> ${T3D_ANDROID_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rmlui_core> ${T3D_ANDROID_LIB_PATH}
        )
elseif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
    add_executable(${BIN_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
    target_link_libraries(${LIB_NAME} ${_rmlui_app_libs})
    if (TINY3D_OS_MACOSX)
        tiny3d_setup_macos_sample(${BIN_NAME})
    endif ()
elseif (TINY3D_OS_LINUX)
    add_executable(${BIN_NAME} ${SOURCE_FILES})
    target_link_libraries(${LIB_NAME} ${_rmlui_app_libs})
endif ()

if (TARGET ${BIN_NAME})
    set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Samples")
    add_dependencies(${BIN_NAME} T3DRmlUi)
endif ()
