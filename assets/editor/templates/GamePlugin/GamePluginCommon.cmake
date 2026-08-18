#-------------------------------------------------------------------------------
# {ProjectName} —— Game Plugin 共享构建逻辑
#
# TinyEditor 打开 C++ 工程时会从 Editor/templates/GamePlugin 同步本文件。
# 调试路径等构建逻辑请改引擎模板，不要改已经生成的游戏工程。
#
# Editor 与 Runtime 两个变体编的是 Assets/Source 根下同一份 .h / .cpp，差别只在
# 链哪个 Core、定不定义 T3D_EDITOR、以及输出到哪个目录。这些差异全部收在这个
# 文件里，两个变体的 CMakeLists.txt 因此都只有几行。
#
# 业务代码与 Editor / Runtime / Player 这些 CMake 变体目录同层。
#-------------------------------------------------------------------------------

# TINY3D_SDK_ROOT 由 TinyEditor 在 configure 时传入，指向编辑器安装目录。
# 手动在命令行 configure 时需要自己传：
#   cmake -S Assets/Source/Editor -B Temp/CppBuild/Editor \
#     -DTINY3D_SDK_ROOT=<编辑器目录> -DGAME_PROJECT_ROOT=<工程根>
if (NOT DEFINED TINY3D_SDK_ROOT)
    message(FATAL_ERROR
        "TINY3D_SDK_ROOT is not defined. Pass -DTINY3D_SDK_ROOT=<TinyEditor install dir> "
        "when configuring, or just hit Play in TinyEditor.")
endif ()

get_filename_component(TINY3D_SDK_ROOT "${TINY3D_SDK_ROOT}" ABSOLUTE)

if (NOT EXISTS "${TINY3D_SDK_ROOT}/Tiny3DSDK.cmake")
    message(FATAL_ERROR
        "Tiny3DSDK.cmake not found under '${TINY3D_SDK_ROOT}'. "
        "It is generated when the engine is built; make sure TINY3D_SDK_ROOT points at "
        "the directory containing the TinyEditor executable.")
endif ()

include("${TINY3D_SDK_ROOT}/Tiny3DSDK.cmake")

if (NOT EXISTS "${TINY3D_SDK_RPP}")
    message(FATAL_ERROR
        "rpp not found at '${TINY3D_SDK_RPP}'. "
        "Game Plugin TCLASS reflection needs the ReflectionPreprocessor next to TinyEditor. "
        "Rebuild the engine so rpp.exe (and libclang) are copied into the editor bin directory.")
endif ()

if (NOT EXISTS "${TINY3D_SDK_REFLECTION_BASE}")
    message(FATAL_ERROR
        "ReflectionSettings.base.json not found at '${TINY3D_SDK_REFLECTION_BASE}'. "
        "Run the engine generate script so nmake/Core/Runtime/ReflectionSettings.json exists, "
        "then reconfigure the engine to export it with Tiny3DSDK.cmake.")
endif ()

