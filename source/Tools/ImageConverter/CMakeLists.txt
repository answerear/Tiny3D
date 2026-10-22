#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(ImageConverter)

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup ")

    add_definitions(
        /wd4244
        /wd4267
        /wd4309
        /wd4800
    )
endif (MSVC)

add_definitions(-DT3D_EDITOR)

include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_SYSTEM_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
    "${TINY3D_UTILS_INC_DIR}"
    "${TINY3D_MATH_INC_DIR}"
    "${TINY3D_FRAMEWORK_INC_DIR}"
    "${TINY3D_CORE_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ImgconvIPC/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common/ImgconvIPC/proto"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${PROTOBUF_INCLUDE_DIR}"
    )

set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)

if (TINY3D_OS_WINDOWS)
    add_compile_options("/wd4251")

    add_executable(
        ${BIN_NAME} WIN32
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${BIN_NAME}
        ImgconvIPC
        T3DPlatform
        T3DSystem
        T3DLog
        T3DUtils
        T3DMath
        T3DFramework
        T3DCoreEditor
        rttr_core
        ${PROTOBUF_LIBRARY}
        )
else ()
    add_executable(
        ${BIN_NAME}
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${BIN_NAME}
        ImgconvIPC
        T3DPlatform
        T3DSystem
        T3DLog
        T3DUtils
        T3DMath
        T3DFramework
        T3DCoreEditor
        rttr_core
        ${PROTOBUF_LIBRARY}
        )
endif ()

set_target_properties(${BIN_NAME} PROPERTIES OUTPUT_NAME imgconv)
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Tools")
