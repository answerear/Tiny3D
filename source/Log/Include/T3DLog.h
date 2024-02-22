/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_LOG_H__
#define __T3D_LOG_H__


#include <T3DLogger.h>
#include <T3DLogErrorDef.h>

#define __T3D_OPEN_LOG__


#if defined (__T3D_OPEN_LOG__)

    #define T3D_LOG_TO_LEVEL_STR(l) \
        ((Tiny3D::Logger::getInstancePtr() != nullptr) ? \
        T3D_LOGGER.toLevelString(l) : Tiny3D::Logger::LEVEL_OFF);

    #define T3D_LOG_TO_LEVEL_VALUE(l) \
        ((Tiny3D::Logger::getInstancePtr() != nullptr) ? \
        T3D_LOGGER.toLevelValue(l) : Tiny3D::Logger::E_LEVEL_OFF);

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

    #define T3D_LOG_STARTUP(app_id, tag, force_output, output_console)  \
         if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
         {   \
             T3D_LOGGER.startup(app_id, tag, force_output, output_console);  \
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

    #define T3D_LOG_TRACE(level, tag, fmt, ...)  \
         if (Tiny3D::Logger::getInstancePtr() != nullptr)    \
         {   \
             T3D_LOGGER.trace(level, __FILE__, __LINE__, tag, fmt, ##__VA_ARGS__);    \
         }

    #define T3D_LOG_FATAL(tag, fmt, ...)         \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_FATAL, tag, fmt, ##__VA_ARGS__)

    #define T3D_LOG_CRITICAL(tag, fmt, ...)      \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_CRITICAL, tag, fmt, ##__VA_ARGS__)

    #define T3D_LOG_ERROR(tag, fmt, ...)         \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)

    #define T3D_LOG_WARNING(tag, fmt, ...)       \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_WARNING, tag, fmt, ##__VA_ARGS__)

    #define T3D_LOG_INFO(tag, fmt, ...)          \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_INFO, tag, fmt, ##__VA_ARGS__)

    #define T3D_LOG_DEBUG(tag, fmt, ...)         \
        T3D_LOG_TRACE(Tiny3D::Logger::E_LEVEL_DEBUG, tag, fmt, ##__VA_ARGS__)

#else
    #define T3D_LOG_TO_LEVEL_STR(l)
    #define T3D_LOG_TO_LEVEL_VALUE(l)
    #define T3D_LOG_SET_LEVEL(l)
    #define T3D_LOG_SET_EXPIRED(e)
    #define T3D_LOG_SET_MAX_CACHE_SIZE(s)
    #define T3D_LOG_SET_MAX_CACHE_TIME(t)
    #define T3D_LOG_STARTUP(app_id, tag, force_output, output_console)
    #define T3D_LOG_SHUTDOWN()
    #define T3D_LOG_ENTER_BACKGROUND()
    #define T3D_LOG_ENTER_FOREGROUND()
    #define T3D_LOG_TRACE(level, tag, fmt, ...)
    #define T3D_LOG_FATAL(tag, fmt, ...)
    #define T3D_LOG_CRITICAL(tag, fmt, ...)
    #define T3D_LOG_ERROR(tag, fmt, ...)
    #define T3D_LOG_WARNING(tag, fmt, ...)
    #define T3D_LOG_INFO(tag, fmt, ...)
    #define T3D_LOG_DEBUG(tag, fmt, ...)
#endif


#endif  /*__T3D_LOG_H__*/
