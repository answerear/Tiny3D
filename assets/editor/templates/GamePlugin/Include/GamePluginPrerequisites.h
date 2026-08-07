/*******************************************************************************
 * {ProjectName} —— 业务逻辑插件
 ******************************************************************************/

#pragma once


#include <Tiny3D.h>


// 只有 dllStartPlugin / dllStopPlugin 需要导出，业务类都是通过 RTTR 反射访问的，
// 不需要挂这个宏。
#if defined GAMEPLUGIN_EXPORT
    #define GAME_PLUGIN_API     T3D_EXPORT_API
#else
    #define GAME_PLUGIN_API     T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define LOG_TAG_GAME    "{ProjectName}"

    #define GAME_LOG_ERROR(fmt, ...)    T3D_LOG_ERROR(LOG_TAG_GAME, fmt, ##__VA_ARGS__)
    #define GAME_LOG_WARNING(fmt, ...)  T3D_LOG_WARNING(LOG_TAG_GAME, fmt, ##__VA_ARGS__)
    #define GAME_LOG_INFO(fmt, ...)     T3D_LOG_INFO(LOG_TAG_GAME, fmt, ##__VA_ARGS__)
    #define GAME_LOG_DEBUG(fmt, ...)    T3D_LOG_DEBUG(LOG_TAG_GAME, fmt, ##__VA_ARGS__)
}
