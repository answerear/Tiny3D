#-------------------------------------------------------------------------------
# TinyPlayer —— 游戏 sln 里的 Runtime 宿主
#
# TinyEditor 打开 C++ 工程时会从 Editor/templates/GamePlugin/Player 同步本文件。
#
# 源码来自 SDK 导出的引擎 Player，这里只负责编成 exe、依赖 Runtime DLL、
# 并写好 VS 的 F5 调试属性。
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(TinyPlayer CXX)

if (NOT DEFINED GAME_PLUGIN_NAME OR "${GAME_PLUGIN_NAME}" STREQUAL "")
    set(GAME_PLUGIN_NAME "{ProjectName}")
endif ()

include("${CMAKE_CURRENT_SOURCE_DIR}/../GamePluginCommon.cmake")

if (NOT TINY3D_SDK_PLAYER_SOURCES)
    message(FATAL_ERROR
        "TINY3D_SDK_PLAYER_SOURCES is empty. Rebuild the engine so Tiny3DSDK.cmake "
        "exports the TinyPlayer sources.")
endif ()

if (WIN32)
    add_executable(TinyPlayer WIN32 ${TINY3D_SDK_PLAYER_SOURCES})
else ()
    add_executable(TinyPlayer ${TINY3D_SDK_PLAYER_SOURCES})
endif ()

source_group("Player" FILES ${TINY3D_SDK_PLAYER_SOURCES})

tiny3d_sdk_setup_app(TinyPlayer)

# F5 先把 Runtime 业务 DLL 编出来，Player 运行时再 LoadLibrary
if (TARGET ${GAME_PLUGIN_NAME})
    add_dependencies(TinyPlayer ${GAME_PLUGIN_NAME})
endif ()

set(_player_out "${GAME_PROJECT_ROOT}/Library/Player")
set_target_properties(TinyPlayer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_player_out}")

foreach (_config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${_config}" _config_upper)
    set_target_properties(TinyPlayer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${_config_upper} "${_player_out}")
endforeach ()

tiny3d_native_path(_dbg_project "${GAME_PROJECT_ROOT}")
tiny3d_native_path(_dbg_sdk "${TINY3D_SDK_ROOT}")
set(_dbg_args "--project \"${_dbg_project}\"")
set(_dbg_env "PATH=${_dbg_sdk};%PATH%")
set_target_properties(TinyPlayer PROPERTIES
    VS_DEBUGGER_COMMAND_ARGUMENTS "${_dbg_args}"
    VS_DEBUGGER_WORKING_DIRECTORY "${_dbg_project}"
    VS_DEBUGGER_ENVIRONMENT "${_dbg_env}")
# %PATH% 不能写进 .user：CMake 在 Windows 上会把 %VAR% 当环境变量展开。
# 环境变量只靠 vcxproj 里的 VS_DEBUGGER_ENVIRONMENT。
tiny3d_write_vs_debugger_user(TinyPlayer
    ARGUMENTS "${_dbg_args}"
    WORKING_DIRECTORY "${_dbg_project}")

message(STATUS "TinyPlayer -> ${_player_out}")
