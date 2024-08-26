#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

set_project_name(ReflectionPreprocessor)


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")

    # Disable some warning for protobuf
    add_definitions(
        /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
        /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
        /wd4309 # 'conversion' : truncation of constant value
        /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    )
endif (MSVC)

set(LLVM_HOME "${TINY3D_DEP_DIR}/llvm" CACHE PATH "llvm home path")

if (TINY3D_OS_WINDOWS)
    set(LLVM_LIB_SUFFIXES "prebuilt/Windows/${MSVC_CXX_ARCHITECTURE_ID}")
    #set(LLVM_LIB_DEBUG_SUFFIXES "prebuilt/Windows/Debug")
elseif (TINY3D_OS_MACOSX)
    set(LLVM_LIB_SUFFIXES "prebuilt/OSX")
elseif (TINY3D_OS_LINUX)
    set(LLVM_LIB_SUFFIXES "prebuilt/Linux")
endif (TINY3D_OS_WINDOWS)


set(LLVM_INCLUDE_DIR "${LLVM_HOME}/Include")
set(LLVM_LIB_DIR "${LLVM_HOME}/${LLVM_LIB_SUFFIXES}")
set(LLVM_BIN_DIR "${LLVM_HOME}/${LLVM_LIB_SUFFIXES}")
#set(LLVM_LIB_DEBUG_DIR "${LLVM_HOME}/${LLVM_LIB_DEBUG_SUFFIXES}")

# Setup project include files path
include_directories(
    "${TINY3D_PLATFORM_INC_DIR}"
    "${TINY3D_LOG_INC_DIR}"
    "${TINY3D_UTILS_INC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${RAPIDJSON_INCLUDE_DIR}"
    #"${RTTR_INCLUDE_DIR}"
    "${SDL2_INCLUDE_DIR}"
    "${LLVM_INCLUDE_DIR}"
    )

# Setup project header files
set_project_files(Include ${CMAKE_CURRENT_SOURCE_DIR}/Include/ .h)


# Setup project source files
set_project_files(Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ .cpp)


if (TINY3D_OS_WINDOWS)
    # Setup executable project for Windows.
    
    add_compile_options("/wd4251")
    
    add_executable(
        ${BIN_NAME} WIN32
        ${SOURCE_FILES}
        )

    #message(STATUS "Project include : ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}")
    #get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    #foreach(dir ${dirs})
    #  message(STATUS "dir='${dir}'")
    #endforeach()
    
    target_link_libraries(
        ${LIB_NAME}
        T3DPlatform
        T3DLog
        T3DUtils
        #rttr_core
        #Version.lib
        #debug ${LLVM_LIB_DEBUG_DIR}/libclang.lib
        #optimized ${LLVM_LIB_DIR}/libclang.lib
        ${LLVM_LIB_DIR}/libclang.lib
        )

    set_target_properties(${BIN_NAME} PROPERTIES OUTPUT_NAME rpp)

    if (TINY3D_BUILD_RTTR_TOOL)
        add_custom_command(TARGET ${BIN_NAME}
            PRE_LINK
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
            COMMAND ${CMAKE_COMMAND} -E copy ${LLVM_BIN_DIR}/libclang.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
            COMMAND ${CMAKE_COMMAND} -E copy ${LLVM_BIN_DIR}/libclang.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
            COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
            COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
            )
    endif (TINY3D_BUILD_RTTR_TOOL)

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
        LINK_PRIVATE T3DLog
        )

    set_target_properties(${BIN_NAME} PROPERTIES OUTPUT_NAME rpp)

    # Setup all properties for this Mac OS X project.
#   set_target_properties(${BIN_NAME}
#       PROPERTIES
#       MACOSX_BUNDLE_BUNDLE_NAME ${BIN_NAME}                                   # Bundle name for this app.
#       MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/Info.plist"   # Info.plist for this app.
#       MACOSX_BUNDLE_GUI_IDENTIFIER "com.tiny3d.hello"                         # Bundle ID for this app.
#       MACOSX_BUNDLE_LONG_VERSION_STRING "0.1.0.0"                             # Long version string for this app.
#       MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"                              # Short version string for this app.
#       XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path"
#       )

    # Setup installing files for making a *.app for this project
#   set(APPS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/${BIN_NAME}.app")
#   set(DIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
#   install(CODE "
#       include(BundleUtilities)
#       fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")
#       " COMPONENT Runtime)

    install(TARGETS ${BIN_NAME}
        BUNDLE DESTINATION bin/debug CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )

#   set(ASSETS_DIR "${APPS}/Contents/MacOS/Assets/")

#   add_custom_command(TARGET ${BIN_NAME}
#       POST_BUILD
#       COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../../assets/config/OSX/Tiny3D.cfg" "${APPS}/Contents/MacOS"
#       COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/*" "${APPS}/Contents/MacOS"
#       COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
#       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/../../Assets/Icon ${ASSETS_DIR}/Icon
#       )
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
        )

    set_target_properties(${BIN_NAME} PROPERTIES OUTPUT_NAME rpp)

    # Setup install files and path for Windows.
    install(TARGETS ${BIN_NAME}
        RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
        LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
        )
endif (TINY3D_OS_WINDOWS)


# Setup project folder
set_property(TARGET ${BIN_NAME} PROPERTY FOLDER "Tools")
