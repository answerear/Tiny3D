#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(Demo_Hello)


if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif(MSVC)

# Setup project include files path
include_directories(
	"${TINY3D_PLATFORM_INC_DIR}"
	"${TINY3D_CORE_INC_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	)

set (CMAKE_OSX_DEPLOYMENT_TARGET "8.0")

# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)


# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

if (TINY3D_OS_ANDROID)
	set_project_files(source\\\\Android ${CMAKE_CURRENT_SOURCE_DIR}/Android/app/src/main/cpp/ .cpp)
elseif (TINY3D_OS_IOS)
    set_project_files(source\\\\iOS ${CMAKE_CURRENT_SOURCE_DIR}/iOS/ .cpp)
else (TINY3D_OS_ANDROID)
	set_project_files(source\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Desktop/ .cpp)
endif (TINY3D_OS_ANDROID)

if (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
    set (TINY3D_FRAMEWORKS
        Foundation
        QuartzCore
        UIKit)

    foreach(FW ${TINY3D_FRAMEWORKS})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework ${FW}")
    endforeach()

endif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)

if (TINY3D_OS_ANDROID)
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
elseif (TINY3D_OS_IOS OR TINY3D_OS_MACOSX)
    add_executable(
        ${BIN_NAME} MACOSX_BUNDLE
        ${SOURCE_FILES}
        )

    set_target_properties(${BIN_NAME}
        PROPERTIES
        XCODE_ATTRIBUTE_SDKROOT
        "iphoneos"
        )

    set_target_properties(${BIN_NAME}
        PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
        "iPhone Developer: Yu Wang"
        )
else (TINY3D_OS_ANDROID)
    add_executable(
		${BIN_NAME} WIN32 
		${SOURCE_FILES}
		)
endif (TINY3D_OS_ANDROID)

target_link_libraries(
    ${LIB_NAME}
    T3DPlatform
    T3DCore
    )

if (TINY3D_OS_WINDOWS)
	install(TARGETS ${BIN_NAME}
		RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
		)
endif ()

