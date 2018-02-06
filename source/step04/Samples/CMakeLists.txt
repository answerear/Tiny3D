#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------



set(TINY3D_PLATFORM_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Include")
set(TINY3D_LOG_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log/Include")
set(TINY3D_FRAMEWORK_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Framework/Include")
set(TINY3D_MATH_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Math/Include")
set(TINY3D_CORE_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Core/Include")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")

add_subdirectory(hello)
add_subdirectory(TransformationApp)

