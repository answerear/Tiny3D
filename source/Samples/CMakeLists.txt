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

add_subdirectory(HelloApp)
add_subdirectory(CameraApp)
add_subdirectory(GeometryApp)
add_subdirectory(SceneApp)
add_subdirectory(DirectionalLightApp)
add_subdirectory(PointLightApp)
add_subdirectory(SpotLightApp)
add_subdirectory(LightApp)
add_subdirectory(ShadowApp)
add_subdirectory(TextureApp)

if (TINY3D_OS_DESKTOP)
	add_subdirectory(TransformationApp)
	add_subdirectory(IntersectionApp)
endif (TINY3D_OS_DESKTOP)

