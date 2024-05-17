#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(TinyLauncher)


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif (MSVC)

set(TINY3D_TINYIMGUI_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../TinyImGui/Include)

# Setup project include files path
include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_SYSTEM_INC_DIR}"
    "${TINY3D_MATH_INC_DIR}"
    "${TINY3D_FRAMEWORK_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
    "${TINY3D_CORE_INC_DIR}"
	"${TINY3D_UTILS_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${IMGUI_DIR}"
    "${IMGUI_BACKENDS_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${TINY3D_TINYIMGUI_INC_DIR}"
    )

# Setup project header files
set_project_files(include ${CMAKE_CURRENT_SOURCE_DIR}/ .h)
set_project_files(include\\\\Language ${CMAKE_CURRENT_SOURCE_DIR}/Language/ .h)


# Setup project source files
set_project_files(source ${CMAKE_CURRENT_SOURCE_DIR}/ .cpp)


# common header files
list (APPEND SOURCE_FILES ${T3D_COMMON_INCLUDE_DIR}/ImGuiImpl.h ${T3D_COMMON_INCLUDE_DIR}/ImGuiApp.h)


if (TINY3D_OS_WINDOWS)
    # Setup executable project for Windows.
    add_executable(
        ${BIN_NAME} WIN32
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DSystem
        LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
        LINK_PRIVATE T3DMath
        LINK_PRIVATE T3DFramework
        LINK_PRIVATE T3DCore
        LINK_PRIVATE rttr_core
        LINK_PRIVATE imgui_dx11
        LINK_PRIVATE ${SDL2_LIBRARY}
        LINK_PRIVATE TinyImGui
        )
    
    # 在生成工程前，先产生语言文件，并移动到对应文件夹下
    set(LANGUAGE_GENERATOR "generate-en-us.bat")
    set(LAUNCHER_LANGUAGE "lang-en-us")
    
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/LanguageGenerator/Launcher/${LANGUAGE_GENERATOR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Launcher")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Launcher/Language")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Launcher/fonts")
    execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/LanguageGenerator/Launcher/${LAUNCHER_LANGUAGE}.h" "${CMAKE_CURRENT_SOURCE_DIR}/Language/Language.h")
    execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/LanguageGenerator/Launcher/${LAUNCHER_LANGUAGE}.txt" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Launcher/Language/${LAUNCHER_LANGUAGE}.txt")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../../../Assets/fonts/arial.ttf" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Launcher/fonts/arial.ttf")

    # Setup install files and path for Windows.
    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )
elseif (TINY3D_OS_MACOSX)
    # Setup executable project for iOS or Mac OS X.
    add_executable(
        ${BIN_NAME} MACOSX_BUNDLE
        ${SOURCE_FILES}
        )

    
	# Setup all link libraries frameworks for Mac OS X
	target_link_libraries(
		${LIB_NAME}
		LINK_PRIVATE T3DPlatform
		LINK_PRIVATE T3DSystem
		LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
		LINK_PRIVATE T3DMath
		LINK_PRIVATE T3DFramework
		LINK_PRIVATE T3DCore
		LINK_PRIVATE rttr_core
        LINK_PRIVATE ${SDL2_LIBRARY}
        LINK_PRIVATE TinyImGui
		)

	# Setup all properties for this Mac OS X project.
	set_target_properties(${BIN_NAME}
		PROPERTIES
		MACOSX_BUNDLE_BUNDLE_NAME ${BIN_NAME}                                   # Bundle name for this app.
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/Info.plist"   # Info.plist for this app.
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.tiny3d.hello"                         # Bundle ID for this app.
		MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0"                             # Long version string for this app.
		MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"                              # Short version string for this app.
		XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path"
		XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep"
		)

	# Setup installing files for making a *.app for this project
	set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/${BIN_NAME}.app")
	set(DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
	install(CODE "
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")
		" COMPONENT Runtime)

	install(TARGETS ${BIN_NAME}
		BUNDLE DESTINATION bin/debug CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
		)

	set(ASSETS_DIR "${APPS}/Contents/MacOS/assets/")

	add_custom_command(TARGET ${BIN_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../assets/config/Tiny3D.cfg" "${APPS}/Contents/MacOS"
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/*" "${APPS}/Contents/MacOS"
		#COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/Plugins" "${APPS}/Contents/MacOS/Plugins"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../assets/icon ${ASSETS_DIR}/icon
		)
elseif (TINY3D_OS_LINUX)
    # Setup executable project for Linux.
    add_executable(
        ${BIN_NAME}
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DSystem
        LINK_PRIVATE T3DLog
		LINK_PRIVATE T3DUtils
        LINK_PRIVATE T3DFramework
        LINK_PRIVATE T3DCore
        LINK_PRIVATE rttr_core
        LINK_PRIVATE ${SDL2_LIBRARY}
        LINK_PRIVATE TinyImGui
        )

    # Setup install files and path for Windows.
    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )
endif (TINY3D_OS_WINDOWS)


# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Editor/Editor")