# 本文件位于 Assets/Source/ 下，两个变体的 CMakeLists 在子目录里，靠这个变量回到共享源码
set(GAME_CPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")

# 解析工程根：业务 C++ 目录可能在工程根下，也可能在 Assets/Source。
# TinyEditor 会传 GAME_PROJECT_ROOT；手动 configure 时也可显式传入。
# 回退时以 ProjectSettings 为准（不要用 Library，错误构建可能在 Assets/Library 留下残留）。
if (DEFINED GAME_PROJECT_ROOT AND NOT "${GAME_PROJECT_ROOT}" STREQUAL "")
    get_filename_component(GAME_PROJECT_ROOT "${GAME_PROJECT_ROOT}" ABSOLUTE)
else ()
    set(GAME_PROJECT_ROOT "")
    get_filename_component(_dir "${GAME_CPP_SOURCE_DIR}" ABSOLUTE)
    foreach (_i RANGE 1 3)
        get_filename_component(_dir "${_dir}/.." ABSOLUTE)
        if (EXISTS "${_dir}/ProjectSettings")
            set(GAME_PROJECT_ROOT "${_dir}")
            break()
        endif ()
    endforeach ()
    if (GAME_PROJECT_ROOT STREQUAL "")
        message(FATAL_ERROR
            "Cannot locate project root from '${GAME_CPP_SOURCE_DIR}'. "
            "Pass -DGAME_PROJECT_ROOT=<project root> when configuring.")
    endif ()
endif ()

# VS 调试页（LocalDebugger*）必须用本机分隔符：Windows 是 \，其它是 /。
# CMake 路径一律是 /；3.25 的 VS 生成器会原样写出 VS_DEBUGGER_*，
# 但 Property Pages 优先读 .vcxproj.user，所以两处都写成原生路径。
function(tiny3d_native_path OUT_VAR IN_PATH)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
        set(_in "${IN_PATH}")
        cmake_path(NATIVE_PATH _in NORMALIZE _native)
    elseif (WIN32)
        string(REPLACE "/" "\\" _native "${IN_PATH}")
    else ()
        string(REPLACE "\\" "/" _native "${IN_PATH}")
    endif ()
    set(${OUT_VAR} "${_native}" PARENT_SCOPE)
endfunction()

function(tiny3d_write_vs_debugger_user TARGET)
    if (NOT WIN32 OR NOT CMAKE_GENERATOR MATCHES "Visual Studio")
        return()
    endif ()

    cmake_parse_arguments(ARG "" "COMMAND;ARGUMENTS;WORKING_DIRECTORY;ENVIRONMENT" "" ${ARGN})

    set(_platform "${CMAKE_VS_PLATFORM_NAME}")
    if ("${_platform}" STREQUAL "")
        set(_platform "${CMAKE_GENERATOR_PLATFORM}")
    endif ()
    if ("${_platform}" STREQUAL "")
        set(_platform "x64")
    endif ()

    set(_configs ${CMAKE_CONFIGURATION_TYPES})
    if (NOT _configs)
        set(_configs "${CMAKE_BUILD_TYPE}")
    endif ()
    if (NOT _configs)
        set(_configs Debug)
    endif ()

    set(_xml "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
    string(APPEND _xml "<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
    foreach (_cfg ${_configs})
        string(APPEND _xml "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='${_cfg}|${_platform}'\">\n")
        if (NOT "${ARG_COMMAND}" STREQUAL "")
            string(APPEND _xml "    <LocalDebuggerCommand>${ARG_COMMAND}</LocalDebuggerCommand>\n")
        endif ()
        if (NOT "${ARG_ARGUMENTS}" STREQUAL "")
            string(APPEND _xml "    <LocalDebuggerCommandArguments>${ARG_ARGUMENTS}</LocalDebuggerCommandArguments>\n")
        endif ()
        if (NOT "${ARG_WORKING_DIRECTORY}" STREQUAL "")
            string(APPEND _xml "    <LocalDebuggerWorkingDirectory>${ARG_WORKING_DIRECTORY}</LocalDebuggerWorkingDirectory>\n")
        endif ()
        if (NOT "${ARG_ENVIRONMENT}" STREQUAL "")
            string(APPEND _xml "    <LocalDebuggerEnvironment>${ARG_ENVIRONMENT}</LocalDebuggerEnvironment>\n")
        endif ()
        string(APPEND _xml "    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n")
        string(APPEND _xml "  </PropertyGroup>\n")
    endforeach ()
    string(APPEND _xml "</Project>\n")

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.vcxproj.user" "${_xml}")
endfunction()

#-------------------------------------------------------------------------------
# game_plugin_add(<variant>)
#   variant: Editor | Runtime
#
# 调用方需先定义 GAME_PLUGIN_NAME。
#-------------------------------------------------------------------------------
function(game_plugin_add VARIANT)
    set(_cpp_source_dir "${GAME_CPP_SOURCE_DIR}")
    set(_project_dir "${GAME_PROJECT_ROOT}")

    if (VARIANT STREQUAL "Editor")
        set(_target "${GAME_PLUGIN_NAME}Editor")
    else ()
        set(_target "${GAME_PLUGIN_NAME}")
    endif ()

    file(GLOB _headers
        "${_cpp_source_dir}/*.h"
        "${_cpp_source_dir}/*.hpp"
        "${_cpp_source_dir}/*.hh")
    file(GLOB _sources
        "${_cpp_source_dir}/*.cpp"
        "${_cpp_source_dir}/*.cc"
        "${_cpp_source_dir}/*.cxx")

    add_library(${_target} SHARED ${_headers} ${_sources})

    target_include_directories(${_target} PRIVATE "${_cpp_source_dir}")

    source_group("Header Files" FILES ${_headers})
    source_group("Source Files" FILES ${_sources})

    # 导出宏：DLL 侧定义它，dllStartPlugin / dllStopPlugin 才会被导出
    target_compile_definitions(${_target} PRIVATE GAMEPLUGIN_EXPORT _USRDLL)

    # SDK 侧统一处理 include 路径、引擎库、ABI 相关的定义与运行时库
    tiny3d_sdk_setup_plugin(${_target} ${VARIANT})

    # TCLASS 反射：生成物放在构建目录，避免写进 Assets/Source
    tiny3d_enable_reflection(${_target}
        SOURCE_DIR "${_cpp_source_dir}"
        GENERATED_DIR "${CMAKE_BINARY_DIR}/Generated"
        SETTINGS_DIR "${CMAKE_BINARY_DIR}/Reflect"
        BASE_SETTINGS "${TINY3D_SDK_REFLECTION_BASE}"
        EXTRA_INCLUDES "${_cpp_source_dir}")

    # 产物直接落到 Library/CppAssemblies/<variant>，编辑器从那里取。
    # Windows 的 DLL 归 RUNTIME，类 Unix 的 .so / .dylib 归 LIBRARY，两个都要设。
    set(_out_dir "${_project_dir}/Library/CppAssemblies/${VARIANT}")

    set_target_properties(${_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${_out_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${_out_dir}")

    # 多配置生成器（VS / Xcode）会在输出目录后面再追加一层配置名，逐配置覆写掉，
    # 免得编辑器还要去猜产物在 Debug/ 还是 Release/ 子目录里。
    foreach (_config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_config}" _config_upper)
        set_target_properties(${_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${_config_upper} "${_out_dir}"
            LIBRARY_OUTPUT_DIRECTORY_${_config_upper} "${_out_dir}")
    endforeach ()

    # Editor DLL 的 F5 可以拉起 TinyEditor；编辑器已经开着时改用
    # 「调试 → 附加到进程 → TinyEditor.exe」。
    if (VARIANT STREQUAL "Editor")
        get_filename_component(_proj_name "${GAME_PROJECT_ROOT}" NAME)
        get_filename_component(_proj_parent "${GAME_PROJECT_ROOT}" DIRECTORY)
        tiny3d_native_path(_dbg_cmd "${TINY3D_SDK_ROOT}/TinyEditor.exe")
        tiny3d_native_path(_dbg_parent "${_proj_parent}")
        tiny3d_native_path(_dbg_cwd "${TINY3D_SDK_ROOT}")
        set(_dbg_args "-p \"${_dbg_parent}\" -n \"${_proj_name}\" -o")
        set_target_properties(${_target} PROPERTIES
            VS_DEBUGGER_COMMAND "${_dbg_cmd}"
            VS_DEBUGGER_COMMAND_ARGUMENTS "${_dbg_args}"
            VS_DEBUGGER_WORKING_DIRECTORY "${_dbg_cwd}")
        tiny3d_write_vs_debugger_user(${_target}
            COMMAND "${_dbg_cmd}"
            ARGUMENTS "${_dbg_args}"
            WORKING_DIRECTORY "${_dbg_cwd}")
    endif ()

    message(STATUS "Game plugin target : ${_target} -> ${_out_dir}")
endfunction()
