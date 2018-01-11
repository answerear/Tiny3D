#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DCore)

if (MSVC)
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


set(TINY3D_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Include")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")


include_directories(
	"${TINY3D_PLATFORM_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${SDL2_INCLUDE_DIR}"
	)


# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Include/Kernel/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Source/Kernel/ .cpp)

if (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)



if (TINY3D_OS_WINDOWS)
	if (MSVC_VERSION GREATER_EQUAL 1900)
		target_link_libraries(
			${LIB_NAME}
			LINK_PRIVATE T3DPlatform
			LINK_PRIVATE legacy_stdio_definitions
			LINK_PRIVATE ${SDL2_LIBRARY}
			)
	else ()
		target_link_libraries(
			${LIB_NAME}
			LINK_PRIVATE T3DPlatform
			LINK_PRIVATE ${SDL2_LIBRARY}
			)
	endif ()
	

	install(TARGETS ${LIB_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	install(DIRECTORY 
		${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION include/Core
		FILE_PERMISSIONS GROUP_READ OWNER_READ
		PATTERN "Android" EXCLUDE
		PATTERN "iOS" EXCLUDE
		PATTERN "Linux" EXCLUDE
		PATTERN "MacOSX" EXCLUDE
		PATTERN "Unix" EXCLUDE
		PATTERN "Windows" EXCLUDE
		PATTERN "Direct3D9" EXCLUDE
		PATTERN "Direct3D11" EXCLUDE
		PATTERN "OpenGL3Plus" EXCLUDE
		PATTERN "OpenGLES2" EXCLUDE
		PATTERN "OpenGLES3" EXCLUDE
		)

#	install(DIRECTORY
#		${TINY3D_MEDIA_DIR}
#		DESTINATION bin
#		)

#	install(FILES
#		${FREEIMAGE_BINARY}
#		DESTINATION bin/Debug
#		CONFIGURATIONS Debug
#		)

#	add_custom_command(TARGET ${BIN_NAME}
#		PRE_LINK 
#		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
#		COMMAND ${CMAKE_COMMAND} -E copy ${FREEIMAGE_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
#		)

	add_custom_command(TARGET ${BIN_NAME}
		PRE_LINK 
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		)
elseif (TINY3D_OS_MACOSX)
elseif (TINY3D_OS_LINUX)
elseif (TINY3D_OS_IOS)
#	target_link_libraries(
#        ${LIB_NAME}
#        LINK_PRIVATE T3DPlatform
#        LINK_PRIVATE ${SDL2_LIBRARY}
#        )
elseif (TINY3D_OS_ANDROID)
	target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE ${SDL2_BINARY}
        )
endif (TINY3D_OS_WINDOWS)
