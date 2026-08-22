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
	add_subdirectory(CompileCommandTool)
	# libclang.dylib 与目标架构对不上时不编 rpp（例如只剩单一切片的旧包）。
	# 当前预编译是 x86_64+arm64 universal，和 Intel / Apple Silicon 都能对上。
	set(_tiny3d_build_rpp TRUE)
	if (TINY3D_OS_MACOSX AND CMAKE_OSX_ARCHITECTURES)
		set(_tiny3d_libclang "${TINY3D_DEP_DIR}/llvm/prebuilt/OSX/libclang.dylib")
		if (EXISTS "${_tiny3d_libclang}")
			execute_process(
				COMMAND lipo -archs "${_tiny3d_libclang}"
				OUTPUT_VARIABLE _tiny3d_libclang_arch
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
			if (NOT _tiny3d_libclang_arch MATCHES "${CMAKE_OSX_ARCHITECTURES}")
				set(_tiny3d_build_rpp FALSE)
				message(STATUS "Skipping ReflectionPreprocessor (libclang is ${_tiny3d_libclang_arch}, target is ${CMAKE_OSX_ARCHITECTURES})")
			endif ()
		endif ()
		unset(_tiny3d_libclang)
		unset(_tiny3d_libclang_arch)
	endif ()
	if (_tiny3d_build_rpp)
		add_subdirectory(ReflectionPreprocessor)
	endif ()
	unset(_tiny3d_build_rpp)
	if (NOT TINY3D_BUILD_RTTR_TOOL)
		add_subdirectory(MeshConverter)
		#add_subdirectory(MetaGenerator)
		if (TINY3D_BUILD_SHADERCONDUCTOR_TOOLS)
			add_subdirectory(ShaderCrossCompiler)
			add_subdirectory(BuiltinGenerator)
		endif (TINY3D_BUILD_SHADERCONDUCTOR_TOOLS)
		add_subdirectory(BundleBuilder)
	endif (NOT TINY3D_BUILD_RTTR_TOOL)
endif (TINY3D_OS_DESKTOP)

