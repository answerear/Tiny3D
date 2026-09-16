####################################################################################################
# Copyright Tiny Studio, 2015
# Created by aa
#
# This file suply some cmake module function help to setup project
####################################################################################################


# Set project name
# project_name : name of project
# Usage : SET_PROJECT_NAME(ProjectName)
macro(SET_PROJECT_NAME project_name)
	set(LIB_NAME ${project_name})
	set(BIN_NAME ${project_name})
	string(TOUPPER ${LIB_NAME} LIB_NAME_TOUPPER)
endmacro(SET_PROJECT_NAME)


function(ADD_PROJECT_FILES file_list group dir)
	foreach(var ${ARGN})
		list(APPEND temp_files ${dir}${var})
		source_group(${group} FILES ${dir}${var})
	endforeach(var)
	
	set(file_list "${temp_files}" PARENT_SCOPE)
endfunction(ADD_PROJECT_FILES)


function(ADD_PROJECT_FILES_BY_EXT out_files group dir ext)
	file(GLOB var CONFIGURE_DEPENDS ${dir}*${ext})

	foreach(f ${var})
		get_filename_component(name ${f} NAME_WE)
		add_project_files(file_list ${group} ${dir} "${name}${ext}")
		list(APPEND temp_list ${file_list})
	endforeach()

	set(out_files "${temp_list}" PARENT_SCOPE)
endfunction(ADD_PROJECT_FILES_BY_EXT)


macro(SET_PROJECT_FILES group dir ext)
	add_project_files_by_ext(out_files ${group} ${dir} ${ext})
	list(APPEND SOURCE_FILES ${out_files})
endmacro(SET_PROJECT_FILES)


# 声明本工程的反射产物，追加进 SOURCE_FILES
# group : Visual Studio 里的分组名
# dir   : 产物目录，必须以分隔符结尾
# Usage : TINY3D_DECLARE_GENERATED_SOURCES(Generated ${dir}/)
#
# 产物清单由「已收集的源文件」推导，而不是去 glob 产物目录：产物是 rpp 在构建期
# 写出来的，configure 期磁盘上可能一个都没有。glob 的话首次 configure 会整批漏掉，
# 必须再 configure 一次才编得上，反射缺失还要等到运行期才暴露。
#
# 前提是 rpp 对扫描到的每个源文件都产出同名产物（没有反射内容的写个空翻译单元），
# 这样两边的清单出自同一个源头。模板实例的产物名推不出来 —— 模板定义在本模块源码
# 树之外 —— rpp 把它们聚合进固定名字的 Templates.generated.cpp。
#
# 详见 doc/todo/Reflection-Settings-CMake-Native-Design-todo.md §5.2 / §5.4
macro(TINY3D_DECLARE_GENERATED_SOURCES group dir)
	if (NOT TINY3D_REFLECT_CMAKE_NATIVE)
		# 旧链路：产物由 generate-*.bat 在 configure 之前就写好了，直接 glob
		set_project_files(${group} ${dir} .generated.cpp)
	else ()
		set(_t3d_gen_files "")

		foreach (_t3d_gen_src IN LISTS SOURCE_FILES)
			get_filename_component(_t3d_gen_ext "${_t3d_gen_src}" EXT)
			if (_t3d_gen_ext STREQUAL ".cpp" OR _t3d_gen_ext STREQUAL ".cxx")
				get_filename_component(_t3d_gen_title "${_t3d_gen_src}" NAME_WE)
				list(APPEND _t3d_gen_files "${dir}${_t3d_gen_title}.generated.cpp")
			endif ()
		endforeach ()

		set(_t3d_gen_templates "${dir}Templates.generated.cpp")
		list(APPEND _t3d_gen_files "${_t3d_gen_templates}")
		list(REMOVE_DUPLICATES _t3d_gen_files)

		# 这些文件构建期才存在，得先声明出来，否则 CMake 在 configure 期就报找不到
		set_source_files_properties(${_t3d_gen_files} PROPERTIES GENERATED TRUE)

		if (MSVC)
			# 聚合产物一个翻译单元里装着几十个模板实例的注册，rttr 的 type_data 又
			# 给每个类型摊开一大片 COMDAT，超过了 COFF 的 65279 段上限（C1128）。
			# 聚合之前这些注册分散在各自的产物里，所以以前碰不到这条线。
			set_source_files_properties("${_t3d_gen_templates}"
				PROPERTIES COMPILE_OPTIONS "/bigobj")
		endif ()
		source_group(${group} FILES ${_t3d_gen_files})
		list(APPEND SOURCE_FILES ${_t3d_gen_files})

		list(LENGTH _t3d_gen_files _t3d_gen_count)
		message(STATUS "${BIN_NAME} : ${_t3d_gen_count} reflection product(s) declared in ${dir}")

		unset(_t3d_gen_files)
		unset(_t3d_gen_templates)
		unset(_t3d_gen_src)
		unset(_t3d_gen_ext)
		unset(_t3d_gen_title)
		unset(_t3d_gen_count)
	endif ()
