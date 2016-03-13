################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
################################################################################

set_project_name(MeshConverter)


set(TINY3D_PLATFORM_INC_DIR "${TINY3D_INC_DIR}/Platform")
set(TINY3D_LOG_INC_DIR "${TINY3D_INC_DIR}/Log")
#set(TINY3D_CORE_INC_DIR "${TINY3D_INC_DIR}/Core")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")
set(FBXSDK_DIR "${TINY3D_DEP_DIR}/fbxsdk")


# Setup project include files path
include_directories(
	"${TINY3D_PLATFORM_INC_DIR}"
	"${TINY3D_LOG_INC_DIR}"
#	"${TINY3D_CORE_INC_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2"
	"${FBXSDK_DIR}/include"
	)

link_directories(
	"${FBXSDK_DIR}/lib/vs2012/x86/debug"
	)

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

set_project_files(tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/ .h)
set_project_files(tinyxml2 ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/ .cpp)


add_executable(
	${BIN_NAME} 
	${SOURCE_FILES}
	)


target_link_libraries(
	${LIB_NAME}
	T3DPlatform
	T3DLog
#	T3DCore
	libfbxsdk
	)

if (TINY3D_OS_WINDOWS)
	install(TARGETS ${BIN_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	install(DIRECTORY 
		${FBXSDK_DIR}/lib/vs2012/x86/debug/ DESTINATION bin/Debug 
		CONFIGURATIONS Debug
		PATTERN "lib" EXCLUDE
		)

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK 
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DLog.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/T3DPlatform.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/libpng.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/Debug/libfbxsdk.dll ${CMAKE_CURRENT_BINARY_DIR}/Debug
		)
endif ()

