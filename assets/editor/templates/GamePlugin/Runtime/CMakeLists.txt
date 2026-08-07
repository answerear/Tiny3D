#-------------------------------------------------------------------------------
# {ProjectName} Game Plugin —— Runtime 变体
#
# 发布游戏时编的就是这一份，它链 T3DCore、不定义 T3D_EDITOR，由独立的 Player
# 加载。平时不参与构建，所以误用了编辑器专有 API 时这里才会编译失败。可以随时
# 用编辑器菜单里的 Validate Runtime Build 提前检查，不必等到导出。
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project({ProjectName}GamePluginRuntime CXX)

set(GAME_PLUGIN_NAME "{ProjectName}")

include("${CMAKE_CURRENT_SOURCE_DIR}/../GamePluginCommon.cmake")

game_plugin_add(Runtime)
