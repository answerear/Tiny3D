#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain.
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)


set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../" CACHE PATH "Tiny3D install prefix")

# macOS 预编译第三方库（FreeImage / ShaderConductor / FBX）目前只有 x86_64。
# 必须在 project() 之前设置，Xcode 生成器才会把 ARCHS 写成 Intel。
# iOS toolchain 会自己设 CMAKE_OSX_ARCHITECTURES，这里不要覆盖。
if (NOT IOS AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    if (NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "macOS build architecture")
    endif ()
endif ()

project(Tiny3D)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)")
endif ()

if (CMAKE_SOURCE_DIR MATCHES " ")
    message("Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif()

if (CMAKE_BINARY_DIR MATCHES " ")
    message("Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause.")
endif()


set(TINY3D_OS "")

if (WIN32)
    # Windows
    message(STATUS "Platform : Windows")

    add_definitions(-DT3D_OS_WINDOWS)
    add_definitions(-DT3D_OS_DESKTOP)
    add_definitions(-D_HAS_EXCEPTIONS=0)

	#add_definitions(/MP)

    set(TINY3D_OS_WINDOWS TRUE CACHE STRING "Windows")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")

    set(TINY3D_OS "Windows" CACHE STRING "OS")

    set(SDL2LIB_SUFFIXES "prebuilt/windows/${MSVC_CXX_ARCHITECTURE_ID}")
    set(PROTOBUF_SUFFIXES "prebuilt/Windows/${MSVC_CXX_ARCHITECTURE_ID}")

    set(CMAKE_SYSTEM_VERSION 10.0.17763)
    set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 10.0.17763.0)

    add_compile_options(/wd4996)
    add_compile_options(/MP)
#    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif (ANDROID)
    # Android
    message(STATUS "Platform : Android")

    add_definitions(-DT3D_OS_ANDROID)
    add_definitions(-DT3D_OS_MOBILE)

    set(TINY3D_OS_ANDROID TRUE CACHE STRING "Android")
    set(TINY3D_OS_MOBILE TRUE CACHE STRING "Mobile")
    set(TINY3D_OS "Android" CACHE STRING "OS")

    set(SDL2LIB_SUFFIXES "prebuilt/android/${ANDROID_ABI}")
    set(PROTOBUF_SUFFIXES "prebuilt/Android/${ANDROID_ABI}")

    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
elseif (APPLE AND IOS)
    # iOS
    message(STATUS "Platform : iOS")

    add_definitions(-DT3D_OS_IOS)
    add_definitions(-DT3D_OS_MOBILE)

    set(TINY3D_OS_IOS TRUE CACHE STRING "iOS")
    set(TINY3D_OS_MOBILE TRUE CACHE STRING "Mobile")
    set(TINY3D_OS "iOS" CACHE STRING "OS")

    set(CMAKE_OSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})

    set(SDL2LIB_SUFFIXES "prebuilt/iOS")
    set(PROTOBUF_SUFFIXES "prebuilt/iOS")

    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wno-undefined-var-template")
    
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    
elseif (APPLE AND NOT IOS)
    # Mac OS X
    message(STATUS "Platform : Mac OS X")

    add_definitions(-DT3D_OS_OSX)
    add_definitions(-DT3D_OS_DESKTOP)

    set(TINY3D_OS_MACOSX TRUE CACHE STRING "Mac OS X")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")
    set(TINY3D_OS "OSX" CACHE STRING "OS")

    set(CMAKE_MACOSX_RPATH true)
    set(SDL2LIB_SUFFIXES "prebuilt/OSX")
    set(PROTOBUF_SUFFIXES "prebuilt/OSX")

    set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-undefined-var-template")

    message(STATUS "macOS architecture : ${CMAKE_OSX_ARCHITECTURES}")
    if (CMAKE_OSX_ARCHITECTURES MATCHES "x86_64")
        # Apple Silicon 上的 Homebrew 是 arm64，Intel 目标不能链 /opt/homebrew。
        list(APPEND CMAKE_IGNORE_PREFIX_PATH "/opt/homebrew")
        if (SDL2MAIN_LIBRARY MATCHES "/opt/homebrew/")
            unset(SDL2MAIN_LIBRARY CACHE)
            unset(SDL2_LIBRARY CACHE)
            unset(SDL2_LIBRARY_TEMP CACHE)
        endif ()
    endif ()
