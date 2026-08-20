#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(MetalRenderer)

if (TINY3D_BUILD_SHARED_LIBS)
    add_definitions(-DMETALRENDERER_EXPORT)
    if (MSVC)
        add_definitions(-D_USRDLL)
    endif (MSVC)
endif (TINY3D_BUILD_SHARED_LIBS)


set(TINY3D_METALRENDERER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)


# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
	"${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_METALRENDERER_SOURCE_DIR}/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    )


# Setup project header files
set_project_files(Include ${TINY3D_METALRENDERER_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${TINY3D_METALRENDERER_SOURCE_DIR}/Source/ .cpp)
set_project_files(Source ${TINY3D_METALRENDERER_SOURCE_DIR}/Source/ .mm)


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
    T3DCore
    rttr_core
    )

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(${LIB_NAME} ${METAL_LIBRARIES} ${COCOA_LIBRARY})
    target_compile_options(${LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
    set_target_properties(${LIB_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        MACOSX_RPATH 1
        INSTALL_NAME_DIR "@rpath"
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
        )
endif (APPLE)

# Setup project folder
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
