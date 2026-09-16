#-------------------------------------------------------------------------------
# Tiny3D 业务工程反射生成
#
# 复用引擎 nmake/Core/Runtime 产出的 ReflectionSettings.json（含 libclang 所需的
# 系统头与宏），追加业务 include 后调用 rpp，把 *.generated.cpp 编进目标。
#
# rpp 会把 GeneratedPath 拼到 SOURCE_DIR 后面，因此 JSON 里必须写相对 SOURCE_DIR
# 的路径，不能写绝对路径。
#-------------------------------------------------------------------------------

# GamePlugin 这类业务工程不经过 source/CMakeLists.txt，那边的 option() 声明不到这里，
# 给个默认值。引擎树里这个变量已经由 option() 定下来了，不会被覆盖。
#
# 这段刻意放在 include_guard 之前：业务工程的 Editor / Runtime / Player 是三个平级
# 子目录，各自 include 一次本文件，而 GLOBAL 的 guard 只让第一个真正执行。普通变量
# 不会从一个子目录流到它的兄弟目录，写在 guard 之后的话，后两个目录里这个变量是空的，
# 于是会退回旧链路去找已经不再导出的 ReflectionSettings.base.json。
if (NOT DEFINED TINY3D_REFLECT_CMAKE_NATIVE)
    set(TINY3D_REFLECT_CMAKE_NATIVE ON)
endif ()

include_guard(GLOBAL)

# 用自身所在目录定位：这个文件会被 GenerateTiny3DSDK.cmake 复制进 SDK 给业务工程用，
# 那边没有引擎的 CMAKE_MODULE_PATH，按模块名 include 会找不到。
include("${CMAKE_CURRENT_LIST_DIR}/Tiny3DDetectSystemIncludes.cmake")

