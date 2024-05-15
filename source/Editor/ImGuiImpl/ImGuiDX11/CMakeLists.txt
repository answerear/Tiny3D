#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(ImGuiDX11)

if (MSVC)
    if (TINY3D_BUILD_SHARED_LIBS)
        add_definitions(-D${LIB_NAME_TOUPPER}_EXPORT -D_USRDLL)
    endif (TINY3D_BUILD_SHARED_LIBS)
endif (MSVC)

set(TINY3D_TINYIMGUI_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../TinyImGui/Include)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${IMGUI_DIR}"
    "${IMGUI_BACKENDS_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../TinyEditor"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${TINY3D_TINYIMGUI_INC_DIR}"
    )

message (STATUS "Common include : ${T3D_COMMON_INCLUDE_DIR}")

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)

# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)

list (APPEND SOURCE_FILES ${T3D_COMMON_INCLUDE_DIR}/T3DEditorInfoDX11.h ${T3D_COMMON_INCLUDE_DIR}/ImGuiApp.h)

#if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
#else (TINY3D_BUILD_SHARED_LIBS)
#    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
#endif (TINY3D_BUILD_SHARED_LIBS)

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Editor/Plugins")

if (TINY3D_OS_IOS)
#    find_library(FWK_SECURITY Security)

    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath")

    set_target_properties (${LIB_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        MACOSX_RPATH 1
        INSTALL_NAME_DIR "@rpath"
        )

    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DSystem
        LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
        LINK_PRIVATE T3DMath
        LINK_PRIVATE T3DCore
        LINK_PRIVATE rttr_core
        LINK_PRIVATE imgui_dx11
        LINK_PRIVATE ${SDL2_LIBRARY}
        )
elseif (TINY3D_OS_LINUX)
    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DSystem
        LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
        LINK_PRIVATE T3DMath
        LINK_PRIVATE T3DCore
        LINK_PRIVATE rttr_core
        LINK_PRIVATE imgui_dx11
        LINK_PRIVATE ${SDL2_LIBRARY}
        )

    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins"
        #COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/lib${BIN_NAME}.so" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins/lib${BIN_NAME}.so"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/lib${BIN_NAME}.so" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${BIN_NAME}.so"
        )
else (TINY3D_OS_IOS)
    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DSystem
        LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
        LINK_PRIVATE T3DMath
        LINK_PRIVATE T3DCore
        LINK_PRIVATE rttr_core
        LINK_PRIVATE imgui_dx11
        LINK_PRIVATE ${SDL2_LIBRARY}
        )
endif (TINY3D_OS_IOS)

add_dependencies(${BIN_NAME} TinyEditor TinyLauncher imgui_dx11)

#install(TARGETS ${LIB_NAME}
#    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
#    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
#    )
