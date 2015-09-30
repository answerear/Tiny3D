####################################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way 
# you like.
####################################################################################################

set_project_name(T3DCore)

if (MSVC)
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

set (TINY3D_RENDERER_LIBRAIES "")

# Search Direct3D 9
if (TINY3D_BUILD_RENDERSYSTEM_D3D9)
	find_package(DirectX)

	if (NOT DirectX9_FOUND)
		# Did not found Direct3D 9 libraries.
		message(STATUS "Could not find dependency: DirectX")
  		message(STATUS "Skipping Direct3D9 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_D3D9 FALSE)
	else ()
		# Find Direct3D 9 libraries.
		# setup header files path
		include_directories(
			"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/Direct3D9"
			"${DirectX9_INCLUDE_DIR}"
			)
		# setup project header files.
		set_project_files(Include\\\\Renderer\\\\Direct3D9 ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/Direct3D9/ .h)
		# setup project source files.
		set_project_files(Source\\\\Renderer\\\\Direct3D9 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Direct3D9/ .cpp)
		# append to link libraries list.
		list(APPEND TINY3D_RENDERER_LIBRAIES ${DirectX9_LIBRARIES})
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_D3D9)

# Search Direct3D 11
if (TINY3D_BUILD_RENDERSYSTEM_D3D11)
	find_package(DirectX11)

	if (NOT DirectX11_FOUND)
		message(STATUS "Could not find dependency: DirectX")
		message(STATUS "Skipping Direct3D11 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_D3D11 FALSE)
	else ()
		# Find Direct3D 11 libraries.
		# setup header files path
		include_directories(
			"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/Direct3D11"
			"${DirectX11_INCLUDE_DIR}"
			)
		# setup project header files.
		set_project_files(Include\\\\Renderer\\\\Direct3D11 ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/Direct3D11/ .h)
		# setup project source files.
		set_project_files(Source\\\\Renderer\\\\Direct3D11 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Direct3D11/ .cpp)
		# append to link libraries list.
		list(APPEND TINY3D_RENDER_LIBRARIES ${DirectX11_LIBRARIES})
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_D3D11)

# Search OpenGL 3.0+
if (TINY3D_BUILD_RENDERSYSTEM_GL3PLUS)
	find_package(OpenGL)

	if (NOT OPENGL_FOUND)
		message(STATUS "Could not find dependency: OpenGL 3+")
		message(STATUS "Skipping OpenGL3Plus rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GL3PLUS FALSE)
	else ()
		message(${OPENGL_LIBRARIES})
		# Find OpenGL 3.0+ libraries.
		# setup header files path
		include_directories(
			"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGL3Plus"
#			"${OPENGL_INCLUDE_DIR}"
			)
		# setup project header files.
		set_project_files(Include\\\\Renderer\\\\OpenGL3Plus ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGL3Plus/ .h)
		# setup project source files.
		set_project_files(Source\\\\Renderer\\\\OpenGL3Plus ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/OpenGL3Plus/ .cpp)
		# append to link libraries list.
		list(APPEND TINY3D_RENDER_LIBRARIES ${OPENGL_LIBRARIES})
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GL3PLUS)

# Search OpenGL ES 2.0
if (TINY3D_BUILD_RENDERSYSTEM_GLES2)
	find_package(OpenGLES2)

	if (NOT OPENGLES2_FOUND)
		message(STATUS "Could not find dependency: OpenGLES 2")
		message(STATUS "Skipping OpenGLES2 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GLES2 FALSE)
	else ()
		# Find OpenGL ES 2.0 libraries.
		# setup header files path
		include_directories(
			"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGLES2"
			"${OPENGLES2_INCLUDE_DIR}"
			)
		# setup project header files.
		set_project_files(Include\\\\Renderer\\\\OpenGLES2 ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGLES2/ .h)
		# setup project source files.
		set_project_files(Source\\\\Renderer\\\\OpenGLES2 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/OpenGLES2/ .cpp)
		# append to link libraries list.
		list(APPEND TINY3D_RENDER_LIBRARIES ${OPENGLES2_LIBRARIES})
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES2)

# Search OpenGl ES 3.0
if (TINY3D_BUILD_RENDERSYSTEM_GLES3)
	find_package(OpenGLES3)

	if (NOT OPENGLES3_FOUND)
		message(STATUS "Could not find dependency: OpenGLES 3")
		message(STATUS "Skipping OpenGLES3 rendersystem build")
		set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
	else ()
		# Find OpenGL ES 3.0 libraries.
		# setup header files path
		include_directories(
			"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGLES3"
			"${OPENGLES3_INCLUDE_DIR}"
			)
		# setup project header files.
		set_project_files(Include\\\\Renderer\\\\OpenGLES3 ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/OpenGLES3/ .h)
		# setup project source files.
		set_project_files(Source\\\\Renderer\\\\OpenGLES3 ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/OpenGLES3/ .cpp)
		# append to link libraries list.
		list(APPEND TINY3D_RENDER_LIBRARIES ${OPENGLES3_LIBRARIES})
	endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES3)

set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform")
set(TINY3D_LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log")

include_directories(
	"${TINY3D_PLATFORM_DIR}/Include"
	"${TINY3D_LOG_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Math"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Node"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer"
	)

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .h)
set_project_files(Include\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Include/Math/ .inl)
set_project_files(Include\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Include/Node/ .h)
set_project_files(Include\\\\Renderer ${CMAKE_CURRENT_SOURCE_DIR}/Include/Renderer/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Math ${CMAKE_CURRENT_SOURCE_DIR}/Source/Math/ .cpp)
set_project_files(Source\\\\Node ${CMAKE_CURRENT_SOURCE_DIR}/Source/Node/ .cpp)
set_project_files(Source\\\\Renderer ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/ .cpp)


if (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
	target_link_libraries(
		${LIB_NAME}
		T3DPlatform
		T3DLog
		${TINY3D_RENDERER_LIBRAIES}
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
elseif (TINY3D_OS_ANDROID)
endif (TINY3D_OS_WINDOWS)