elseif (UNIX AND NOT APPLE)
    # Linux
    message(STATUS "Platform : Linux")

    add_definitions(-DT3D_OS_LINUX)
    add_definitions(-DT3D_OS_DESKTOP)

    set(TINY3D_OS_LINUX TRUE CACHE STRING "Linux")
    set(TINY3D_OS_DESKTOP TRUE CACHE STRING "Desktop")
    set(TINY3D_OS "Linux" CACHE STRING "OS")

    set(SDL2LIB_SUFFIXES "prebuilt/Linux")
    set(PROTOBUF_SUFFIXES "prebuilt/Linux")

    set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wno-undefined-var-template")
endif (WIN32)

# ShaderConductor OSX 预编译库只有 x86_64。若仍按 arm64 配置，则跳过依赖它的主机工具。
set(TINY3D_BUILD_SHADERCONDUCTOR_TOOLS TRUE)
if (TINY3D_OS_MACOSX AND CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
    set(TINY3D_BUILD_SHADERCONDUCTOR_TOOLS FALSE)
    message(STATUS "Skipping ShaderCrossCompiler/BuiltinGenerator (ShaderConductor prebuilt is x86_64-only)")
endif ()


if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DT3D_OS_X64)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_definitions(-DT3D_OS_X86)
endif ()

set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

if (TINY3D_OS_IOS)
    # Because in iOS, there only support static library.
    option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
else (TINY3D_OS_IOS)
    option(TINY3D_BUILD_SHARED_LIBS "Build shared libraries" TRUE)
endif (TINY3D_OS_IOS)

if (NOT TINY3D_OS_ANDROID AND NOT TINY3D_BUILD_RTTR_TOOL)
    # In Android, this switch is setted by build.gradle in Android Studio.
    option(TINY3D_BUILD_SAMPLES "Build samples" TRUE)
endif (NOT TINY3D_OS_ANDROID AND NOT TINY3D_BUILD_RTTR_TOOL)

if (TINY3D_OS_DESKTOP)
    option(TINY3D_BUILD_TOOLS "Build tools" TRUE)
    option(TINY3D_BUILD_EDITOR "Build editor" TRUE)
else (TINY3D_OS_DESKTOP)
    option(TINY3D_BUILD_TOOLS "Build tools" FALSE)
    option(TINY3D_BUILD_EDITOR "Build editor" FALSE)
endif (TINY3D_OS_DESKTOP)

