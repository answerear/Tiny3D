#-------------------------------------------------------------------------------
# SPIRV-Cross 精简构建：只收 GLSL / HLSL / MSL 三个后端。
# 源码 vendor 在本目录（版本见 VERSION.txt），跟随主工程架构编译（含 macOS universal）。
#-------------------------------------------------------------------------------

set(SPIRV_CROSS_SOURCES
    spirv_cross.cpp
    spirv_parser.cpp
    spirv_cross_parsed_ir.cpp
    spirv_cfg.cpp
    spirv_cross_util.cpp
    spirv_glsl.cpp
    spirv_hlsl.cpp
    spirv_msl.cpp
    )

# 主工程在 Windows 上全局定义了 _HAS_EXCEPTIONS=0。SPIRV-Cross 靠抛 CompilerError 报错，
# 关掉异常后 STL 与 throw 的行为都不可靠，必须还原
remove_definitions(-D_HAS_EXCEPTIONS=0)

add_library(spirv-cross STATIC ${SPIRV_CROSS_SOURCES})

target_include_directories(spirv-cross PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# 这个静态库要被链进动态库 HLSLCross，必须编成位置无关代码。缺了它 Linux 上会报
#   relocation R_X86_64_32 against '.rodata' can not be used when making a shared object
set_target_properties(spirv-cross PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (NOT MSVC)
    # 与 HLSLCross 的 CXX_VISIBILITY_PRESET 配套，否则静态库里的符号仍是 default 可见
    set_target_properties(spirv-cross PROPERTIES
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN ON
        )
endif ()

# 所有配置都关掉 assert。SPIRV-Cross 内部有些 assert 在合法输入下也会触发（Release 下被编译掉、
# 产物正常），Debug 下会弹 CRT 断言对话框把批处理的 scc 挂住。PUBLIC 是因为头文件里的内联函数
# 也带 assert，会在 HLSLCross 里展开
target_compile_definitions(spirv-cross PUBLIC NDEBUG)

if (MSVC)
    target_compile_options(spirv-cross PRIVATE /EHsc /wd4996 /wd4267 /wd4244)
else ()
    target_compile_options(spirv-cross PRIVATE -fexceptions -Wno-deprecated-declarations)
endif ()

set_property(TARGET spirv-cross PROPERTY FOLDER "Dependencies")
