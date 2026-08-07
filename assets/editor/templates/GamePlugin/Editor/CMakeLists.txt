#-------------------------------------------------------------------------------
# {ProjectName} Game Plugin —— Editor 变体
#
# TinyEditor 点 Play 时编的就是这一份。它链 T3DCoreEditor 并定义 T3D_EDITOR，
# 因此只能被编辑器加载，不能拿去给发布出去的游戏用（那个是 Runtime 变体）。
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project({ProjectName}GamePluginEditor CXX)

set(GAME_PLUGIN_NAME "{ProjectName}")

include("${CMAKE_CURRENT_SOURCE_DIR}/../GamePluginCommon.cmake")

game_plugin_add(Editor)
