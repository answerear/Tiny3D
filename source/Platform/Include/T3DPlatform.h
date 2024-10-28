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

#ifndef __T3D_PLATFORM_H__
#define __T3D_PLATFORM_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class IPlatform;
    class IFactory;
    class ThreadSingleton;
    class ThreadManager;
    
    /**
     * @class System
     * @brief 系统类.
     * @note 该类对一些全局的单例创建和释放，为上层提供框架层和系统层统一接口.
     */
    class T3D_PLATFORM_API Platform : public Allocator, public Singleton<Platform>
    {
    public:
        /**
         * @brief Constructor for System.
         */
        Platform();

        /**
         * @brief Destructor for System.
         */
        ~Platform();

        /**
         * @brief 初始化系统
         * @note 没有调用初始化之前，部分功能无法使用
         */
        int32_t init();

        /**
         * @brief 每个程序循环调用处理.
         */
        void poll();

        /**
         * \brief 内存屏障
         */
        void memoryBarrier();

        /**
         * @brief 获取操作系统适配层工厂接口对象
         */
        IFactory &getPlatformFactory()
        {
            return (*mPlatformFactory);
        }

        IPlatform *getPlatformImpl()
        {
            return mPlatform;
        }

    private:
        IFactory        *mPlatformFactory {nullptr};
        IPlatform       *mPlatform {nullptr};
        ThreadManager   *mThreadMgr {nullptr};
        TimerManager    *mTimerMgr {nullptr};
        Console         *mConsole {nullptr};
        Locale          *mLocale {nullptr};
        DeviceInfo      *mDeviceInfo {nullptr};
        ThreadSingleton *mThreadSingleton {nullptr};
    };

    #define T3D_PLATFORM            (Platform::getInstance())
    #define T3D_PLATFORM_FACTORY    (T3D_PLATFORM.getPlatformFactory())
}


#endif  /*__T3D_PLATFORM_H__*/