endmacro(TINY3D_DECLARE_GENERATED_SOURCES)

# macOS Sample：把 cfg / 引擎 dylib / 插件 / SDL / 资源拷到可执行文件目录。
function(tiny3d_setup_macos_sample target_name)
    set_target_properties(${target_name} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        MACOSX_RPATH 1
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
    )

    set(_macos_dir "$<TARGET_FILE_DIR:${target_name}>")
    set(_cfg "${CMAKE_SOURCE_DIR}/../assets/config/OSX/Tiny3D.cfg")
    set(_sdl "${CMAKE_SOURCE_DIR}/../dependencies/sdl2/prebuilt/osx/libSDL2-2.0.0.dylib")
    set(_freeimage "${CMAKE_SOURCE_DIR}/../dependencies/freeimage/prebuilt/OSX/libfreeimage-3.18.0.dylib")

    add_custom_command(TARGET ${target_name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_cfg}"
            "${_macos_dir}/Tiny3D.cfg"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:T3DPlatform>"
            "$<TARGET_FILE:T3DSystem>"
            "$<TARGET_FILE:T3DLog>"
            "$<TARGET_FILE:T3DUtils>"
            "$<TARGET_FILE:T3DMath>"
            "$<TARGET_FILE:T3DFramework>"
            "$<TARGET_FILE:T3DCore>"
            "$<TARGET_FILE:rttr_core>"
            "$<TARGET_FILE:MetalRenderer>"
            "$<TARGET_FILE:FileSystemArchive>"
            "$<TARGET_FILE:FreeImageCodec>"
            "$<TARGET_FILE:BundleFSArchive>"
            "${_macos_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_sdl}"
            "${_macos_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_freeimage}"
            "${_macos_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_macos_dir}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../assets/icon"
            "${_macos_dir}/assets/icon"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/../assets/samples"
            "${_macos_dir}/assets/samples"
        COMMENT "Packaging ${target_name} macOS runtime files"
    )

    add_dependencies(${target_name}
        MetalRenderer
        FileSystemArchive
        FreeImageCodec
        BundleFSArchive
    )
endfunction()

# TransformationApp / IntersectionApp 这类控制台 Sample，只需要 rpath 和内核库。
function(tiny3d_setup_macos_console_sample target_name)
    set_target_properties(${target_name} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        MACOSX_RPATH 1
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
    )

    set(_macos_dir "$<TARGET_FILE_DIR:${target_name}>")
    add_custom_command(TARGET ${target_name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:T3DPlatform>"
            "$<TARGET_FILE:T3DSystem>"
            "$<TARGET_FILE:T3DLog>"
            "$<TARGET_FILE:T3DUtils>"
            "$<TARGET_FILE:T3DMath>"
            "$<TARGET_FILE:T3DFramework>"
            "$<TARGET_FILE:T3DCore>"
            "$<TARGET_FILE:rttr_core>"
            "${_macos_dir}"
        COMMENT "Packaging ${target_name} macOS console runtime files"
    )
endfunction()
	