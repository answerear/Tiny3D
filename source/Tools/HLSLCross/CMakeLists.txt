#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# T3DHLSLCross：HLSL 跨平台编译动态库（DXC + SPIRV-Cross）
#-------------------------------------------------------------------------------

set_project_name(T3DHLSLCross)

message(STATUS "Generating project : ${BIN_NAME}")

find_package(DXC REQUIRED)

# SPIRV-Cross 与本库都靠 C++ 异常报错，不能沿用主工程的 _HAS_EXCEPTIONS=0
remove_definitions(-D_HAS_EXCEPTIONS=0)

# 只链 T3DPlatform（SharedLibrary / Environment / Dir），不链 Core。
# include 路径也只给 Platform，防止实现时顺手 include 了 Core 的东西
include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${DXC_INCLUDE_DIR}"
    )

set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Source  ${CMAKE_CURRENT_SOURCE_DIR}/Source/  .h)
set_project_files(Source  ${CMAKE_CURRENT_SOURCE_DIR}/Source/  .cpp)

add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

# 导出宏在所有平台都要定义：POSIX 上靠它切换 visibility("default")
target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME_TOUPPER}_EXPORT)
if (MSVC)
    target_compile_definitions(${LIB_NAME} PRIVATE _USRDLL)
    target_compile_options(${LIB_NAME} PRIVATE /EHsc /wd4251)
endif ()

target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")

target_link_libraries(${LIB_NAME}
    LINK_PRIVATE T3DPlatform spirv-cross
    )

if (UNIX)
    # dlopen / dladdr 已由 T3DPlatform 消化，本库不再链 libdl。
    # 默认隐藏符号，只导出 T3D_HLSLCROSS_API 标注的接口，防止吸收进来的 spirv-cross 外泄
    set_target_properties(${LIB_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN ON
        )
endif ()

if (APPLE)
    set_target_properties(${LIB_NAME} PROPERTIES
        MACOSX_RPATH             1
        INSTALL_NAME_DIR         "@rpath"
        BUILD_WITH_INSTALL_RPATH 1
        )
endif ()

set(T3D_DXC_BINARY "${DXC_BINARY}" CACHE INTERNAL "dxcompiler shared library path")

# dxcompiler 以本库自身所在目录为锚点查找，必须与本库同目录部署
add_custom_command(TARGET ${LIB_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DXC_BINARY}" "$<TARGET_FILE_DIR:${LIB_NAME}>"
    )

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Tools")

message(STATUS "Generating project : ${BIN_NAME} - done")
