#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------


if (TINY3D_OS_WINDOWS)
    add_subdirectory(ImGuiDX11)
endif (TINY3D_OS_WINDOWS)
add_subdirectory(ImGuiTiny3D)


