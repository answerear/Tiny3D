#-------------------------------------------------------------------------------
# {ProjectName} —— 业务逻辑代码
#
# TinyEditor 不用这个文件，它是直接 configure Editor/ 或 Runtime/ 子目录的
# （两个变体各有独立的构建目录，互不干扰）。这里提供一个把 Editor、Runtime
# 和 TinyPlayer 都包进来的顶层工程，方便用 IDE 打开 Assets/Source/ 整体开发：
#
#   cmake -S Assets/Source -B <build dir> \
#     -DTINY3D_SDK_ROOT=<TinyEditor 所在目录> \
#     -DGAME_PROJECT_ROOT=<工程根目录>
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project({ProjectName}GamePlugin CXX)

set(GAME_PLUGIN_NAME "{ProjectName}")

add_subdirectory(Editor)
add_subdirectory(Runtime)
add_subdirectory(Player)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT TinyPlayer)
