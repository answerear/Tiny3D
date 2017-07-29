#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(Demo_Transformation)


set(TINY3D_MATH_INC_DIR "${TINY3D_INC_DIR}/Math")
set(TINY3D_PLATFORM_INC_DIR "${TINY3D_INC_DIR}/Platform")
set(TINY3D_LOG_INC_DIR "${TINY3D_INC_DIR}/Log")
set(TINY3D_CORE_INC_DIR "${TINY3D_INC_DIR}/Core")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")

# Setup project include files path
include_directories(
	"${TINY3D_MATH_INC_DIR}"
	"${TINY3D_PLATFORM_INC_DIR}"
	"${TINY3D_LOG_INC_DIR}"
	"${TINY3D_CORE_INC_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)


# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../common/ .h)


# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)
set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../common/ .cpp)


if (TINY3D_OS_WINDOWS)
	set_project_files(include\\\\windows ${CMAKE_CURRENT_SOURCE_DIR}/windows/ .h)
	set_project_files(source\\\\windows ${CMAKE_CURRENT_SOURCE_DIR}/windows/ .cpp)
endif ()


add_executable(
	${BIN_NAME} WIN32 
	${SOURCE_FILES}
	)


target_link_libraries(
	${LIB_NAME}
	T3DPlatform
	T3DLog
	T3DMath
	T3DCore
	)

if (TINY3D_OS_WINDOWS)
	install(TARGETS ${BIN_NAME}
		RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
		)

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK 
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DPlatform.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DLog.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DMath.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DCore.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DD3D9Renderer.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DD3D11Renderer.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DGL3PRenderer.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BINARY} ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_INSTALL_PREFIX}/media/config ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_INSTALL_PREFIX}/media/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
		)
endif ()

