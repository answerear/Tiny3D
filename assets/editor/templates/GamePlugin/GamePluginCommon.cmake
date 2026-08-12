#-------------------------------------------------------------------------------
# {ProjectName} —— Game Plugin 共享构建逻辑
#
# Editor 与 Runtime 两个变体编的是同一份 Include / Source，差别只在链哪个 Core、
# 定不定义 T3D_EDITOR、以及输出到哪个目录。这些差异全部收在这个文件里，两个
# 变体的 CMakeLists.txt 因此都只有几行。
#
# 这个布局与引擎自身的插件一致（参考 SDK 里的 Plugins/Archive/MetaFileSystem）。
#-------------------------------------------------------------------------------

# TINY3D_SDK_ROOT 由 TinyEditor 在 configure 时传入，指向编辑器安装目录。
# 手动在命令行 configure 时需要自己传：
#   cmake -S Assets/Source/Editor -B Temp/ScriptBuild/Editor \
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

# 本文件位于 Assets/Source/ 下，两个变体的 CMakeLists 在子目录里，靠这个变量回到共享源码
set(GAME_SCRIPTS_DIR "${CMAKE_CURRENT_LIST_DIR}")

# 解析工程根：业务 C++ 目录可能在工程根下，也可能在 Assets/Source。
# TinyEditor 会传 GAME_PROJECT_ROOT；手动 configure 时也可显式传入。
# 回退时以 ProjectSettings 为准（不要用 Library，错误构建可能在 Assets/Library 留下残留）。
if (DEFINED GAME_PROJECT_ROOT AND NOT "${GAME_PROJECT_ROOT}" STREQUAL "")
    get_filename_component(GAME_PROJECT_ROOT "${GAME_PROJECT_ROOT}" ABSOLUTE)
else ()
    set(GAME_PROJECT_ROOT "")
    get_filename_component(_dir "${GAME_SCRIPTS_DIR}" ABSOLUTE)
    foreach (_i RANGE 1 3)
        get_filename_component(_dir "${_dir}/.." ABSOLUTE)
        if (EXISTS "${_dir}/ProjectSettings")
            set(GAME_PROJECT_ROOT "${_dir}")
            break()
        endif ()
    endforeach ()
    if (GAME_PROJECT_ROOT STREQUAL "")
        message(FATAL_ERROR
            "Cannot locate project root from '${GAME_SCRIPTS_DIR}'. "
            "Pass -DGAME_PROJECT_ROOT=<project root> when configuring.")
    endif ()
endif ()

#-------------------------------------------------------------------------------
# game_plugin_add(<variant>)
#   variant: Editor | Runtime
#
# 调用方需先定义 GAME_PLUGIN_NAME。
#-------------------------------------------------------------------------------
function(game_plugin_add VARIANT)
    set(_scripts_dir "${GAME_SCRIPTS_DIR}")
    set(_project_dir "${GAME_PROJECT_ROOT}")

    if (VARIANT STREQUAL "Editor")
        set(_target "${GAME_PLUGIN_NAME}Editor")
    else ()
        set(_target "${GAME_PLUGIN_NAME}")
    endif ()

    file(GLOB _headers "${_scripts_dir}/Include/*.h")
    file(GLOB _sources "${_scripts_dir}/Source/*.cpp")

    add_library(${_target} SHARED ${_headers} ${_sources})

    target_include_directories(${_target} PRIVATE "${_scripts_dir}/Include")

    # 让 IDE 里的目录结构和磁盘一致
    source_group("Include" FILES ${_headers})
    source_group("Source" FILES ${_sources})

    # 导出宏：DLL 侧定义它，dllStartPlugin / dllStopPlugin 才会被导出
    target_compile_definitions(${_target} PRIVATE GAMEPLUGIN_EXPORT _USRDLL)

    # SDK 侧统一处理 include 路径、引擎库、ABI 相关的定义与运行时库
    tiny3d_sdk_setup_plugin(${_target} ${VARIANT})

    # 产物直接落到 Library/ScriptAssemblies/<variant>，编辑器从那里取。
    # Windows 的 DLL 归 RUNTIME，类 Unix 的 .so / .dylib 归 LIBRARY，两个都要设。
    set(_out_dir "${_project_dir}/Library/ScriptAssemblies/${VARIANT}")

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

    message(STATUS "Game plugin target : ${_target} -> ${_out_dir}")
endfunction()
