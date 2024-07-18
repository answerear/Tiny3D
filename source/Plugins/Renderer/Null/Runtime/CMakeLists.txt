#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(NullRendererRuntime)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-DNULLRENDERER_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)


set(TINY3D_NULLRENDERER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)


# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
	"${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_NULLRENDERER_SOURCE_DIR}/Include"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    )


# Setup project header files
set_project_files(Include ${TINY3D_NULLRENDERER_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${TINY3D_NULLRENDERER_SOURCE_DIR}/Source/ .cpp)


if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif (TINY3D_BUILD_SHARED_LIBS)


target_link_libraries(
    ${LIB_NAME}
    T3DMath
    T3DLog
	T3DUtils
    T3DPlatform
    T3DSystem
    T3DCoreRuntime
    rttr_core
    )

# Setup project folder
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
