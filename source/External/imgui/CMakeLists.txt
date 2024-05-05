#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------


if (TINY3D_OS_WINDOWS)
	add_subdirectory(windows)
elseif (TINY3D_OS_MACOSX)
	add_subdirectory(macosx)
elseif (TINY3D_OS_LINUX)
	add_subdirectory(linux)
endif (TINY3D_OS_WINDOWS)

