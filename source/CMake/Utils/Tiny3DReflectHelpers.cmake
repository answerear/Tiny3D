#-------------------------------------------------------------------------------
# Tiny3D 业务工程反射生成
#
# 复用引擎 nmake/Core/Runtime 产出的 ReflectionSettings.json（含 libclang 所需的
# 系统头与宏），追加业务 include 后调用 rpp，把 *.generated.cpp 编进目标。
#
# rpp 会把 GeneratedPath 拼到 SOURCE_DIR 后面，因此 JSON 里必须写相对 SOURCE_DIR
# 的路径，不能写绝对路径。
#-------------------------------------------------------------------------------

include_guard(GLOBAL)

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

    if (_run_rpp)

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

        set(_reflect_target "reflect_${TARGET}")
        if (NOT TARGET ${_reflect_target})
            add_custom_target(${_reflect_target}
                COMMAND ${_rpp_bld} "${T3DR_SETTINGS_DIR}" "${T3DR_SOURCE_DIR}" -b -j 8
                COMMENT "Running reflection generation: ${TARGET}")
            if (TARGET ReflectionPreprocessor)
                add_dependencies(${_reflect_target} ReflectionPreprocessor)
            endif ()
            set_property(TARGET ${_reflect_target} PROPERTY FOLDER "Reflect")
        endif ()
        add_dependencies(${TARGET} ${_reflect_target})
    endif ()
endfunction()
