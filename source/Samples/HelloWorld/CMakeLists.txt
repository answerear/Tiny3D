################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
################################################################################

set_project_name(HelloWorld)


set(TINY3D_PLATFORM_INC_DIR "${TINY3D_INC_DIR}/Platform")
set(TINY3D_LOG_INC_DIR "${TINY3D_INC_DIR}/Log")
set(TINY3D_CORE_INC_DIR "${TINY3D_INC_DIR}/Core")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")

# Setup project include files path
include_directories(
	"${TINY3D_PLATFORM_INC_DIR}"
	"${TINY3D_LOG_INC_DIR}"
	"${TINY3D_CORE_INC_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)


# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)


# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

if (TINY3D_OS_WINDOWS)
	set_project_files(Include\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Windows/ .h)
	set_project_files(Source\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Windows/ .cpp)
endif ()



add_executable(
	${BIN_NAME} WIN32 
	${SOURCE_FILES}
	)


target_link_libraries(
	${LIB_NAME}
	T3DPlatform
	T3DLog
	T3DCore
	)

if (TINY3D_OS_WINDOWS)
	install(TARGETS ${BIN_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK 
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DPlatform.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DLog.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DCore.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DD3D9Renderer.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DD3D11Renderer.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DGL3PRenderer.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/libpng.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		)
endif ()

