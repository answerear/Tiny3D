#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(AndroidAssetArchive)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DANDROIDASSETARCHIVE_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

set(TINY3D_ANDROIDASSET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})


# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${TINY3D_ANDROIDASSET_SOURCE_DIR}/Include"
    )


# Setup project header files
set_project_files(Include ${TINY3D_ANDROIDASSET_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${TINY3D_ANDROIDASSET_SOURCE_DIR}/Source/ .cpp)


add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Archive")

target_link_libraries(
    ${LIB_NAME}
    T3DPlatform
    T3DSystem
    T3DLog
    T3DUtils
    T3DMath
    T3DCore
    rttr_core
    android
    )


add_dependencies(AndroidAssetArchive T3DCore)
