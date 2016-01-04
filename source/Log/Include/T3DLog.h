

#ifndef __T3D_LOG_H__
#define __T3D_LOG_H__


#include "T3DLogger.h"


#define __T3D_OPEN_LOG__


#if defined (__T3D_OPEN_LOG__)

#define T3D_LOG_TO_LEVEL_STR(l) \
    ((Tiny3D::Logger::getInstancePtr() != nullptr) ? T3D_LOGGER.toLevelString(l) : Tiny3D::Logger::LEVEL_OFF);

#define T3D_LOG_TO_LEVEL_VALUE(l) \
    ((Tiny3D::Logger::getInstancePtr() != nullptr) ? T3D_LOGGER.toLevelValue(l) : Tiny3D::Logger::E_LEVEL_OFF);

#define T3D_LOG_SET_LEVEL(l)    \
    if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
    {    \
        T3D_LOGGER.setLevel(l); \
    }

#define T3D_LOG_SET_EXPIRED(e)    \
    if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
    {    \
        T3D_LOGGER.setExpired(e); \
    }

#define T3D_LOG_SET_MAX_CACHE_SIZE(s)    \
    if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
    {    \
        T3D_LOGGER.setMaxCacheSize(s); \
    }

#define T3D_LOG_SET_MAX_CACHE_TIME(t)    \
    if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
    {    \
        T3D_LOGGER.setMaxCacheTime(t); \
    }

#define T3D_LOG_STARTUP(app_id, tag, force_output)  \
     if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
     {   \
         T3D_LOGGER.startup(app_id, tag, force_output);  \
     }

#define T3D_LOG_SHUTDOWN()  \
     if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
     {   \
         T3D_LOGGER.shutdown();  \
     }

#define T3D_LOG_ENTER_BACKGROUND()  \
     if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
     {   \
         T3D_LOGGER.enterBackground();   \
     }

#define T3D_LOG_ENTER_FOREGROUND()  \
     if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
     {   \
         T3D_LOGGER.enterForeground();   \
     }

#define T3D_LOG_TRACE(level, fmt, ...)  \
     if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
     {   \
         T3D_LOGGER.trace(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__);    \
     }

#define T3D_LOG_FATAL(fmt, ...)         T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_FATAL)
#define T3D_LOG_CRITICAL(fmt, ...)      T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_CRITICAL)
#define T3D_LOG_ERROR(fmt, ...)         T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_ERROR)
#define T3D_LOG_WARNING(fmt, ...)       T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_WARNING)
#define T3D_LOG_INFO(fmt, ...)          T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_INFO)
#define T3D_LOG_DEBUG(fmt, ...)         T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_DEBUG)
#else
#define T3D_LOG_TO_LEVEL_STR(l)
#define T3D_LOG_TO_LEVEL_VALUE(l)
#define T3D_LOG_SET_LEVEL(l)
#define T3D_LOG_SET_EXPIRED(e)
#define T3D_LOG_SET_MAX_CACHE_SIZE(s)
#define T3D_LOG_SET_MAX_CACHE_TIME(t)
#define T3D_LOG_STARTUP(app_id, tag, force_output)
#define T3D_LOG_SHUTDOWN()
#define T3D_LOG_ENTER_BACKGROUND()
#define T3D_LOG_ENTER_FOREGROUND()
#define T3D_LOG_TRACE(level, fmt, ...)
#define T3D_LOG_FATAL(fmt, ...)
#define T3D_LOG_CRITICAL(fmt, ...)
#define T3D_LOG_ERROR(fmt, ...)
#define T3D_LOG_WARNING(fmt, ...)
#define T3D_LOG_INFO(fmt, ...)
#define T3D_LOG_DEBUG(fmt, ...)
#endif


#endif  /*__T3D_LOG_H__*/
