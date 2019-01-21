#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")

add_subdirectory(Renderer)
add_subdirectory(Archive)
add_subdirectory(ImageCodec)
