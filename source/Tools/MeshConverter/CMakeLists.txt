################################################################################
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
################################################################################

set_project_name(mesh-conv)


set(TINY3D_PLATFORM_INC_DIR "${TINY3D_INC_DIR}/Platform")
set(TINY3D_LOG_INC_DIR "${TINY3D_INC_DIR}/Log")
set(TINY3D_MATH_INC_DIR "${TINY3D_INC_DIR}/Math")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies")
set(FBXSDK_DIR "${TINY3D_DEP_DIR}/fbxsdk")


add_definitions(-DFBXSDK_SHARED)

# Setup project include files path
include_directories(
	"${TINY3D_PLATFORM_INC_DIR}"
	"${TINY3D_LOG_INC_DIR}"
	"${TINY3D_MATH_INC_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2"
	"${FBXSDK_DIR}/include"
	)

if (MSVC11)
	link_directories(
		"${FBXSDK_DIR}/lib/vs2012/x86"
	)
elseif (MSVC14)
	link_directories(
		"${FBXSDK_DIR}/lib/vs2015/x86"
	)
endif ()


# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)

# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

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
	T3DMath
	libfbxsdk
	)

if (TINY3D_OS_WINDOWS)
	if (MSVC11)
		install(DIRECTORY 
			${FBXSDK_DIR}/lib/vs2012/x86/debug/ DESTINATION bin/Debug 
			CONFIGURATIONS Debug
			PATTERN "lib" EXCLUDE
			)
	elseif (MSVC14)
		install(DIRECTORY 
			${FBXSDK_DIR}/lib/vs2015/x86/debug/ DESTINATION bin/Debug 
			CONFIGURATIONS Debug
			PATTERN "lib" EXCLUDE
			)
	endif ()
	
	install(TARGETS ${BIN_NAME}
		RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
		)

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK 
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DMath.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DLog.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/T3DPlatform.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/libpng.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/bin/debug/libfbxsdk.dll ${CMAKE_CURRENT_BINARY_DIR}/debug
		)
endif ()