# Set all relative directory
if (TINY3D_OS_ANDROID)
    set(TINY3D_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin/${TINY3D_OS}/${ANDROID_ABI}" CACHE PATH "Tiny3D binary path")
    set(TINY3D_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib/${TINY3D_OS}/${ANDROID_ABI}" CACHE PATH "Tiny3D library path")
else ()
    set(TINY3D_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin/${TINY3D_OS}" CACHE PATH "Tiny3D binary path")
    set(TINY3D_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib/${TINY3D_OS}" CACHE PATH "Tiny3D library path")
endif ()
set(TINY3D_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Tiny3D include path")
set(TINY3D_RES_DIR "${CMAKE_INSTALL_PREFIX}/assets" CACHE PATH "Tiny3D assets path")

# Set this directory for debugging app
if (TINY3D_OS_ANDROID)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${TINY3D_OS}/${ANDROID_ABI}" CACHE STRING "Tiny3D binary files" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/${TINY3D_OS}/${ANDROID_ABI}" CACHE STRING "Tiny3D library files" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/${TINY3D_OS}/${ANDROID_ABI}" CACHE STRING "Tiny3D archive files" FORCE)
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${TINY3D_OS}" CACHE STRING "Tiny3D binary files" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/${TINY3D_OS}" CACHE STRING "Tiny3D library files" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/${TINY3D_OS}" CACHE STRING "Tiny3D archive files" FORCE)
endif ()


message(STATUS "Runtime dir : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} BUILTTYPE)
    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}" CACHE STRING "Tiny3D binary files ${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}" CACHE STRING "Tiny3D library files ${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}" CACHE STRING "Tiny3D archive files ${OUTPUTCONFIG}")
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Set cmake module path.
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages"
    )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# Platform error code
add_definitions(-DT3D_ERR_PLATFORM=0x00001000)
# Log error code
add_definitions(-DT3D_ERR_LOG=0x00002000)
# Framework error code
add_definitions(-DT3D_ERR_FRAMEWORK=0x00003000)
# Core error code
add_definitions(-DT3D_ERR_CORE=0x00004000)
# Plugin error code
add_definitions(-DT3D_ERR_PLUGINS=0x00010000)
# User error code
add_definitions(-DT3D_ERR_USER=0x00020000)

include(FindPkgMacros)
include(ProjectCMakeModule)

#-------------------------------------------------------------------------------
# 反射配置由 CMake 原生生成。ON 走新链路，OFF 退回 cct / nmake 旧链路。
#
#   ON  : ReflectionSettings.json 由 file(GENERATE) 从 target 属性写出，rpp 只在
#         构建期跑，产物清单在 configure 期按源文件预声明。
#   OFF : 配置来自 generate-*.bat 里 cct 写进 nmake/<Module>/ 的那份，产物靠
#         glob 产物目录收集。
#
# 必须声明在 Tiny3DReflectHelpers 之前：那个文件为独立的 GamePlugin 工程准备了同名
# 默认值，先 include 会让它建出一个普通变量，随后的 option() 再把它清掉（CMP0077）。
#
# 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §8 阶段 2
#-------------------------------------------------------------------------------
option(TINY3D_REFLECT_CMAKE_NATIVE
    "Generate ReflectionSettings.json from CMake instead of the cct / nmake pipeline" ON)

include(Tiny3DReflectHelpers)

# Common headers directory
set(T3D_COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Common")

# Search SDL2 library for all projects.
set(SDL2DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/sdl2" CACHE PATH "SDL2 library path")
find_package(SDL2)

# T3DPrerequisites.h includes SDL_main.h on Apple. Directory-level include_directories
# are inherited by all subdirectories, so plugins/tools do not each need to repeat it.
if (SDL2_INCLUDE_DIR)
    include_directories("${SDL2_INCLUDE_DIR}")
endif ()

# Search GLAD library for OpenGL4 renderer.
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/glad" CACHE PATH "GLAD library path")
find_package(GLAD)

# Search glslang library for OpenGL4 renderer.
set(GLSLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/glslang" CACHE PATH "glslang library path")
find_package(Glslang)

# Search protobuf library for all projects.
#set(PROTOBUF_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/protobuf" CACHE PATH "protobuf library path")
#find_package(protobuf)

# Search rapidjson library for all projects.
#set(RAPIDJSON_HOME "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/rapidjson" CACHE PATH "rapidjson library path")
#find_package(rapidjson)



# Project rapidjson
set(RAPIDJSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/rapidjson")

if (NOT TINY3D_BUILD_RTTR_TOOL)
	# Project protobuf
	option (protobuf_BUILD_LITE_LIBRARIES "Build protobuf-lite library" OFF)
	set(PROTOBUF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/protobuf/src")
	
	# Project rttr
	set(RTTR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/rttr/src" "${CMAKE_CURRENT_BINARY_DIR}/External/rttr/src")
	message(STATUS "rttr include : ${RTTR_INCLUDE_DIR}")

    # All Third Parties Sources
    add_subdirectory(External)

    # Setup project folder
	set(protobuf_VERBOSE TRUE)
	#set(protobuf_HAVE_BUILTIN_ATOMICS TRUE)
	if (protobuf_BUILD_LITE_LIBRARIES)
		set_property(TARGET libprotobuf-lite PROPERTY FOLDER "External/protobuf")
		set (PROTOBUF_LIBRARY libprotobuf-lite)
	else (protobuf_BUILD_LITE_LIBRARIES)
		set_property(TARGET libprotobuf PROPERTY FOLDER "External/protobuf")
		set_property(TARGET libprotobuf-lite PROPERTY FOLDER "External/protobuf")
		set (PROTOBUF_LIBRARY libprotobuf)
	endif (protobuf_BUILD_LITE_LIBRARIES)
	
	set_property(TARGET rttr_core PROPERTY FOLDER "External/rttr")
endif (NOT TINY3D_BUILD_RTTR_TOOL)

# Pre-generated reflection source base directory.
# Cross-compilation (Android etc.) reuses the host build's generated .cpp files.
if (CMAKE_CROSSCOMPILING)
    set(TINY3D_GENERATED_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vs2019-x64" CACHE PATH
        "Base directory containing pre-generated reflection .cpp files for cross-compilation")
else()
    set(TINY3D_GENERATED_BASE_DIR "${CMAKE_BINARY_DIR}" CACHE PATH
        "Base directory containing generated reflection .cpp files (native build)")
endif()

# Project Reflect (incremental reflection generation, can be disabled in CI)
option(TINY3D_INCREMENTAL_RTTR "Enable incremental RTTR reflection generation during build" ON)

if (TINY3D_REFLECT_CMAKE_NATIVE AND NOT TINY3D_INCREMENTAL_RTTR)
    # 产物是预声明的，没人生成它们的话构建期一定找不到文件。与其编到一半报
    # 「file not found」，不如在这里说清楚两个开关的关系。
    message(FATAL_ERROR
        "TINY3D_INCREMENTAL_RTTR=OFF 时反射产物没有生成者，而 "
        "TINY3D_REFLECT_CMAKE_NATIVE=ON 会把它们预声明进各模块。\n"
        "  要跳过反射生成请同时设置 -DTINY3D_REFLECT_CMAKE_NATIVE=OFF（走旧链路，"
        "复用 generate-*.bat 预先产出的产物）。")
endif ()

if (TINY3D_INCREMENTAL_RTTR)
    add_subdirectory(Reflect)
endif ()

# Project Platform
add_subdirectory(Platform)
# Project Log
add_subdirectory(Log)
# Project Utils
add_subdirectory(Utils)

if (NOT TINY3D_BUILD_RTTR_TOOL)
	# Project System
	add_subdirectory(System)
    # Project Framework
    add_subdirectory(Framework)
    # Project Math
    add_subdirectory(Math)
    # Project Core
    add_subdirectory(Core)
    
    # Project plugins
    add_subdirectory(Plugins)
    
    # Project editor
    if (TINY3D_BUILD_EDITOR)
        add_subdirectory(Editor)
        add_subdirectory(Player)
    endif (TINY3D_BUILD_EDITOR)
endif (NOT TINY3D_BUILD_RTTR_TOOL)


#foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
#    string(TOUPPER ${OUTPUTCONFIG} BUILTTYPE)
#    set(RUNTIME_OUTPUT_DIR_TEMP_${BUILTTYPE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE}}")
#    set(LIBRARY_OUTPUT_DIR_TEMP_${BUILTTYPE} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE}}")
#    set(ARCHIVE_OUTPUT_DIR_TEMP_${BUILTTYPE} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE}}")
#    
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE}}/Plugins")
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE}}/Plugins")
#    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE}}/Plugins")
#endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

#foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
#    string(TOUPPER ${OUTPUTCONFIG} BUILTTYPE)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILTTYPE} "${RUNTIME_OUTPUT_DIR_TEMP_${BUILTTYPE}}")
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILTTYPE} "${LIBRARY_OUTPUT_DIR_TEMP_${BUILTTYPE}}")
#    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILTTYPE} "${ARCHIVE_OUTPUT_DIR_TEMP_${BUILTTYPE}}")
#endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Set the dependencies for all projects.
add_dependencies(T3DLog T3DPlatform)
add_dependencies(T3DUtils T3DLog T3DPlatform)


if (NOT TINY3D_BUILD_RTTR_TOOL)
    # 新链路下每个模块只依赖自己那个 reflect target（见文件末尾的接线），不必再
    # 挂伞形 target —— 那会让 T3DSystem 的编译排在 Core 的反射生成之后
    if (TINY3D_INCREMENTAL_RTTR AND NOT TINY3D_REFLECT_CMAKE_NATIVE)
	    add_dependencies(T3DSystem T3DReflect rttr_core T3DPlatform)
    else ()
        add_dependencies(T3DSystem rttr_core T3DPlatform)
    endif ()
    add_dependencies(T3DMath T3DLog T3DPlatform)
    add_dependencies(T3DFramework T3DLog T3DPlatform)
    #add_dependencies(T3DCore ${PROTOBUF_LIBRARY} T3DMath T3DFramework T3DUtils T3DLog T3DSystem)
    add_dependencies(T3DCore T3DMath T3DFramework T3DUtils T3DLog T3DSystem T3DPlatform)

    if (TINY3D_BUILD_EDITOR)
        add_dependencies(T3DCoreEditor T3DMath T3DFramework T3DUtils T3DLog T3DSystem T3DPlatform)
        add_dependencies(TinyEditor ${PROTOBUF_LIBRARY} T3DCoreEditor)
        add_dependencies(TinyLauncher ${PROTOBUF_LIBRARY} T3DCoreEditor)
        add_dependencies(TinyPlayer T3DCore)
    endif (TINY3D_BUILD_EDITOR)
endif (NOT TINY3D_BUILD_RTTR_TOOL)

if (TINY3D_BUILD_TOOLS)
    add_subdirectory(Tools)
    if (NOT TINY3D_BUILD_RTTR_TOOL AND TINY3D_BUILD_SHADERCONDUCTOR_TOOLS)
	    add_dependencies(ShaderCrossCompiler T3DCore)
	    add_dependencies(BuiltinGenerator T3DCore)
        #add_dependencies(T3DPostBuild MetaGenerator)
	endif (NOT TINY3D_BUILD_RTTR_TOOL AND TINY3D_BUILD_SHADERCONDUCTOR_TOOLS)
endif(TINY3D_BUILD_TOOLS)


if (TINY3D_BUILD_SAMPLES)
    # Build samples. Set all projects of samples.
    add_subdirectory(Samples)
    if (NOT TINY3D_OS_MACOSX)
        add_dependencies(HelloApp T3DCore)
    endif (NOT TINY3D_OS_MACOSX)
    add_dependencies(CameraApp T3DCore)
    add_dependencies(GeometryApp T3DCore)
    add_dependencies(TextureApp T3DCore)
    add_dependencies(SceneApp T3DCore)
    add_dependencies(DirectionalLightApp T3DCore)
    add_dependencies(PointLightApp T3DCore)
    add_dependencies(SpotLightApp T3DCore)
    add_dependencies(LightApp T3DCore)
    add_dependencies(ShadowApp T3DCore)
    add_dependencies(BehaviourApp T3DCore)
    add_dependencies(PostProcessingApp T3DCore)
    add_dependencies(InputApp T3DCore)
    add_dependencies(SkeletalAnimationApp T3DCore)
    add_dependencies(ResourceApp T3DCore)
    add_dependencies(SkyboxApp T3DCore)
    if (TINY3D_OS_DESKTOP)
        add_dependencies(TransformationApp T3DMath T3DLog T3DSystem)
        add_dependencies(IntersectionApp T3DMath T3DLog T3DSystem)
        add_dependencies(PrefabApp T3DCore)
    endif (TINY3D_OS_DESKTOP)
endif (TINY3D_BUILD_SAMPLES)

# 业务 Game Plugin 工程要用和编辑器一致的工具链编译，把当前构建的实际配置导出给它们。
if (TINY3D_BUILD_EDITOR AND NOT TINY3D_BUILD_RTTR_TOOL)
    include(GenerateTiny3DSDK)
    tiny3d_generate_sdk_config()
endif ()

# 编辑器与 Sample 是两条独立的资源链路，同一份内置资源要在两边各留一份且 UUID 一致。
# 这个 target 手工触发（不接进任何构建流程），把 BuiltinGenerator 产出的天空盒资源
# 同步到 assets/samples/meshes，并用 scc 烘焙出全语言变体的 .tshader。
if (TINY3D_BUILD_TOOLS AND NOT TINY3D_BUILD_RTTR_TOOL AND TINY3D_BUILD_SHADERCONDUCTOR_TOOLS)
    add_custom_target(sync_builtin_to_samples
        COMMAND ${CMAKE_COMMAND}
            -DBUILTIN_DIR=${CMAKE_SOURCE_DIR}/../assets/editor/builtin
            -DSAMPLES_DIR=${CMAKE_SOURCE_DIR}/../assets/samples/meshes
            -DSCC_EXECUTABLE=$<TARGET_FILE:ShaderCrossCompiler>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils/SyncBuiltinToSamples.cmake
        COMMENT "Syncing builtin skybox assets to assets/samples/meshes")

    add_dependencies(sync_builtin_to_samples ShaderCrossCompiler)

    set_property(TARGET sync_builtin_to_samples PROPERTY FOLDER "CMakeTargets")
endif ()

#-------------------------------------------------------------------------------
# 反射接线：为每个模块写出 ReflectionSettings.json，并挂一个构建期跑 rpp 的 target。
#
# 放在文件末尾而不是各模块的 CMakeLists 里，是因为 tiny3d_write_reflection_settings
# 会读目录级的 COMPILE_DEFINITIONS 兜底（补 add_library 之后才 add_definitions 的那
# 些）。在模块目录里调用会连带把 add_definitions 塞进去的编译选项一起收进来，而在
# 这里调用取到的是 source/ 的目录属性，模块自己的宏则由 target 属性提供 —— 这正是
# 阶段 1 逐字段比对通过的那个口径。
#
# 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §6 / §8 阶段 2
#-------------------------------------------------------------------------------
if (TINY3D_REFLECT_CMAKE_NATIVE AND TINY3D_INCREMENTAL_RTTR AND NOT TINY3D_BUILD_RTTR_TOOL)
    # 不打标签也自动反射的模板。散在 .bat / build.yml / Reflect/CMakeLists.txt 三处
    # 的白名单收拢到这里，成为唯一来源。
    set(_t3d_reflect_white_list
        Tiny3D::TAabb Tiny3D::TCapsule Tiny3D::TDegree Tiny3D::TFrustum
        Tiny3D::TMatrix2 Tiny3D::TMatrix3 Tiny3D::TMatrix4 Tiny3D::TObb
        Tiny3D::TPlane Tiny3D::TQuaternion Tiny3D::TRadian Tiny3D::TRay
        Tiny3D::TSize Tiny3D::TPoint Tiny3D::TRect Tiny3D::TSphere
        Tiny3D::TTriangle Tiny3D::TVector2 Tiny3D::TVector3 Tiny3D::TVector4)

    # <target> <相对 source 的 rpp 扫描根> <相对 TINY3D_GENERATED_BASE_DIR 的产物目录>
    # 三元组与各模块 tiny3d_declare_generated_sources 的产物目录必须一一对应。
    set(_t3d_reflect_modules
        "T3DSystem|System|System"
        "T3DMath|Math|Math"
        "T3DCore|Core|Core/Runtime")

    if (TINY3D_BUILD_EDITOR)
        list(APPEND _t3d_reflect_modules
            "T3DCoreEditor|Core|Core/Editor"
            "TinyLauncher|Editor/TinyLauncher|Editor/TinyLauncher"
            "TinyEditor|Editor/TinyEditor|Editor/TinyEditor")
    endif ()

    foreach (_entry IN LISTS _t3d_reflect_modules)
        string(REPLACE "|" ";" _parts "${_entry}")
        list(GET _parts 0 _tgt)
        list(GET _parts 1 _src_rel)
        list(GET _parts 2 _gen_rel)

        if (TARGET ${_tgt})
            # 白名单只有扫 Core 的两个模块需要：Math 的类模板定义在 source/Math，
            # 从 Core 看过去是「外部模板」，不点名就不会被实例化。
            set(_white "")
            if (_src_rel STREQUAL "Core")
                set(_white ${_t3d_reflect_white_list})
            endif ()

            tiny3d_add_reflection_target(${_tgt}
                SETTINGS_DIR  "${CMAKE_BINARY_DIR}/Reflect/${_gen_rel}"
                SOURCE_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/${_src_rel}"
                GENERATED_DIR "${TINY3D_GENERATED_BASE_DIR}/${_gen_rel}/Generated"
                WHITE_LIST    ${_white})
        else ()
            message(STATUS "Reflect: 跳过 ${_tgt}（target 未定义）")
        endif ()
    endforeach ()

    unset(_t3d_reflect_modules)
    unset(_t3d_reflect_white_list)
endif ()

