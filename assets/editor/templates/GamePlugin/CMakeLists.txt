#-------------------------------------------------------------------------------
# {ProjectName} —— 业务逻辑代码
#
# TinyEditor 不用这个文件，它是直接 configure Editor/ 或 Runtime/ 子目录的
# （两个变体各有独立的构建目录，互不干扰）。这里提供一个把两个变体都包进来的
# 顶层工程，方便你想用 IDE 直接打开 Assets/Scripts/ 整体开发时使用：
#
#   cmake -S Assets/Scripts -B <build dir> \
#     -DTINY3D_SDK_ROOT=<TinyEditor 所在目录> \
#     -DGAME_PROJECT_ROOT=<工程根目录>
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project({ProjectName}GamePlugin CXX)

add_subdirectory(Editor)
add_subdirectory(Runtime)
