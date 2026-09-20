#-------------------------------------------------------------------------------
# 工具链自带头文件路径的探测
#
# rpp 用的 libclang 不做工具链自动发现：MSVC 那套注册表 / COM 探测在它身上不生效，
# NDK / Xcode 的 sysroot 它也不会自己找。这些路径必须显式写进 ReflectionSettings.json
# 的 SystemIncludePath（以 -isystem 传给 clang）和 OtherFlags。
#
# 过去这份信息来自 cct 读取的 %INCLUDE%，于是整条反射链路被绑死在「Windows 上、
# vcvarsall 起的环境里」。这里改为从 CMake 自身的工具链信息推导，让每个平台的构建
# 各自得到与**当次编译**一致的那一套头文件。
#
# 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §4
#-------------------------------------------------------------------------------

include_guard(GLOBAL)

#-------------------------------------------------------------------------------
# _tiny3d_append_existing_dir(<list_var> <dir>...)
#
# 目录不存在就跳过：探测出来的候选路径本来就允许落空（比如没装 ATLMFC），
# 把不存在的目录喂给 clang 只会多一条无用的 -isystem。
#-------------------------------------------------------------------------------
function(_tiny3d_append_existing_dir LIST_VAR)
    set(_result ${${LIST_VAR}})
    foreach (_dir IN LISTS ARGN)
        if (_dir AND IS_DIRECTORY "${_dir}")
            file(TO_CMAKE_PATH "${_dir}" _dir)
            list(APPEND _result "${_dir}")
        endif ()
    endforeach ()
    if (_result)
        list(REMOVE_DUPLICATES _result)
    endif ()
    set(${LIST_VAR} "${_result}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_detect_msvc_includes(<out_includes>)
#
# 三级来源，优先级从高到低：
#   1. %INCLUDE%      —— 在开发者环境里 configure 时直接可用，与 cct 的行为等价
#   2. 编译器路径反推 —— 一定命中**实际编译用的**那个工具集，比 rpp 自带的
#                        「扫默认安装位置挑版本号最大的」兜底可靠
#   3. 都没有         —— 交给调用方裁决（阶段 1 只告警，切换主链路后应当报错）
#-------------------------------------------------------------------------------
function(_tiny3d_detect_msvc_includes OUT_INCLUDES)
    set(_includes "")

    # ---- 第 1 级：环境变量 -------------------------------------------------
    # EXTERNAL_INCLUDE 是新版 vcvarsall 拆出去的一份，一并收下（与 cct 一致）
    foreach (_env_name INCLUDE EXTERNAL_INCLUDE)
        if (DEFINED ENV{${_env_name}} AND NOT "$ENV{${_env_name}}" STREQUAL "")
            # %INCLUDE% 是 ';' 分隔，而 ';' 在 CMake 里正好是列表分隔符
            string(REPLACE ";" "\\;" _raw "$ENV{${_env_name}}")
            string(REPLACE "\\;" ";" _entries "${_raw}")
            _tiny3d_append_existing_dir(_includes ${_entries})
        endif ()
    endforeach ()

    if (_includes)
        set(${OUT_INCLUDES} "${_includes}" PARENT_SCOPE)
        return()
    endif ()

    # ---- 第 2 级：从 CMAKE_CXX_COMPILER 反推 -------------------------------
    # cl.exe 的位置形如
    #   <VS>/VC/Tools/MSVC/<toolset>/bin/Host<arch>/<arch>/cl.exe
    # 往上四级即工具集根目录。
    set(_toolset_root "${CMAKE_CXX_COMPILER}")
    foreach (_i RANGE 3)
        get_filename_component(_toolset_root "${_toolset_root}" DIRECTORY)
    endforeach ()

    if (EXISTS "${_toolset_root}/include/vcruntime.h")
        _tiny3d_append_existing_dir(_includes
            "${_toolset_root}/include"
            "${_toolset_root}/ATLMFC/include")

        # <VS>/VC/Auxiliary/VS/include，从工具集根再往上三级到 VC
        get_filename_component(_vc_root "${_toolset_root}" DIRECTORY)   # MSVC
        get_filename_component(_vc_root "${_vc_root}" DIRECTORY)        # Tools
        get_filename_component(_vc_root "${_vc_root}" DIRECTORY)        # VC
        _tiny3d_append_existing_dir(_includes "${_vc_root}/Auxiliary/VS/include")
    endif ()

    # Windows SDK。版本号优先用 CMake 解析出来的那个，保证与编译一致。
    set(_sdk_version "")
    foreach (_candidate
            "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
            "${CMAKE_SYSTEM_VERSION}")
        if (_candidate AND _candidate MATCHES "^10\\.")
            set(_sdk_version "${_candidate}")
            break()
        endif ()
    endforeach ()

    # KitsRoot10 是安装器写的权威位置；装在非默认盘时只有它是对的
    set(_kits_root "")
    get_filename_component(_kits_root
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]"
        ABSOLUTE CACHE)
    if (NOT _kits_root OR NOT IS_DIRECTORY "${_kits_root}")
        set(_kits_root "$ENV{ProgramFiles\(x86\)}/Windows Kits/10")
    endif ()

    if (IS_DIRECTORY "${_kits_root}")
        # 版本号拿不到就退而挑目录里最大的那个（um 与 ucrt 齐备才算数）
        if (NOT _sdk_version)
            file(GLOB _sdk_dirs RELATIVE "${_kits_root}/Include" "${_kits_root}/Include/*")
            foreach (_dir IN LISTS _sdk_dirs)
                if (IS_DIRECTORY "${_kits_root}/Include/${_dir}/um"
                        AND IS_DIRECTORY "${_kits_root}/Include/${_dir}/ucrt"
                        AND "${_dir}" STRGREATER "${_sdk_version}")
                    set(_sdk_version "${_dir}")
                endif ()
            endforeach ()
        endif ()

        if (_sdk_version)
            # ucrt 给 C 运行库，um 给 Win32 API（WinSock.h 在这儿），shared 给
            # 两者共用的定义，winrt / cppwinrt 少数头会用到
            foreach (_sub ucrt um shared winrt cppwinrt)
                _tiny3d_append_existing_dir(_includes
                    "${_kits_root}/Include/${_sdk_version}/${_sub}")
            endforeach ()
        endif ()
    endif ()

    set(${OUT_INCLUDES} "${_includes}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_android_arch_triple(<out>)
#
# NDK 把架构相关的头放在 sysroot/usr/include/<triple>/ 下。CMAKE_CXX_COMPILER_TARGET
# 带 API 等级和 "none"（aarch64-none-linux-android26），目录名没有这两段。
#-------------------------------------------------------------------------------
function(_tiny3d_android_arch_triple OUT_VAR)
    set(_triple "")
    foreach (_candidate
            "${CMAKE_LIBRARY_ARCHITECTURE}"
            "${ANDROID_LLVM_TRIPLE}")
        if (_candidate)
            set(_triple "${_candidate}")
            break()
        endif ()
    endforeach ()

    if (NOT _triple)
        if (ANDROID_ABI STREQUAL "arm64-v8a")
            set(_triple "aarch64-linux-android")
        elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
            set(_triple "arm-linux-androideabi")
        elseif (ANDROID_ABI STREQUAL "x86")
            set(_triple "i686-linux-android")
        elseif (ANDROID_ABI STREQUAL "x86_64")
            set(_triple "x86_64-linux-android")
        endif ()
    endif ()

    set(${OUT_VAR} "${_triple}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_detect_android_includes(<out_includes>)
#
# host 上的 rpp 用的是 Windows 编出来的 libclang，不会自动带上 NDK 的默认搜索路径。
# 只给 usr/include 的话 <vector> 都找不到。优先用 CMake 从 NDK clang 问来的隐式
# 搜索路径，拿不到再按 NDK 布局手拼。
#-------------------------------------------------------------------------------
function(_tiny3d_detect_android_includes OUT_INCLUDES)
    set(_includes "")

    _tiny3d_android_arch_triple(_arch_triple)
    _tiny3d_append_existing_dir(_includes
        "${CMAKE_SYSROOT}/usr/include/c++/v1"
        "${CMAKE_SYSROOT}/usr/include/${_arch_triple}"
        "${CMAKE_SYSROOT}/usr/include")

    # 隐式搜索路径里属于 sysroot 的补上。编译器内建头（stddef.h 等）不要从 NDK
    # clang 的 resource dir 拿 —— host 上的 libclang 版本未必对得上，rpp 会自己
    # 用 -resource-dir 指到随包的那份。
    if (CMAKE_SYSROOT)
        file(TO_CMAKE_PATH "${CMAKE_SYSROOT}" _sys)
        foreach (_inc IN LISTS CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
            file(TO_CMAKE_PATH "${_inc}" _inc)
            string(FIND "${_inc}" "${_sys}" _pos)
            if (_pos EQUAL 0)
                _tiny3d_append_existing_dir(_includes "${_inc}")
            endif ()
        endforeach ()
    endif ()

    set(${OUT_INCLUDES} "${_includes}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# tiny3d_detect_system_includes()
#
# 设置两个全局缓存变量：
#   TINY3D_SYSTEM_INCLUDE_DIRS   -> ReflectionSettings.json 的 SystemIncludePath
#   TINY3D_REFLECT_TARGET_FLAGS  -> ReflectionSettings.json 的 OtherFlags 追加项
#
# 探测有磁盘 IO，结果进 cache 只做一次；工具链换了要重新 configure，与 CMake 其他
# 编译器信息的失效方式保持一致。
#-------------------------------------------------------------------------------
function(tiny3d_detect_system_includes)
    if (DEFINED TINY3D_SYSTEM_INCLUDE_DIRS_CACHED)
        return()
    endif ()

    set(_includes "")
    set(_flags "")

    if (CMAKE_SYSTEM_NAME STREQUAL "Android")
        # host 上的 libclang 默认三元组是 Windows。不显式给 --target，预定义宏会变成
        # _WIN32 / _MSC_VER，条件编译按 Windows 求值，Android 反射产物就全错了。
        _tiny3d_detect_android_includes(_includes)
        if (CMAKE_CXX_COMPILER_TARGET)
            list(APPEND _flags "--target=${CMAKE_CXX_COMPILER_TARGET}")
        endif ()
        if (CMAKE_SYSROOT)
            file(TO_CMAKE_PATH "${CMAKE_SYSROOT}" _sysroot)
            list(APPEND _flags "--sysroot=${_sysroot}")
        endif ()
        list(APPEND _flags "-stdlib=libc++")

        # 编译器内建头（stddef.h、各架构 intrinsic）不从这里给。解析用的是 host
        # libclang，内建头必须跟它同版本，而不是跟 NDK clang 同版本 —— 借 NDK 那份
        # 的话，x86_64 目标下 immintrin.h 会展开成当前 libclang 已经删掉的
        # __builtin_ia32_* 旧名字。这份由 rpp 用自己旁边的 clang-resource 提供。
    elseif (APPLE)
        if (CMAKE_OSX_SYSROOT)
            file(TO_CMAKE_PATH "${CMAKE_OSX_SYSROOT}" _sysroot)
            list(APPEND _flags "-isysroot" "${_sysroot}")
        endif ()
        foreach (_arch IN LISTS CMAKE_OSX_ARCHITECTURES)
            list(APPEND _flags "-arch" "${_arch}")
        endforeach ()
        list(APPEND _flags "-stdlib=libc++")
    elseif (MSVC)
        _tiny3d_detect_msvc_includes(_includes)
    else ()
        # GCC / Clang 下 CMake 会把编译器的隐式搜索路径填好，直接用
        _tiny3d_append_existing_dir(_includes ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    endif ()

    # 业务工程 configure 时未必在 vcvarsall 起的环境里，而它也拿不到引擎的
    # CMAKE_CXX_COMPILER 布局，两级探测都可能落空。这时用 SDK 固化过来的那份。
    if (NOT _includes AND TINY3D_SDK_SYSTEM_INCLUDE_DIRS)
        _tiny3d_append_existing_dir(_includes ${TINY3D_SDK_SYSTEM_INCLUDE_DIRS})
        if (NOT _flags)
            set(_flags ${TINY3D_SDK_REFLECT_TARGET_FLAGS})
        endif ()
        message(STATUS "Reflection system includes: 用 SDK 固化的探测结果")
    endif ()

    if (MSVC AND NOT _includes)
        # 缺了系统头，每个翻译单元都会 file not found，继续下去只会得到一份空的
        # 反射代码 —— 构建仍然通过，直到运行期反射对不上才发现。那种失败比在这里
        # 直接报错难查得多。
        message(FATAL_ERROR
            "tiny3d_detect_system_includes: 未能确定 MSVC / Windows SDK 头文件路径。\n"
            "  %INCLUDE% 为空，且从 ${CMAKE_CXX_COMPILER} 反推失败。\n"
            "  从开发者命令提示符运行，或先执行 source/Projects/setup-msvc-env.bat。")
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Android")
        if (NOT CMAKE_CXX_COMPILER_TARGET)
            message(FATAL_ERROR
                "tiny3d_detect_system_includes: Android 交叉编译缺少 CMAKE_CXX_COMPILER_TARGET。\n"
                "  host 上的 libclang 会按 Windows 三元组解析，条件编译整棵树都会走错。")
        endif ()
        if (NOT _includes)
            message(FATAL_ERROR
                "tiny3d_detect_system_includes: 未能确定 NDK 头文件路径。\n"
                "  CMAKE_SYSROOT = ${CMAKE_SYSROOT}\n"
                "  CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES 为空，按 NDK 布局手拼也没找到目录。")
        endif ()
    endif ()

    list(LENGTH _includes _count)
    message(STATUS "Reflection system includes: ${_count} dir(s), flags: ${_flags}")

    set(TINY3D_SYSTEM_INCLUDE_DIRS "${_includes}"
        CACHE INTERNAL "System include directories for reflection parsing")
    set(TINY3D_REFLECT_TARGET_FLAGS "${_flags}"
        CACHE INTERNAL "Extra clang flags for reflection parsing")
    set(TINY3D_SYSTEM_INCLUDE_DIRS_CACHED TRUE
        CACHE INTERNAL "Guard so the detection only runs once")
endfunction()
