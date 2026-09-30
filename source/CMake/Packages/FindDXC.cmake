#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

# FindDXC.cmake
# -------------
# 定位 DirectXShaderCompiler 的头文件与预编译动态库。
#
# 输出变量：
#   DXC_FOUND          - 是否找到
#   DXC_INCLUDE_DIR    - 头文件目录（含 dxc/dxcapi.h）
#   DXC_BINARY         - 运行期需要部署到 T3DHLSLCross 旁边的动态库
#
# 本项目在运行期用 LoadLibrary / dlopen 加载 dxcompiler，因此不导出 link library，
# 也不需要 import lib。

include(FindPkgMacros)
findpkg_begin(DXC)

if (NOT DXC_HOME)
    set(DXC_HOME "${CMAKE_SOURCE_DIR}/../dependencies/dxc")
endif ()

find_path(DXC_INCLUDE_DIR dxc/dxcapi.h
    HINTS "${DXC_HOME}/include"
    NO_DEFAULT_PATH
)

if (WIN32)
    set(_DXC_LIB_DIR "${DXC_HOME}/prebuilt/Windows/x64")
    set(_DXC_LIB_NAME "dxcompiler.dll")
elseif (APPLE)
    set(_DXC_LIB_DIR "${DXC_HOME}/prebuilt/OSX")
    set(_DXC_LIB_NAME "libdxcompiler.dylib")
elseif (UNIX)
    # Linux 按目标架构分目录，不能写死 x64
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        set(_DXC_LIB_ARCH "arm64")
    else ()
        set(_DXC_LIB_ARCH "x64")
    endif ()
    set(_DXC_LIB_DIR "${DXC_HOME}/prebuilt/Linux/${_DXC_LIB_ARCH}")
    set(_DXC_LIB_NAME "libdxcompiler.so")
endif ()

find_file(DXC_BINARY ${_DXC_LIB_NAME}
    HINTS "${_DXC_LIB_DIR}"
    NO_DEFAULT_PATH
)

# macOS：动态库切片必须覆盖全部目标架构
if (APPLE AND DXC_BINARY AND CMAKE_OSX_ARCHITECTURES)
    execute_process(
        COMMAND lipo -archs "${DXC_BINARY}"
        OUTPUT_VARIABLE _dxc_archs
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    foreach (_arch ${CMAKE_OSX_ARCHITECTURES})
        if (NOT _dxc_archs MATCHES "${_arch}")
            message(FATAL_ERROR
                "libdxcompiler.dylib is [${_dxc_archs}] but target requires ${_arch}. "
                "Rebuild it as a universal binary, see doc/todo/ShaderConductor-Replacement-todo.md")
        endif ()
    endforeach ()
    unset(_dxc_archs)
endif ()

if (DXC_INCLUDE_DIR AND DXC_BINARY)
    set(DXC_FOUND TRUE)
endif ()

if (NOT DXC_FOUND)
    if (DXC_FIND_REQUIRED)
        message(FATAL_ERROR "Required dependency DXC not found. Set DXC_HOME to the dxc root directory.")
    elseif (NOT DXC_FIND_QUIETLY)
        message(STATUS "Could not locate DXC")
    endif ()
else ()
    if (NOT DXC_FIND_QUIETLY)
        message(STATUS "Found DXC: ${DXC_INCLUDE_DIR} / ${DXC_BINARY}")
    endif ()
endif ()

mark_as_advanced(DXC_INCLUDE_DIR DXC_BINARY)
