#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(ImgconvIPC)

if (MSVC)
    # Disable some warning for protobuf
    add_definitions(
        /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
        /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
        /wd4309 # 'conversion' : truncation of constant value
        /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
        /wd4251
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
    "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${PROTOBUF_INCLUDE_DIR}"
)

set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/ .pb.h)
set_project_files(proto ${CMAKE_CURRENT_SOURCE_DIR}/proto/ .pb.cc)

add_library(
    ${LIB_NAME} STATIC
    ${SOURCE_FILES}
)

target_include_directories(
    ${LIB_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(
    ${LIB_NAME}
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

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Tools")
