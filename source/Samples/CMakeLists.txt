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
add_subdirectory(BehaviourApp)
add_subdirectory(PostProcessingApp)
add_subdirectory(InputApp)
add_subdirectory(TextureApp)
add_subdirectory(SkeletalAnimationApp)
add_subdirectory(ResourceApp)
add_subdirectory(SkyboxApp)
add_subdirectory(ComputeApp)

if (TINY3D_BUILD_RMLUI)
    add_subdirectory(RmlUiApp)
endif (TINY3D_BUILD_RMLUI)

if (TINY3D_OS_DESKTOP)
    add_subdirectory(TransformationApp)
    add_subdirectory(IntersectionApp)
    add_subdirectory(PrefabApp)
endif (TINY3D_OS_DESKTOP)

if (TINY3D_OS_ANDROID AND TINY3D_BUILD_RMLUI)
    # 共享的 Android Tiny3D.cfg 会加载 T3DRmlUi。各样例 APK 只打包自己拷贝的 .so，
    # 这里给已有 Android 工程补上运行时 UI 库，避免其它样例启动时找不到插件。
    set(_t3d_android_samples
        HelloApp CameraApp GeometryApp SceneApp DirectionalLightApp PointLightApp
        SpotLightApp LightApp ShadowApp BehaviourApp PostProcessingApp InputApp
        TextureApp SkeletalAnimationApp ResourceApp SkyboxApp ComputeApp
        PlatformApp FrameworkApp RmlUiApp)
    foreach(_t3d_android_sample IN LISTS _t3d_android_samples)
        if(TARGET ${_t3d_android_sample})
            set(_t3d_android_libs "${CMAKE_CURRENT_SOURCE_DIR}/${_t3d_android_sample}/Android/app/libs/${ANDROID_ABI}")
            add_custom_command(TARGET ${_t3d_android_sample} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${_t3d_android_libs}"
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:T3DRmlUi> "${_t3d_android_libs}"
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rmlui_core> "${_t3d_android_libs}"
                )
        endif()
    endforeach()
    unset(_t3d_android_libs)
    unset(_t3d_android_sample)
    unset(_t3d_android_samples)
endif ()
