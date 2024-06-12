#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------


add_subdirectory(protobuf)
add_subdirectory(rttr)

if (TINY3D_OS_DESKTOP)
	add_subdirectory(imgui)
endif (TINY3D_OS_DESKTOP)
