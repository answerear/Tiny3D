#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(ScriptCompiler)


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
endif (MSVC)

set(SHADERCONDUCTOR_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../../../../dependencies/ShaderConductor" CACHE PATH "ShaderConductor library path")

if (TINY3D_OS_WINDOWS)
	set(SHADERCONDUCTOR_LIB_SUFFIXES "prebuilt/win32/${MSVC_CXX_ARCHITECTURE_ID}")
elseif (TINY3D_OS_OSX)
	set(SHADERCONDUCTOR_LIB_SUFFIXES "prebuilt/OSX")
elseif (TINY3D_OS_LINUX)
	set(SHADERCONDUCTOR_LIB_SUFFIXES "prebuilt/Linux")
endif (TINY3D_OS_WINDOWS)
	
find_package(shaderConductor)

# Setup project include files path
include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
	"${TINY3D_UTILS_INC_DIR}"
    "${TINY3D_MATH_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${SHADERCONDUCTOR_INCLUDE_DIR}"
    )

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)


# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)



if (TINY3D_OS_WINDOWS)
    # Setup executable project for Windows.
    add_executable(
        ${BIN_NAME} WIN32
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        T3DPlatform
        T3DLog
		T3DUtils
        T3DMath
		${SHADERCONDUCTOR_LIBRARY}
        )

    # Setup project folder
    set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Tools")

	add_custom_command(TARGET ${BIN_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADERCONDUCTOR_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADERCONDUCTOR_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        )

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
		T3DPlatform
		T3DLog
		T3DUtils
        T3DMath
		)

	# Setup all properties for this Mac OS X project.
#	set_target_properties(${BIN_NAME}
#		PROPERTIES
#		MACOSX_BUNDLE_BUNDLE_NAME ${BIN_NAME}                                   # Bundle name for this app.
#		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/Info.plist"   # Info.plist for this app.
#		MACOSX_BUNDLE_GUI_IDENTIFIER "com.tiny3d.hello"                         # Bundle ID for this app.
#		MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0"                             # Long version string for this app.
#		MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"                              # Short version string for this app.
#		XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path"
#		)

	# Setup installing files for making a *.app for this project
#	set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/${BIN_NAME}.app")
#	set(DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
#	install(CODE "
#		include(BundleUtilities)
#		fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")
#		" COMPONENT Runtime)

	install(TARGETS ${BIN_NAME}
		BUNDLE DESTINATION bin/debug CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
		LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
		ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
		)

#	set(ASSETS_DIR "${APPS}/Contents/MacOS/Assets/")

#	add_custom_command(TARGET ${BIN_NAME}
#		POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../../assets/config/OSX/Tiny3D.cfg" "${APPS}/Contents/MacOS"
#		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/*" "${APPS}/Contents/MacOS"
#		COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
#		COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../../Assets/Icon ${ASSETS_DIR}/Icon
#		)
elseif (TINY3D_OS_LINUX)
    # Setup executable project for Linux.
    add_executable(
        ${BIN_NAME}
        ${SOURCE_FILES}
        )

    target_link_libraries(
        ${LIB_NAME}
        T3DPlatform
        T3DLog
		T3DUtils
        T3DMath
        )

    # Setup install files and path for Windows.
    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )
endif (TINY3D_OS_WINDOWS)


