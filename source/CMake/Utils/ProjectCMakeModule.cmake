####################################################################################################
# Copyright Tiny Studio, 2015
# Created by aa
#
# This file suply some cmake module function help to setup project
####################################################################################################


# Set project name
# project_name : name of project
# Usage : SET_PROJECT_NAME(ProjectName)
MACRO(SET_PROJECT_NAME project_name)
	SET(LIB_NAME ${project_name})
	SET(BIN_NAME ${project_name})
	STRING(TOUPPER ${LIB_NAME} LIB_NAME_TOUPPER)
ENDMACRO(SET_PROJECT_NAME)


FUNCTION(ADD_PROJECT_FILES file_list group dir)
	FOREACH(var ${ARGN})
		LIST(APPEND temp_files ${dir}${var})
		SOURCE_GROUP(${group} FILES ${dir}${var})
	ENDFOREACH(var)
	
	SET(file_list "${temp_files}" PARENT_SCOPE)
ENDFUNCTION(ADD_PROJECT_FILES)


FUNCTION(ADD_PROJECT_FILES_BY_EXT out_files group dir ext)
	FILE(GLOB var ${dir}*${ext})
	
	FOREACH(f ${var})
		GET_FILENAME_COMPONENT(name ${f} NAME_WE)
		ADD_PROJECT_FILES(file_list ${group} ${dir} "${name}${ext}")
		LIST(APPEND temp_list ${file_list})
	ENDFOREACH()

	SET(out_files "${temp_list}" PARENT_SCOPE)
ENDFUNCTION(ADD_PROJECT_FILES_BY_EXT)


MACRO(SET_PROJECT_FILES group dir ext)
	ADD_PROJECT_FILES_BY_EXT(out_files ${group} ${dir} ${ext})
	LIST(APPEND SOURCE_FILES ${out_files})
ENDMACRO(SET_PROJECT_FILES)
	