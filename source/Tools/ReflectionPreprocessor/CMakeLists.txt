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


set(LLVM_INCLUDE_DIR "${LLVM_HOME}/include")
set(LLVM_LIB_DIR "${LLVM_HOME}/${LLVM_LIB_SUFFIXES}")
set(LLVM_BIN_DIR "${LLVM_HOME}/${LLVM_LIB_SUFFIXES}")
set(TINY3D_CLANG_RESOURCE_SRC "${LLVM_BIN_DIR}/clang-resource")
#set(LLVM_LIB_DEBUG_DIR "${LLVM_HOME}/${LLVM_LIB_DEBUG_SUFFIXES}")

# macOS 26 SDK 的 libc++ 依赖 Clang 16+ 的 __remove_cv 等 builtin。
# 仓库里预编译的是 LLVM 15 libclang，解析标准库头会整片失败。
# rpp 是本机工具，优先链接当前 Xcode 的 libclang。
if (TINY3D_OS_MACOSX)
    execute_process(
        COMMAND xcrun --find clang
        OUTPUT_VARIABLE _tiny3d_xcode_clang
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    if (_tiny3d_xcode_clang)
        get_filename_component(_tiny3d_xcode_bin "${_tiny3d_xcode_clang}" DIRECTORY)
        get_filename_component(_tiny3d_xcode_usr "${_tiny3d_xcode_bin}" DIRECTORY)
        set(_tiny3d_xcode_libclang "${_tiny3d_xcode_usr}/lib/libclang.dylib")
        if (EXISTS "${_tiny3d_xcode_libclang}")
            execute_process(
                COMMAND xcrun clang -print-resource-dir
                OUTPUT_VARIABLE _tiny3d_xcode_resource
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
            set(LLVM_LIB_DIR "${_tiny3d_xcode_usr}/lib")
            set(LLVM_BIN_DIR "${_tiny3d_xcode_usr}/lib")
            if (EXISTS "${_tiny3d_xcode_usr}/include/clang-c/Index.h")
                set(LLVM_INCLUDE_DIR "${_tiny3d_xcode_usr}/include")
            endif ()
            if (_tiny3d_xcode_resource AND EXISTS "${_tiny3d_xcode_resource}/include")
                set(TINY3D_CLANG_RESOURCE_SRC "${_tiny3d_xcode_resource}")
            endif ()
            message(STATUS "rpp uses Xcode libclang: ${_tiny3d_xcode_libclang}")
            message(STATUS "rpp clang resource dir: ${TINY3D_CLANG_RESOURCE_SRC}")
        endif ()
        unset(_tiny3d_xcode_libclang)
        unset(_tiny3d_xcode_resource)
        unset(_tiny3d_xcode_usr)
        unset(_tiny3d_xcode_bin)
    endif ()
    unset(_tiny3d_xcode_clang)
endif ()

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

    # Game Plugin 与 Sample 在编辑器 bin 目录调 rpp，libclang.dll 必须始终跟过去。
    add_custom_command(TARGET ${BIN_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        COMMAND ${CMAKE_COMMAND} -E copy ${LLVM_BIN_DIR}/libclang.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E copy ${LLVM_BIN_DIR}/libclang.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
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
        LINK_PRIVATE T3DPlatform
        LINK_PRIVATE T3DLog
        LINK_PRIVATE T3DUtils
        LINK_PRIVATE ${LLVM_LIB_DIR}/libclang.dylib
        )

    # BUILD_RPATH 是追加在 CMake 自动推导的构建树 rpath 之上的，不要设
    # BUILD_WITH_INSTALL_RPATH，否则 lib/OSX 下的 libT3D*.dylib 会找不到。
    # rpp 是本机命令行工具，不是要分发的 GUI。关掉签名：clang-resource 是头文件，
    # 不能放在 Contents/MacOS 里当嵌套代码签，Xcode 会报
    # "code object is not signed at all / __stddef_header_macro.h"。
    set_target_properties(${BIN_NAME} PROPERTIES
        OUTPUT_NAME rpp
        MACOSX_RPATH 1
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")

    # rpp 是本机工具；macOS 上优先链 Xcode libclang，才能解析当前 SDK 的 libc++。

    # libclang 的 install name 是 @rpath/…，必须跟 rpp 放在同一目录才能加载。
    # clang-resource 是编译器内建头，必须放进 Contents/Resources，不能进 MacOS。
    set(_tiny3d_rpp_runtime_copy
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LLVM_BIN_DIR}/libclang.dylib"
            "$<TARGET_FILE_DIR:${BIN_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E rm -rf
            "$<TARGET_FILE_DIR:${BIN_NAME}>/clang-resource"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${TINY3D_CLANG_RESOURCE_SRC}"
            "$<TARGET_FILE_DIR:${BIN_NAME}>/../Resources/clang-resource"
    )
    if (EXISTS "${LLVM_BIN_DIR}/libzstd.1.dylib")
        list(APPEND _tiny3d_rpp_runtime_copy
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${LLVM_BIN_DIR}/libzstd.1.dylib"
                "$<TARGET_FILE_DIR:${BIN_NAME}>")
    endif ()
    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        ${_tiny3d_rpp_runtime_copy}
        COMMENT "Copying libclang runtime next to rpp")
    unset(_tiny3d_rpp_runtime_copy)

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
