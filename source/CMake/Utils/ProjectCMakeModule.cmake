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
	