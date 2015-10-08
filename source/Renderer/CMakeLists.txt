####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################


set(TINY3D_CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Core" CACHE PATH "Tiny3D core source path")

if (TINY3D_BUILD_RENDERSYSTEM_D3D9)
	find_package(DirectX)

	if (NOT DirectX9_FOUND)
		message(STATUS "Could not find dependency: DirectX")
  		message(STATUS "Skipping Direct3D9 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_D3D9 FALSE)
	else ()
		add_subdirectory(Direct3D9)
		add_dependencies(T3DD3D9Renderer T3DCore T3DLog T3DPlatform)
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_D3D9)


if (TINY3D_BUILD_RENDERSYSTEM_D3D11)
	find_package(DirectX11)

	if (NOT DirectX11_FOUND)
		message(STATUS "Could not find dependency: DirectX")
		message(STATUS "Skipping Direct3D11 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_D3D11 FALSE)
	else ()
		add_subdirectory(Direct3D11)
		add_dependencies(T3DD3D11Renderer T3DCore T3DLog T3DPlatform)
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_D3D11)


if (TINY3D_BUILD_RENDERSYSTEM_GL3PLUS)
	find_package(OpenGL)

	if (NOT OPENGL_FOUND)
		message(STATUS "Could not find dependency: OpenGL 3+")
		message(STATUS "Skipping OpenGL3Plus rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS FALSE)
	else ()
		add_subdirectory(OpenGL3Plus)
		add_dependencies(T3DGL3PRenderer T3DCore T3DLog T3DPlatform)
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GL3PLUS)


if (TINY3D_BUILD_RENDERSYSTEM_GLES2)
	find_package(OpenGLES2)

	if (NOT OPENGLES2_FOUND)
		message(STATUS "Could not find dependency: OpenGLES 2")
		message(STATUS "Skipping OpenGLES2 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GLES2 FALSE)
	else ()
		add_subdirectory(OpenGLES2)
		add_dependencies(T3DGLES2Renderer T3DCore T3DLog T3DPlatform)
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES2)


if (TINY3D_BUILD_RENDERSYSTEM_GLES3)
	find_package(OpenGLES3)

	if (NOT OPENGLES3_FOUND)
		message(STATUS "Could not find dependency: OpenGLES 3")
		message(STATUS "Skipping OpenGLES3 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
	else ()
		add_subdirectory(OpenGLES3)
		add_dependencies(T3DGLES3Renderer T3DCore T3DLog T3DPlatform)
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES3)
