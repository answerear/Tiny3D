#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(FreeImageCodec)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


if (TINY3D_OS_WINDOWS)
    # Windows
    set(FREEIMAGE_LIB_SUFFIXES "prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}")
elseif (TINY3D_OS_ANDROID)
    set(FREEIMAGE_LIB_SUFFIXES "prebuilt/Android/${ANDROID_ABI}")
elseif (TINY3D_OS_IOS)
    set(FREEIMAGE_LIB_SUFFIXES "prebuilt/iOS")
elseif (TINY3D_OS_MACOSX)
    set(FREEIMAGE_LIB_SUFFIXES "prebuilt/OSX")
elseif (TINY3D_OS_LINUX)
    set(FREEIMAGE_LIB_SUFFIXES "prebuilt/Linux")
endif (TINY3D_OS_WINDOWS)


set(FREEIMAGE_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../dependencies/freeimage" CACHE PATH "FreeImage library path")
find_package(FreeImage)

# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${FREEIMAGE_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    )


# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)


if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/ImageCodec")

if (TINY3D_OS_IOS)
#    find_library(FWK_SECURITY Security)

    target_link_libraries(
        ${LIB_NAME}
        T3DLog
        T3DPlatform
        T3DCore
        ${FREEIMAGE_HOME}
        )
elseif (TINY3D_OS_LINUX)
    target_link_libraries(
        ${LIB_NAME}
        T3DLog
        T3DPlatform
        T3DCore
        ${FREEIMAGE_HOME}
        )

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/lib${BIN_NAME}.so" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins/lib${BIN_NAME}.so"
        )
else (TINY3D_OS_IOS)
    target_link_libraries(
        ${LIB_NAME}
        T3DLog
        T3DPlatform
        T3DCore
        ${FREEIMAGE_HOME}
        )
endif (TINY3D_OS_IOS)



install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
