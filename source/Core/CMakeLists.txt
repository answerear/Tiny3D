#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------



# Setup all cmake variables for this project.
set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_SYSTEM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../System")
set(TINY3D_MATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Math")
set(TINY3D_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Utils")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")
set(TINY3D_FRAMEWORK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Framework")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")

add_subdirectory(Runtime)
add_subdirectory(Editor)

