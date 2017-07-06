################################################################################
# This file is part of the CMake build system for Tiny3D Sample
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
################################################################################


set(SDL2DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/sdl2" CACHE PATH "SDL2 library path")
find_package(SDL2)

if (SDL2_FOUND)
	install(FILES 
		${SDL2_BINARY} DESTINATION bin/Debug 
		CONFIGURATIONS Debug
		PATTERN "lib" EXCLUDE
		)

	install(FILES 
		${SDL2_BINARY} DESTINATION bin/Release 
		CONFIGURATIONS Release
		PATTERN "lib" EXCLUDE
		)
endif (SDL2_FOUND)

add_subdirectory(helloworld)
add_subdirectory(transformation)
add_subdirectory(triangle)
add_subdirectory(cube)
add_subdirectory(texture)
add_subdirectory(model)
add_subdirectory(skeleton)

