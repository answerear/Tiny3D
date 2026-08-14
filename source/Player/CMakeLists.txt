#-------------------------------------------------------------------------------
# TinyPlayer —— 游戏 Runtime 宿主
#-------------------------------------------------------------------------------

set_project_name(TinyPlayer)

message(STATUS "Generating project : ${BIN_NAME}")

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif (MSVC)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../System/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Math/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Framework/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Log/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Core/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Utils/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    )

set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)

if (TINY3D_OS_WINDOWS)
    add_executable(${BIN_NAME} WIN32 ${SOURCE_FILES})
elseif (TINY3D_OS_MACOSX)
    add_executable(${BIN_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else ()
    add_executable(${BIN_NAME} ${SOURCE_FILES})
endif ()

target_link_libraries(
    ${LIB_NAME}
    T3DPlatform
    T3DSystem
    T3DLog
    T3DUtils
    T3DMath
    T3DFramework
    T3DCore
    rttr_core
    )

if (TINY3D_OS_MACOSX)
    set_target_properties(${BIN_NAME}
        PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME ${BIN_NAME}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.tiny3d.player"
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path"
        )
endif ()

set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Editor/Player")

message(STATUS "Generating project : ${BIN_NAME} - done")
