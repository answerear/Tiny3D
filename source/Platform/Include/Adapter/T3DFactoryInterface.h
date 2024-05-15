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

#ifndef __T3D_FACTORY_INTERFACE_H__
#define __T3D_FACTORY_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class IPlatform;
    class IWindow;
    class IApplication;
    class ITime;
    class ITimerService;
    class IDir;
    class IDeviceInfo;
    class IConsole;
    class IThread;
    class IThreadSingleton;
    class ICriticalSection;
    class IMutex;
    class IRecursiveMutex;
    class ISemaphore;
    class IEvent;
    class IWaitCondition;
    class IProcess;

    enum EPlatform
    {
        E_PLATFORM_UNKNOWN = 0,
        E_PLATFORM_WIN32,
        E_PLATFORM_OSX,
        E_PLATFORM_LINUX,
        E_PLATFORM_IOS,
        E_PLATFORM_ANDROID,
    };

    /**
     * @class IFactory
     * @brief 操作系统适配层工厂接口.
     * @note 不同操作系统平台需要实现继承该类实现具体平台接口
     */
    class IFactory
    {
        T3D_DECLARE_INTERFACE(IFactory);

    public:
        virtual IPlatform *createPlatform() = 0;
        
        /**
         * @brief 创建操作系统平台相关的应用程序对象
         * @return 返回平台相关的应用程序对象，需要用户调用delete释放对象
         */
        virtual IApplication *createPlatformApplication() = 0;

        /**
         * @brief 创建操作系统平台相关的窗口.
         * @return 返回平台相关的窗口操作对象，需要用户调用delete释放对象
         */
        virtual IWindow *createPlatformWindow() = 0;

        /**
         * @brief 创建操作系统平台相关的时间对象.
         * @return 返回平台相关的时间对象，需要用户调用delete释放对象
         */
        virtual ITime *createPlatformTime() = 0;

        /**
         * @brief 创建操作系统平台相关的定时器服务对象.
         * @return 返回平台相关的定时器服务对象，需要用户调用delete释放对象
         */
        virtual ITimerService *createPlatformTimerService() = 0;

        /**
         * @brief 创建操作系统平台相关的路径搜索对象.
         * @return 返回平台相关的路径搜索对象，需要用户调用delete释放对象
         */
        virtual IDir *createPlatformDir() = 0;

        /**
         * @brief 创建操作系统平台相关的设备信息对象.
         * @return 返回平台相关的设备信息对象，需要用户调用delete释放对象
         */
        virtual IDeviceInfo *createPlatformDeviceInfo() = 0;

        /**
         * @brief 创建操作系统平台相关的控制台对象.
         * @return 返回平台相关的控制台对象，需要用户调用delete释放对象
         */
        virtual IConsole *createPlatformConsole() = 0;

        /**
         * \brief 创建操作系统平台相关的线程对象
         * \return 返回平台相关的线程对象，需要用户调用 delete 释放对象
         */
        virtual IThread *createPlatformThread() = 0;

        virtual ICriticalSection *createPlatformCriticalSection() = 0;

        virtual IMutex *createPlatformMutex() = 0;

        virtual IRecursiveMutex *createPlatformRecursiveMutex() = 0;

        virtual ISemaphore *createPlatformSemaphore() = 0;

        virtual IEvent *createPlatformEvent() = 0;

        virtual IWaitCondition *createPlatformWaitCondition() = 0;

        virtual IProcess *createPlatformProcess() = 0;

        /**
         * @brief 获取当前平台类型.
         * @return 返回当前平台类型
         */
        virtual EPlatform getPlatform() = 0;
    };

    /**
     * @brief 创建操作系统相关的适配层工厂对象.
     * @note 不同操作系统平台需要实现该接口以返回操作系统适配层工厂对象
     * @return 返回适配层工厂对象，需要用户调用delete释放资源
     */
    IFactory *createPlatformFactory();
}


#endif  /*__T3D_FACTORY_INTERFACE_H__*/
