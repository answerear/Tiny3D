#-------------------------------------------------------------------------------
# 生成 Tiny3DSDK.cmake，随编辑器一起安装到 bin 目录。
#
# 业务 Game Plugin 工程需要用和编辑器完全相同的工具链来编译，否则加载后会崩。
# 与其在编辑器代码里硬编码「Visual Studio 16 2019」这类信息，不如在引擎自己
# 构建的时候把实际生效的值记下来——这样从 VS2019 升到 VS2022、或换到 Linux /
# macOS 构建，业务侧和编辑器侧代码都不用动。
#-------------------------------------------------------------------------------

# 函数体里的 CMAKE_CURRENT_LIST_DIR 指向调用方的 listfile，所以在这里先记下本文件目录
set(T3DSDK_TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(tiny3d_generate_sdk_config)
    #---------------------------------------------------------------------------
    # 工具链
    #---------------------------------------------------------------------------
    if (MSVC)
        # CMAKE_GENERATOR_PLATFORM / CMAKE_GENERATOR_TOOLSET 只反映命令行显式传了
        # 什么。generate-vs2019-x64-debug.bat 没写 -A 也没写 -T（VS2019 在 64 位
        # 主机上默认就是 x64 + v142），此时这两个变量都是空的。CMAKE_VS_PLATFORM_*
        # 才是实际生效值。
        set(T3DSDK_GENERATOR_PLATFORM "${CMAKE_VS_PLATFORM_NAME}")
        set(T3DSDK_GENERATOR_TOOLSET  "${CMAKE_VS_PLATFORM_TOOLSET}")
        # 编译器由生成器决定，显式指定反而会干扰 VS 生成器
        set(T3DSDK_CXX_COMPILER       "")
    else ()
        set(T3DSDK_GENERATOR_PLATFORM "${CMAKE_GENERATOR_PLATFORM}")
        set(T3DSDK_GENERATOR_TOOLSET  "${CMAKE_GENERATOR_TOOLSET}")
        # 类 Unix 下编译器通常由 CC / CXX 环境变量指定（见 generate-codeblock-linux.sh
        # 的 CC=clang CXX=clang++），业务工程 configure 时不会继承这个环境。漏了这项，
        # 业务 DLL 可能被 g++ 编成链 libstdc++，而引擎是 clang++ + libc++，ABI 不兼容。
        set(T3DSDK_CXX_COMPILER       "${CMAKE_CXX_COMPILER}")
    endif ()

    # CMAKE_GENERATOR 已经是剥掉 IDE 前缀的底层生成器：脚本里传的
    # "CodeBlocks - Unix Makefiles" 会被拆成 CMAKE_GENERATOR=Unix Makefiles
    # 加 CMAKE_EXTRA_GENERATOR=CodeBlocks。用前者，业务工程就不会被生成一堆
    # 用不上的 CodeBlocks 工程文件。
    set(T3DSDK_GENERATOR "${CMAKE_GENERATOR}")

    #---------------------------------------------------------------------------
    # 编译期一致性
    #---------------------------------------------------------------------------
    set(T3DSDK_CXX_STANDARD "${CMAKE_CXX_STANDARD}")

    if (CMAKE_MSVC_RUNTIME_LIBRARY)
        set(T3DSDK_MSVC_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    elseif (MSVC)
        # 引擎没显式设置，用的就是 CMake 的默认值，这里写成等价形式传给业务工程
        set(T3DSDK_MSVC_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else ()
        set(T3DSDK_MSVC_RUNTIME "")
    endif ()

    # 顶层 add_definitions 的结果。里面有 _HAS_EXCEPTIONS=0 这种会改变 STL 布局的
    # 定义，业务工程必须原样带上，否则跨 DLL 传 String / 容器时行为未定义。
    # 直接取目录属性而不是手工维护列表，引擎以后加减定义会自动跟上。
    get_directory_property(_defs DIRECTORY "${CMAKE_SOURCE_DIR}" COMPILE_DEFINITIONS)
    get_directory_property(_opts DIRECTORY "${CMAKE_SOURCE_DIR}" COMPILE_OPTIONS)

    set(T3DSDK_COMPILE_DEFINITIONS "${_defs}")
    set(T3DSDK_COMPILE_OPTIONS "${_opts}")

    #---------------------------------------------------------------------------
    # SDK 布局
    #
    # 开发期没有独立的 install 目录，头文件仍在源码树、库在 lib/ 下，所以这里写
    # 绝对路径。日后做真正的安装包时，改成相对 SDK 根目录解析即可，业务工程侧
    # 因为只调 tiny3d_sdk_setup_plugin() 而不受影响。
    #---------------------------------------------------------------------------
    set(_src "${CMAKE_SOURCE_DIR}")

    set(T3DSDK_INCLUDE_DIRS
        "${_src}/Platform/Include"
        "${_src}/System/Include"
        "${_src}/Log/Include"
        "${_src}/Utils/Include"
        "${_src}/Math/Include"
        "${_src}/Framework/Include"
        "${_src}/Core/Include"
        ${RTTR_INCLUDE_DIR})

    # 游戏 sln 里的 TinyPlayer 工程编译这份源码，避免每个工程各抄一份宿主
    set(T3DSDK_PLAYER_DIR "${_src}/Player")

    set(T3DSDK_LIB_DIR_DEBUG   "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}")
    set(T3DSDK_LIB_DIR_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}")
    set(T3DSDK_BIN_DIR_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
    set(T3DSDK_BIN_DIR_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")

    # rttr 是唯一带配置后缀的库（BUILD_RTTR_DYNAMIC 下 Debug 产出 rttr_core_d）
    set(T3DSDK_RTTR_LIB_DEBUG   "rttr_core_d")
    set(T3DSDK_RTTR_LIB_RELEASE "rttr_core")

    # 引擎各目标是链 rttr_core 这个 target 拿到 RTTR_DLL 的，业务插件链的是库文件
    # 路径，得把这份 INTERFACE 定义显式带过去，否则 rttr::policy::ctor::* 之类的
    # 导出常量会解析成本地符号而链接失败
    if (TARGET rttr_core)
        get_target_property(_rttr_defs rttr_core INTERFACE_COMPILE_DEFINITIONS)
        if (_rttr_defs)
            set(T3DSDK_RTTR_DEFINITIONS "${_rttr_defs}")
        endif ()
    endif ()

    # 反斜杠会被 CMake 当转义符，统一成正斜杠
    foreach (_var
        T3DSDK_CXX_COMPILER T3DSDK_INCLUDE_DIRS
        T3DSDK_LIB_DIR_DEBUG T3DSDK_LIB_DIR_RELEASE
        T3DSDK_BIN_DIR_DEBUG T3DSDK_BIN_DIR_RELEASE
        T3DSDK_PLAYER_DIR)
        string(REPLACE "\\" "/" ${_var} "${${_var}}")
    endforeach ()

    set(_template "${T3DSDK_TEMPLATE_DIR}/Tiny3DSDK.cmake.in")
    set(_helper "${T3DSDK_TEMPLATE_DIR}/Tiny3DReflectHelpers.cmake")
    set(_reflect_base_src "${CMAKE_SOURCE_DIR}/nmake/Core/Runtime/ReflectionSettings.json")

    # 每个配置的 bin 目录都放一份。编辑器用 Dir::getAppPath() 找它，而 appPath
    # 就是当前配置的 bin 目录，两个配置各自独立。
    foreach (_dir "${T3DSDK_BIN_DIR_DEBUG}" "${T3DSDK_BIN_DIR_RELEASE}")
        if (_dir)
            if (WIN32)
                set(T3DSDK_RPP "${_dir}/rpp.exe")
            elseif (APPLE)
                set(T3DSDK_RPP "${_dir}/rpp.app/Contents/MacOS/rpp")
            else ()
                set(T3DSDK_RPP "${_dir}/rpp")
            endif ()
            set(T3DSDK_REFLECTION_BASE "${_dir}/ReflectionSettings.base.json")
            string(REPLACE "\\" "/" T3DSDK_RPP "${T3DSDK_RPP}")
            string(REPLACE "\\" "/" T3DSDK_REFLECTION_BASE "${T3DSDK_REFLECTION_BASE}")

            configure_file("${_template}" "${_dir}/Tiny3DSDK.cmake" @ONLY)
            configure_file("${_helper}" "${_dir}/Tiny3DReflectHelpers.cmake" COPYONLY)

            if (EXISTS "${_reflect_base_src}")
                configure_file("${_reflect_base_src}" "${T3DSDK_REFLECTION_BASE}" COPYONLY)
            else ()
                message(WARNING
                    "nmake/Core/Runtime/ReflectionSettings.json not found. "
                    "Game Plugin TCLASS reflection needs it; run the engine generate script first.")
            endif ()
        endif ()
    endforeach ()

    message(STATUS "Generated Tiny3DSDK.cmake : generator = ${T3DSDK_GENERATOR}, "
                   "platform = ${T3DSDK_GENERATOR_PLATFORM}, toolset = ${T3DSDK_GENERATOR_TOOLSET}")
endfunction()