#-------------------------------------------------------------------------------
# _tiny3d_reflect_json_escape(<out> <path>)
#-------------------------------------------------------------------------------
function(_tiny3d_reflect_json_escape OUT_VAR PATH)
    string(REPLACE "\\" "/" _path "${PATH}")
    string(REPLACE "\"" "\\\"" _path "${_path}")
    set(${OUT_VAR} "${_path}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_write_reflection_settings()
#
# 读 BASE_SETTINGS，把 EXTRA_INCLUDES 插到 IncludePath 数组头部，把 GeneratedPath
# 改成相对 SOURCE_DIR 的 GENERATED_DIR。
#-------------------------------------------------------------------------------
function(_tiny3d_write_reflection_settings)
    set(oneValueArgs BASE OUTPUT SOURCE_DIR GENERATED_DIR)
    set(multiValueArgs EXTRA_INCLUDES)
    cmake_parse_arguments(T3DRS "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT EXISTS "${T3DRS_BASE}")
        message(FATAL_ERROR
            "tiny3d_enable_reflection: BASE_SETTINGS not found:\n  ${T3DRS_BASE}\n"
            "Run the engine generate script first so nmake/Core/Runtime/ReflectionSettings.json exists.")
    endif ()

    file(READ "${T3DRS_BASE}" _json)

    file(RELATIVE_PATH _gen_rel "${T3DRS_SOURCE_DIR}" "${T3DRS_GENERATED_DIR}")
    if (IS_ABSOLUTE "${_gen_rel}")
        message(FATAL_ERROR
            "tiny3d_enable_reflection: GENERATED_DIR must share a drive/root with SOURCE_DIR "
            "because rpp concatenates SOURCE_DIR + GeneratedPath.\n"
            "  SOURCE_DIR    = ${T3DRS_SOURCE_DIR}\n"
            "  GENERATED_DIR = ${T3DRS_GENERATED_DIR}")
    endif ()
    _tiny3d_reflect_json_escape(_gen_json "${_gen_rel}")

    string(REGEX REPLACE
        "\"GeneratedPath\"[ \t]*:[ \t]*\"[^\"]*\""
        "\"GeneratedPath\": \"${_gen_json}\""
        _json "${_json}")

    set(_extra_json "")
    foreach (_inc IN LISTS T3DRS_EXTRA_INCLUDES)
        _tiny3d_reflect_json_escape(_inc_json "${_inc}")
        string(APPEND _extra_json "        \"${_inc_json}\",\n")
    endforeach ()

    if (_extra_json)
        string(REGEX REPLACE
            "(\"IncludePath\"[ \t]*:[ \t]*\\[)[ \t]*(\r?\n)"
            "\\1\\2${_extra_json}"
            _json "${_json}")
        list(GET T3DRS_EXTRA_INCLUDES 0 _first_inc)
        _tiny3d_reflect_json_escape(_first_json "${_first_inc}")
        string(FIND "${_json}" "${_first_json}" _inserted)
        if (_inserted EQUAL -1)
            message(WARNING
                "tiny3d_enable_reflection: failed to insert EXTRA_INCLUDES into ${T3DRS_OUTPUT}")
        endif ()
    endif ()

    get_filename_component(_out_dir "${T3DRS_OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_out_dir}")
    file(WRITE "${T3DRS_OUTPUT}" "${_json}")
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_find_rpp(<out_configure> <out_build>)
#
# configure 期不能用生成器表达式，所以要同时给出一个磁盘路径（若已编过 rpp）
# 和一个构建期路径。
#-------------------------------------------------------------------------------
function(_tiny3d_find_rpp OUT_CONFIGURE OUT_BUILD)
    set(_cfg "")
    set(_bld "")

    if (DEFINED TINY3D_SDK_RPP AND NOT "${TINY3D_SDK_RPP}" STREQUAL "")
        set(_cfg "${TINY3D_SDK_RPP}")
        set(_bld "${TINY3D_SDK_RPP}")
    elseif (CMAKE_CROSSCOMPILING AND TINY3D_HOST_RPP)
        set(_cfg "${TINY3D_HOST_RPP}")
        set(_bld "${TINY3D_HOST_RPP}")
    else ()
        if (TARGET ReflectionPreprocessor)
            set(_bld "$<TARGET_FILE:ReflectionPreprocessor>")
        endif ()

        set(_candidates "")
        if (WIN32)
            list(APPEND _candidates
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp.exe"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/rpp.exe"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp.exe")
        elseif (APPLE)
            list(APPEND _candidates
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp.app/Contents/MacOS/rpp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/rpp.app/Contents/MacOS/rpp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp.app/Contents/MacOS/rpp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp")
        else ()
            list(APPEND _candidates
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/rpp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/rpp"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rpp")
        endif ()

        foreach (_c IN LISTS _candidates)
            if (_c AND EXISTS "${_c}")
                set(_cfg "${_c}")
                break()
            endif ()
        endforeach ()

        # TINY3D_BUILD_TOOLS=OFF 时没有 ReflectionPreprocessor 目标，但 bin 目录里
        # 往常还留着一个能用的 rpp。构建期用它，比直接判定「找不到 rpp」有用。
        if (NOT _bld)
            set(_bld "${_cfg}")
        endif ()
    endif ()

    set(${OUT_CONFIGURE} "${_cfg}" PARENT_SCOPE)
    set(${OUT_BUILD} "${_bld}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# tiny3d_enable_reflection(<target>
#   SOURCE_DIR <dir>
#   GENERATED_DIR <dir>
#   SETTINGS_DIR <dir>
#   BASE_SETTINGS <ReflectionSettings.json>
#   EXTRA_INCLUDES <dir>...
# )
#-------------------------------------------------------------------------------
function(tiny3d_enable_reflection TARGET)
    set(oneValueArgs SOURCE_DIR GENERATED_DIR SETTINGS_DIR BASE_SETTINGS)
    set(multiValueArgs EXTRA_INCLUDES)
    cmake_parse_arguments(T3DR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET "${TARGET}")
        message(FATAL_ERROR "tiny3d_enable_reflection: target '${TARGET}' does not exist")
    endif ()
    if (NOT T3DR_SOURCE_DIR)
        message(FATAL_ERROR "tiny3d_enable_reflection: SOURCE_DIR is required")
    endif ()
    if (NOT T3DR_GENERATED_DIR)
        message(FATAL_ERROR "tiny3d_enable_reflection: GENERATED_DIR is required")
    endif ()
    if (NOT T3DR_SETTINGS_DIR)
        set(T3DR_SETTINGS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Reflect")
    endif ()

    if (TINY3D_REFLECT_CMAKE_NATIVE)
        _tiny3d_enable_reflection_native(${TARGET}
            SOURCE_DIR     "${T3DR_SOURCE_DIR}"
            GENERATED_DIR  "${T3DR_GENERATED_DIR}"
            SETTINGS_DIR   "${T3DR_SETTINGS_DIR}"
            EXTRA_INCLUDES ${T3DR_EXTRA_INCLUDES})
        return()
    endif ()

    if (NOT T3DR_BASE_SETTINGS)
        if (DEFINED TINY3D_SDK_REFLECTION_BASE AND NOT "${TINY3D_SDK_REFLECTION_BASE}" STREQUAL "")
            set(T3DR_BASE_SETTINGS "${TINY3D_SDK_REFLECTION_BASE}")
        else ()
            set(T3DR_BASE_SETTINGS "${CMAKE_SOURCE_DIR}/nmake/Core/Runtime/ReflectionSettings.json")
        endif ()
    endif ()

    get_filename_component(T3DR_SOURCE_DIR "${T3DR_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(T3DR_GENERATED_DIR "${T3DR_GENERATED_DIR}" ABSOLUTE)
    get_filename_component(T3DR_SETTINGS_DIR "${T3DR_SETTINGS_DIR}" ABSOLUTE)

    file(MAKE_DIRECTORY "${T3DR_GENERATED_DIR}")
    file(MAKE_DIRECTORY "${T3DR_SETTINGS_DIR}")

    set(_run_rpp TRUE)
    if (CMAKE_CROSSCOMPILING AND NOT TINY3D_HOST_RPP AND NOT TINY3D_SDK_RPP)
        set(_run_rpp FALSE)
    endif ()
    if (DEFINED TINY3D_INCREMENTAL_RTTR AND NOT TINY3D_INCREMENTAL_RTTR)
        set(_run_rpp FALSE)
    endif ()
    # generate-*.bat 的 nmake 阶段只为 System/Math/Core/Editor 导出 compile_commands.json，
    # 此时 cct 还没写出 ReflectionSettings.json，Samples 不能在这里跑 rpp。
    if (TINY3D_SYSTEM_RTTR OR TINY3D_MATH_RTTR OR TINY3D_CORE_RTTR
            OR TINY3D_CORE_EDITOR_RTTR OR TINY3D_LAUNCHER_RTTR OR TINY3D_EDITOR_RTTR)
        set(_run_rpp FALSE)
        message(STATUS
            "tiny3d_enable_reflection: skip rpp for '${TARGET}' (compile_commands export)")
    endif ()

    _tiny3d_find_rpp(_rpp_cfg _rpp_bld)

    if (_run_rpp)
        if (NOT T3DR_BASE_SETTINGS OR NOT EXISTS "${T3DR_BASE_SETTINGS}")
            # GamePlugin 必须有 SDK 导出的底板；引擎树则可能处于 nmake 尚未跑完 cct 的阶段
            if (DEFINED TINY3D_SDK_RPP AND NOT "${TINY3D_SDK_RPP}" STREQUAL "")
                message(FATAL_ERROR
                    "tiny3d_enable_reflection: BASE_SETTINGS not found:\n  ${T3DR_BASE_SETTINGS}\n"
                    "Run the engine generate script first so nmake/Core/Runtime/ReflectionSettings.json exists, "
                    "then reconfigure the engine to export ReflectionSettings.base.json.")
            endif ()
            message(STATUS
                "tiny3d_enable_reflection: skip rpp for '${TARGET}', "
                "BASE_SETTINGS not found: ${T3DR_BASE_SETTINGS}")
            set(_run_rpp FALSE)
        endif ()
    endif ()

    # 反射产物是按 GENERATED_DIR 组织的（含 rpp 的 .deps / .tpl 增量缓存），同一个
    # GENERATED_DIR 下只该有一次 rpp。业务工程的 Editor 与 Runtime 两个变体编的是同一份
    # 源码、共享同一个 GENERATED_DIR，用 target 名派生 stamp / reflect target 会变成两份
    # 活儿并发往同一目录写，所以这里按 GENERATED_DIR 派生。
    # 构建目录内的相对路径够短也够唯一，只有 GENERATED_DIR 在构建目录之外（交叉编译
    # 复用预生成产物时）才退回全路径
    file(RELATIVE_PATH _reflect_rel "${CMAKE_BINARY_DIR}" "${T3DR_GENERATED_DIR}")
    if (_reflect_rel MATCHES "^\\.\\." OR IS_ABSOLUTE "${_reflect_rel}")
        set(_reflect_rel "${T3DR_GENERATED_DIR}")
    endif ()
    string(MAKE_C_IDENTIFIER "${_reflect_rel}" _reflect_id)
    set(_reflect_target "reflect_${_reflect_id}")
    set(_reflect_stamp "${T3DR_GENERATED_DIR}/.rpp.stamp")

    # 共享 GENERATED_DIR 的第二个 target 不必再跑一次 configure 期的 rpp：产物、.deps 缓存
    # 和反射配置都是同一份，第一个 target 已经把它们弄成最新的了
    if (_run_rpp AND NOT TARGET ${_reflect_target})

        _tiny3d_write_reflection_settings(
            BASE "${T3DR_BASE_SETTINGS}"
            OUTPUT "${T3DR_SETTINGS_DIR}/ReflectionSettings.json"
            SOURCE_DIR "${T3DR_SOURCE_DIR}"
            GENERATED_DIR "${T3DR_GENERATED_DIR}"
            EXTRA_INCLUDES ${T3DR_EXTRA_INCLUDES})

        if (_rpp_cfg AND EXISTS "${_rpp_cfg}")
            execute_process(
                COMMAND "${_rpp_cfg}" "${T3DR_SETTINGS_DIR}" "${T3DR_SOURCE_DIR}" -b -j 8
                RESULT_VARIABLE _rpp_ret
                OUTPUT_VARIABLE _rpp_out
                ERROR_VARIABLE _rpp_err)
            if (NOT _rpp_ret EQUAL 0)
                message(FATAL_ERROR
                    "tiny3d_enable_reflection: rpp failed for '${TARGET}' (exit ${_rpp_ret})\n"
                    "${_rpp_out}${_rpp_err}")
            endif ()
            if (_rpp_out)
                message(STATUS "rpp [${TARGET}]: ${_rpp_out}")
            endif ()
            # 刚在 configure 期跑过一次，把 stamp 顶到最新，紧接着的 build 就不必再跑。
            # 用 file(WRITE) 而不是 file(TOUCH) 是为了不依赖较新的 CMake。
            file(WRITE "${_reflect_stamp}" "")
        else ()
            message(STATUS
                "tiny3d_enable_reflection: rpp not found at configure time for '${TARGET}'; "
                "will generate at build time")
        endif ()
    endif ()

    # 只编本工程源文件对应的产物（RotateBehaviour.cpp → RotateBehaviour.generated.cpp）。
    # rpp 扫到引擎头里的 STL 实例会另写出 list/unordered_map.generated.cpp，那些已在
    # T3DCore 里注册过，应用层再编会 include 路径不对，还会重复 RTTR 注册。
    file(GLOB_RECURSE _src_cpp
        "${T3DR_SOURCE_DIR}/*.cpp"
        "${T3DR_SOURCE_DIR}/*.cxx")
    set(_generated "")
    foreach (_src IN LISTS _src_cpp)
        get_filename_component(_fname "${_src}" NAME)
        if (NOT _fname MATCHES "\\.generated\\.cpp$")
            get_filename_component(_title "${_src}" NAME_WE)
            set(_gen "${T3DR_GENERATED_DIR}/${_title}.generated.cpp")
            if (EXISTS "${_gen}")
                list(APPEND _generated "${_gen}")
            endif ()
        endif ()
    endforeach ()
    if (_generated)
        list(REMOVE_DUPLICATES _generated)
    endif ()
    list(LENGTH _generated _ngen)
    message(STATUS "tiny3d_enable_reflection: ${TARGET} -> ${_ngen} generated source(s) in ${T3DR_GENERATED_DIR}")
    if (_generated)
        target_sources(${TARGET} PRIVATE ${_generated})
        source_group("Generated" FILES ${_generated})
    endif ()

    if (_run_rpp)
        if (NOT _rpp_bld)
            message(FATAL_ERROR
                "tiny3d_enable_reflection: cannot locate rpp for '${TARGET}'. "
                "Build ReflectionPreprocessor, or set TINY3D_SDK_RPP / TINY3D_HOST_RPP.")
        endif ()

        if (NOT TARGET ${_reflect_target})
            # 裸 add_custom_target 没有输出文件，CMake 一律当作 always-out-of-date，于是
            # 每次 build 都要把 rpp 启一遍：扫全部头文件找反射宏、校验 .deps/.tpl、算依赖
            # hash，一行代码没改也得几秒。挂个 stamp 输出并声明输入之后，源码没动就完全
            # 不启动 rpp，MSBuild / ninja 自己就把这一步跳掉了。
            file(GLOB_RECURSE _reflect_inputs
                "${T3DR_SOURCE_DIR}/*.h"
                "${T3DR_SOURCE_DIR}/*.hpp"
                "${T3DR_SOURCE_DIR}/*.hh"
                "${T3DR_SOURCE_DIR}/*.cpp"
                "${T3DR_SOURCE_DIR}/*.cxx")
            list(FILTER _reflect_inputs EXCLUDE REGEX "\\.generated\\.cpp$")

            # rpp 的真实依赖还含引擎头（它自己记在 .deps 里），CMake 侧只能近似到业务源码
            # 加上反射配置底板。底板随引擎重建刷新，引擎升级这条路也就覆盖到了。
            set(_reflect_deps ${_reflect_inputs}
                "${T3DR_SETTINGS_DIR}/ReflectionSettings.json")
            if (T3DR_BASE_SETTINGS AND EXISTS "${T3DR_BASE_SETTINGS}")
                list(APPEND _reflect_deps "${T3DR_BASE_SETTINGS}")
            endif ()

            add_custom_command(
                OUTPUT "${_reflect_stamp}"
                COMMAND ${_rpp_bld} "${T3DR_SETTINGS_DIR}" "${T3DR_SOURCE_DIR}" -b -j 8
                COMMAND "${CMAKE_COMMAND}" -E touch "${_reflect_stamp}"
                DEPENDS ${_reflect_deps}
                COMMENT "Running reflection generation: ${T3DR_SOURCE_DIR}"
                VERBATIM)

            add_custom_target(${_reflect_target} DEPENDS "${_reflect_stamp}")

            if (TARGET ReflectionPreprocessor)
                add_dependencies(${_reflect_target} ReflectionPreprocessor)
            endif ()
            set_property(TARGET ${_reflect_target} PROPERTY FOLDER "Reflect")
        endif ()
        add_dependencies(${TARGET} ${_reflect_target})
    endif ()
endfunction()

#===============================================================================
# 以下是「反射配置由 CMake 原生生成」的新链路。
# 阶段 1 与上面的 cct / nmake 链路并存，只额外写出一份配置供比对，不接进构建。
# 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §6
#===============================================================================

#-------------------------------------------------------------------------------
# _tiny3d_json_array_literal(<out> <item>...)
#
# 把 configure 期就已知的字符串列表排成 JSON 数组项（含缩进与尾换行）。
#-------------------------------------------------------------------------------
function(_tiny3d_json_array_literal OUT_VAR)
    set(_items "")
    foreach (_item IN LISTS ARGN)
        if (NOT "${_item}" STREQUAL "")
            _tiny3d_reflect_json_escape(_escaped "${_item}")
            string(APPEND _items "        \"${_escaped}\",\n")
        endif ()
    endforeach ()
    # 去掉最后一项的逗号，JSON 不接受尾逗号
    string(REGEX REPLACE ",\n$" "\n" _items "${_items}")
    set(${OUT_VAR} "${_items}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_json_array_genex(<out> <list-genex>)
#
# 同上，但列表要到 generate 期才求值。空列表必须整段消失，否则会写出 [""]，
# 于是外面套一层 $<BOOL:>；分隔符里的逗号得写成 $<COMMA>，不然会被当成生成器
# 表达式自己的参数分隔符。
#-------------------------------------------------------------------------------
function(_tiny3d_json_array_genex OUT_VAR LIST_GENEX)
    # 空元素（列表拼接留下的）会变成 "" 混进数组，先滤掉
    set(_clean "$<FILTER:$<REMOVE_DUPLICATES:${LIST_GENEX}>,EXCLUDE,^$>")
    set(${OUT_VAR}
        "$<$<BOOL:${_clean}>:        \"$<JOIN:${_clean},\"$<COMMA>\n        \">\"\n>"
        PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------------
# tiny3d_write_reflection_settings(<target>
#   SETTINGS_DIR   <dir>     # 写出目录，rpp 收的是目录，文件名固定
#   SOURCE_DIR     <dir>     # rpp 的扫描根，GeneratedPath 相对它
#   GENERATED_DIR  <dir>     # *.generated.cpp 落脚处
#   EXTRA_INCLUDES <dir>...  # 追加到 IncludePath 最前面
# )
#
# 用 file(GENERATE) 写出，因为 include 目录与宏定义只有到 generate 期才完整
# （生成器表达式、INTERFACE 传递都得等到那时）。输出路径带 $<CONFIG>：多配置
# 生成器下 Debug / Release 的宏本就不同，各出一份才是对的。
#-------------------------------------------------------------------------------
function(tiny3d_write_reflection_settings TARGET)
    set(oneValueArgs SETTINGS_DIR SOURCE_DIR GENERATED_DIR OUT_SETTINGS_FILES)
    set(multiValueArgs EXTRA_INCLUDES)
    cmake_parse_arguments(T3DW "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET "${TARGET}")
        message(FATAL_ERROR "tiny3d_write_reflection_settings: target '${TARGET}' 不存在")
    endif ()
    foreach (_required SETTINGS_DIR SOURCE_DIR GENERATED_DIR)
        if (NOT T3DW_${_required})
            message(FATAL_ERROR "tiny3d_write_reflection_settings: 缺少参数 ${_required}")
        endif ()
    endforeach ()

    tiny3d_detect_system_includes()

    get_filename_component(_source_dir "${T3DW_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(_generated_dir "${T3DW_GENERATED_DIR}" ABSOLUTE)

    # rpp 把 GeneratedPath 拼在 SOURCE_DIR 后面，所以这里必须是相对路径
    file(RELATIVE_PATH _gen_rel "${_source_dir}" "${_generated_dir}")
    if (IS_ABSOLUTE "${_gen_rel}")
        message(FATAL_ERROR
            "tiny3d_write_reflection_settings: GENERATED_DIR 与 SOURCE_DIR 必须在同一个盘符下，"
            "因为 rpp 会做 SOURCE_DIR + GeneratedPath 的拼接。\n"
            "  SOURCE_DIR    = ${_source_dir}\n"
            "  GENERATED_DIR = ${_generated_dir}")
    endif ()
    _tiny3d_reflect_json_escape(_gen_json "${_gen_rel}")

    # ---- IncludePath：业务追加项在前，target 自身的在后 --------------------
    set(_extra_normalized "")
    foreach (_inc IN LISTS T3DW_EXTRA_INCLUDES)
        file(TO_CMAKE_PATH "${_inc}" _inc)
        list(APPEND _extra_normalized "${_inc}")
    endforeach ()
    _tiny3d_json_array_genex(_inc_items
        "${_extra_normalized};$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>")

    # ---- MacroDefinition ---------------------------------------------------
    # add_definitions() 写的是目录级属性。CMake 只在 add_library/add_executable
    # 的那一刻把当时的目录属性拷进 target，之后再 add_definitions 就进不去了，
    # 所以两处都取，靠 REMOVE_DUPLICATES 收口。
    get_directory_property(_dir_defs COMPILE_DEFINITIONS)

    # 共享库编译自身源文件时，CMake 会额外定义 <target>_EXPORTS（或 DEFINE_SYMBOL）。
    # 它不在 COMPILE_DEFINITIONS 属性里，得自己补——少了它，导出宏会被解析成
    # dllimport，类声明的语义就和实际编译时不是一回事了。
    set(_export_define "")
    get_target_property(_target_type ${TARGET} TYPE)
    if (_target_type STREQUAL "SHARED_LIBRARY" OR _target_type STREQUAL "MODULE_LIBRARY")
        get_target_property(_define_symbol ${TARGET} DEFINE_SYMBOL)
        if (_define_symbol)
            set(_export_define "${_define_symbol}")
        else ()
            set(_export_define "${TARGET}_EXPORTS")
        endif ()
    endif ()

    _tiny3d_json_array_genex(_def_items
        "${_dir_defs};${_export_define};$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>")

    # ---- SystemIncludePath / OtherFlags：configure 期即可确定 --------------
    _tiny3d_json_array_literal(_sys_items ${TINY3D_SYSTEM_INCLUDE_DIRS})
    _tiny3d_json_array_literal(_flag_items
        -x c++ -std=c++17 -fsyntax-only ${TINY3D_REFLECT_TARGET_FLAGS})

    set(_content "{
    \"IncludePath\": [
${_inc_items}    ],
    \"SystemIncludePath\": [
${_sys_items}    ],
    \"MacroDefinition\": [
${_def_items}    ],
    \"OtherFlags\": [
${_flag_items}    ],
    \"GeneratedPath\": \"${_gen_json}\"
}
")

    file(GENERATE
        OUTPUT "${T3DW_SETTINGS_DIR}/$<CONFIG>/ReflectionSettings.json"
        CONTENT "${_content}")

    # 调用方要把这些文件挂进 rpp 的 DEPENDS，配置一变就得重跑反射。这里逐个配置
    # 列出而不是回一个带 $<CONFIG> 的路径：DEPENDS 里的生成器表达式要较新的 CMake
    # 策略才认，而本工程的 cmake_minimum_required 还很老。
    if (T3DW_OUT_SETTINGS_FILES)
        set(_configs ${CMAKE_CONFIGURATION_TYPES})
        if (NOT _configs)
            set(_configs "${CMAKE_BUILD_TYPE}")
        endif ()

        set(_files "")
        foreach (_cfg IN LISTS _configs)
            list(APPEND _files "${T3DW_SETTINGS_DIR}/${_cfg}/ReflectionSettings.json")
        endforeach ()

        set(${T3DW_OUT_SETTINGS_FILES} "${_files}" PARENT_SCOPE)
    endif ()
endfunction()

#-------------------------------------------------------------------------------
# tiny3d_add_reflection_target(<target>
#   SOURCE_DIR    <dir>      # rpp 的扫描根
#   GENERATED_DIR <dir>      # *.generated.cpp 落脚处
#   SETTINGS_DIR  <dir>      # ReflectionSettings.json 写出目录（内部再分 $<CONFIG>）
#   WHITE_LIST    <class>... # 不打标签也自动反射的模板
# )
#
# 引擎模块的反射接线：写出配置，并挂一个构建期跑 rpp 的 target。
#
# rpp 只在构建期跑。以前是 configure 期 execute_process 跑一遍，为的是让紧接着的
# file(GLOB) 能收到产物；产物改成预声明之后（见 TINY3D_DECLARE_GENERATED_SOURCES）
# 这个理由就没了，而配置本身要等 generate 期才落盘，configure 期根本读不到。
#-------------------------------------------------------------------------------
function(tiny3d_add_reflection_target TARGET)
    set(oneValueArgs SOURCE_DIR GENERATED_DIR SETTINGS_DIR)
    set(multiValueArgs WHITE_LIST EXTRA_INCLUDES)
    cmake_parse_arguments(T3DA "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET "${TARGET}")
        message(FATAL_ERROR "tiny3d_add_reflection_target: target '${TARGET}' 不存在")
    endif ()
    foreach (_required SOURCE_DIR GENERATED_DIR SETTINGS_DIR)
        if (NOT T3DA_${_required})
            message(FATAL_ERROR "tiny3d_add_reflection_target: 缺少参数 ${_required}")
        endif ()
    endforeach ()

    get_filename_component(_source_dir "${T3DA_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(_generated_dir "${T3DA_GENERATED_DIR}" ABSOLUTE)
    get_filename_component(_settings_dir "${T3DA_SETTINGS_DIR}" ABSOLUTE)

    file(MAKE_DIRECTORY "${_generated_dir}")

    tiny3d_write_reflection_settings(${TARGET}
        SETTINGS_DIR       "${_settings_dir}"
        SOURCE_DIR         "${_source_dir}"
        GENERATED_DIR      "${_generated_dir}"
        EXTRA_INCLUDES     ${T3DA_EXTRA_INCLUDES}
        OUT_SETTINGS_FILES _settings_files)

    _tiny3d_find_rpp(_rpp_cfg _rpp_bld)
    if (NOT _rpp_bld)
        message(FATAL_ERROR
            "tiny3d_add_reflection_target: 找不到 rpp（target '${TARGET}'）。"
            "先构建 ReflectionPreprocessor，或设置 TINY3D_SDK_RPP / TINY3D_HOST_RPP。")
    endif ()

    # 反射产物按 GENERATED_DIR 组织（含 rpp 自己的 .deps / .tpl 增量缓存），同一个
    # GENERATED_DIR 下只该有一次 rpp，所以 target 名按它派生而不是按 TARGET 派生。
    file(RELATIVE_PATH _reflect_rel "${CMAKE_BINARY_DIR}" "${_generated_dir}")
    if (_reflect_rel MATCHES "^\\.\\." OR IS_ABSOLUTE "${_reflect_rel}")
        set(_reflect_rel "${_generated_dir}")
    endif ()
    string(MAKE_C_IDENTIFIER "${_reflect_rel}" _reflect_id)
    set(_reflect_target "reflect_${_reflect_id}")
    set(_reflect_stamp "${_generated_dir}/.rpp.stamp")

    if (NOT TARGET ${_reflect_target})
        # 裸 add_custom_target 没有输出文件，CMake 一律当作 always-out-of-date，每次
        # build 都要把 rpp 启一遍：扫全部头文件找反射宏、校验 .deps/.tpl、算依赖 hash，
        # 一行没改也得好几秒。挂个 stamp 输出并声明输入之后，源码没动就完全不启动。
        file(GLOB_RECURSE _reflect_inputs CONFIGURE_DEPENDS
            "${_source_dir}/*.h"
            "${_source_dir}/*.hpp"
            "${_source_dir}/*.hh"
            "${_source_dir}/*.inl"
            "${_source_dir}/*.cpp"
            "${_source_dir}/*.cxx")
        list(FILTER _reflect_inputs EXCLUDE REGEX "\\.generated\\.cpp$")
        list(FILTER _reflect_inputs EXCLUDE REGEX "/(CMakeFiles|\\.cxx|\\.gradle)/")

        # rpp 的 -W 收一个分号分隔的字符串。转义分号才不会在展开时被拆成多个参数。
        set(_rpp_args "")
        if (T3DA_WHITE_LIST)
            string(REPLACE ";" "\\;" _white "${T3DA_WHITE_LIST}")
            set(_rpp_args -W "${_white}")
        endif ()

        add_custom_command(
            OUTPUT "${_reflect_stamp}"
            COMMAND ${_rpp_bld} "${_settings_dir}/$<CONFIG>" "${_source_dir}" -b -j 8 ${_rpp_args}
            COMMAND "${CMAKE_COMMAND}" -E touch "${_reflect_stamp}"
            DEPENDS ${_reflect_inputs} ${_settings_files}
            COMMENT "Running reflection generation: ${_source_dir}"
            VERBATIM)

        add_custom_target(${_reflect_target} DEPENDS "${_reflect_stamp}")

        if (TARGET ReflectionPreprocessor)
            add_dependencies(${_reflect_target} ReflectionPreprocessor)
        endif ()
        set_property(TARGET ${_reflect_target} PROPERTY FOLDER "Reflect")
    endif ()

    add_dependencies(${TARGET} ${_reflect_target})

    if (TARGET T3DReflect)
        add_dependencies(T3DReflect ${_reflect_target})
    endif ()
endfunction()

#-------------------------------------------------------------------------------
# _tiny3d_enable_reflection_native(<target> ...)
#
# tiny3d_enable_reflection() 在新链路下的实现：业务工程自己产出一份完整的反射配置，
# 不再读引擎的 JSON 当底板往 IncludePath 里正则插值。
#-------------------------------------------------------------------------------
function(_tiny3d_enable_reflection_native TARGET)
    set(oneValueArgs SOURCE_DIR GENERATED_DIR SETTINGS_DIR)
    set(multiValueArgs EXTRA_INCLUDES)
    cmake_parse_arguments(T3DN "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(_source_dir "${T3DN_SOURCE_DIR}" ABSOLUTE)
    get_filename_component(_generated_dir "${T3DN_GENERATED_DIR}" ABSOLUTE)

    # 产物清单按源文件名推导。CONFIGURE_DEPENDS 让用户新增 .cpp 之后 CMake 自己
    # 察觉，不必手动重新 configure —— 引擎侧的 SET_PROJECT_FILES 一直带着它，业务
    # 工程这边以前漏了。
    file(GLOB_RECURSE _src_cpp CONFIGURE_DEPENDS
        "${_source_dir}/*.cpp"
        "${_source_dir}/*.cxx")

    # 工程目录里常有构建残留（gradle 的 .cxx/、CMake 自己的 CMakeFiles/），里面的
    # CMakeCXXCompilerId.cpp 之类会被当成源文件。它们不属于任何 target，跟着编只是
    # 徒增噪音，更麻烦的是残留文件名可能和真源文件撞上，产物就对应到错的那个。
    list(FILTER _src_cpp EXCLUDE REGEX "/(CMakeFiles|\\.cxx|\\.gradle)/")

    set(_generated "")
    foreach (_src IN LISTS _src_cpp)
        get_filename_component(_fname "${_src}" NAME)
        if (NOT _fname MATCHES "\\.generated\\.cpp$")
            get_filename_component(_title "${_src}" NAME_WE)
            list(APPEND _generated "${_generated_dir}/${_title}.generated.cpp")
        endif ()
    endforeach ()
    if (_generated)
        list(REMOVE_DUPLICATES _generated)
    endif ()

    # Templates.generated.cpp 有意不编进业务工程：里面是引擎类型的模板实例，
    # T3DCore 已经注册过一遍，而且它的 #include 是相对引擎 include 路径写的，
    # 业务工程这边根本编不过去。详见设计文档 §5.4.6 与 §11。
    #
    # 以前这个排除是「按 .cpp 名推导恰好漏掉」的隐式副作用，聚合之后成了这一行。

    # rpp 构建期才写出这些文件，先声明出来，别让 CMake 在 configure 期就去找
    set_source_files_properties(${_generated} PROPERTIES GENERATED TRUE)

    list(LENGTH _generated _ngen)
    message(STATUS "tiny3d_enable_reflection: ${TARGET} -> ${_ngen} generated source(s) in ${_generated_dir}")

    if (_generated)
        target_sources(${TARGET} PRIVATE ${_generated})
        source_group("Generated" FILES ${_generated})
    endif ()

    tiny3d_add_reflection_target(${TARGET}
        SOURCE_DIR     "${_source_dir}"
        GENERATED_DIR  "${_generated_dir}"
        SETTINGS_DIR   "${T3DN_SETTINGS_DIR}"
        EXTRA_INCLUDES ${T3DN_EXTRA_INCLUDES})
endfunction()
