#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#
# The contents of this file are placed in the public domain. 
# Feel free to make use of it in any way you like.
#-------------------------------------------------------------------------------



set(TINY3D_PLATFORM_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Platform/Include")
set(TINY3D_SYSTEM_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../System/Include")
set(TINY3D_LOG_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Log/Include")
set(TINY3D_UTILS_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Utils/Include")
set(TINY3D_FRAMEWORK_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Framework/Include")
set(TINY3D_MATH_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Math/Include")
set(TINY3D_CORE_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Core/Include")
set(TINY3D_DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")


if (TINY3D_OS_DESKTOP)
    #add_subdirectory(ScriptCompiler)
	add_subdirectory(CompileCommandTool)
	add_subdirectory(ReflectionPreprocessor)
	if (NOT TINY3D_BUILD_RTTR_TOOL)
		#add_subdirectory(MeshConverter)
		#add_subdirectory(MetaGenerator)
		add_subdirectory(ShaderCrossCompiler)
	endif (NOT TINY3D_BUILD_RTTR_TOOL)
endif (TINY3D_OS_DESKTOP)

