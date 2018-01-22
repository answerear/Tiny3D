#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(T3DPlatform)

message(STATUS "Generating project : ${BIN_NAME}")


if (MSVC)
	# Add definition only for Windows.
	if (TINY3D_BUILD_SHARED_LIBS)
		add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
	endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


add_definitions(-DT3D_ERR_PLATFORM=0x1000)

# Setup search header path for this project.
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Common"
	"${SDL2_INCLUDE_DIR}"
	)

if (TINY3D_OS_WINDOWS)
	# Windows
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Windows"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop"
		)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/OSX"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop"
		)
elseif (TINY3D_OS_LINUX)
	# Linux
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Linux"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop"
		)
elseif (TINY3D_OS_IOS)
	# iOS
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/iOS"
		)
elseif (TINY3D_OS_ANDROID)
	# Android
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix"
		"${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Android"
		)
endif (TINY3D_OS_WINDOWS)




# Setup header files for this project.
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)
set_project_files(Include\\\\Adapter ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/ .h)
set_project_files(Include\\\\Adapter\\\\Common ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Common/ .h)

if (TINY3D_OS_WINDOWS)
	# Windows
	set_project_files(Include\\\\Adapter\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Windows/ .h)
	set_project_files(Include\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop/ .h)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix/ .h)
	set_project_files(Include\\\\Adapter\\\\OSX ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/OSX/ .h)
	set_project_files(Include\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop/ .h)
elseif (TINY3D_OS_LINUX)
	# Linux
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix/ .h)
	set_project_files(Include\\\\Adapter\\\\Linux ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Linux/ .h)
	set_project_files(Include\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop/ .h)
elseif (TINY3D_OS_IOS)
	# iOS
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix/ .h)
	set_project_files(Include\\\\Adapter\\\\iOS ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/iOS/ .h)
elseif (TINY3D_OS_ANDROID)
	# Android
	set_project_files(Include\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Unix/ .h)
	set_project_files(Include\\\\Adapter\\\\Android ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Android/ .h)
endif (TINY3D_OS_WINDOWS)

if (TINY3D_OS_DESKTOP)
	set_project_files(Include\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Desktop/ .h)
elseif (TINY3D_OS_MOBILE)
	set_project_files(Include\\\\Adapter\\\\Mobile ${CMAKE_CURRENT_SOURCE_DIR}/Include/Adapter/Mobile/ .h)
endif (TINY3D_OS_DESKTOP)


# Setup source files for this project.
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source\\\\Adapter ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/ .cpp)
set_project_files(Source\\\\Adapter\\\\Common ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Common/ .cpp)

if (TINY3D_OS_WINDOWS)
	# Windows
	set_project_files(Source\\\\Adapter\\\\Windows ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Windows/ .cpp)
	set_project_files(Source\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Desktop/ .cpp)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X
	set_project_files(Source\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Unix/ .cpp)
	set_project_files(Source\\\\Adapter\\\\OSX ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/OSX/ .cpp)
	set_project_files(Source\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Desktop/ .cpp)
	
	#foreach(FW ${TINY3D_FRAMEWORKS})
    #    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework ${FW}")
    #endforeach()
    message(STATUS "Link flags : ${CMAKE_EXE_LINKER_FLAGS}")
elseif (TINY3D_OS_LINUX)
	# Linux
	set_project_files(Source\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Unix/ .cpp)
	set_project_files(Source\\\\Adapter\\\\Linux ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Linux/ .cpp)
	set_project_files(Source\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Desktop/ .cpp)
elseif (TINY3D_OS_IOS)
	# iOS
	set_project_files(Source\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Unix/ .cpp)
	set_project_files("Source\\\\Adapter\\\\iOS" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/iOS/" .cpp)
elseif (TINY3D_OS_ANDROID)
	# Android
	set_project_files(Source\\\\Adapter\\\\Unix ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Unix/ .cpp)
	set_project_files(Source\\\\Adapter\\\\Android ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Android/ .cpp)
endif (TINY3D_OS_WINDOWS)

if (TINY3D_OS_DESKTOP)
	set_project_files(Source\\\\Adapter\\\\Desktop ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Desktop/ .cpp)
elseif (TINY3D_OS_MOBILE)
	set_project_files(Source\\\\Adapter\\\\Mobile ${CMAKE_CURRENT_SOURCE_DIR}/Source/Adapter/Mobile/ .cpp)
endif (TINY3D_OS_DESKTOP)

# Setup all files for building this project.
if (TINY3D_BUILD_SHARED_LIBS)
	# Make dynamic library for this project.
	add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
	# Make static library for this project.
	add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


if (TINY3D_OS_WINDOWS)
	# Windows
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE ${SDL2_LIBRARY}
		)

	install(TARGETS ${LIB_NAME}
		RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/Debug CONFIGURATIONS Debug
		)

	install(DIRECTORY 
		${CMAKE_CURRENT_SOURCE_DIR}/Include/ DESTINATION include/Platform
		FILE_PERMISSIONS GROUP_READ OWNER_READ
		PATTERN "Android" EXCLUDE
		PATTERN "iOS" EXCLUDE
		PATTERN "Linux" EXCLUDE
		PATTERN "MacOSX" EXCLUDE
		PATTERN "Unix" EXCLUDE
		PATTERN "Windows" EXCLUDE)
elseif (TINY3D_OS_MACOSX)
	# Mac OS X
	target_link_libraries(
		${LIB_NAME}
		${SDL2_LIBRARY}
		${SDL2_OSX_FRAMEWORKS}
		)
elseif (TINY3D_OS_LINUX)
	# Linux
	target_link_libraries(
		${LIB_NAME}
		)
elseif (TINY3D_OS_IOS)
	# iOS
	target_link_libraries(
		${LIB_NAME}
		)
elseif (TINY3D_OS_ANDROID)
	# Android
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE ${SDL2_BINARY}
		)
endif (TINY3D_OS_WINDOWS)

message(STATUS "Generating project : ${BIN_NAME} - done")
