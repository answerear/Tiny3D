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


#ifndef __T3D_APPLICATION_H__
#define __T3D_APPLICATION_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DSingleton.h"
#include "Application/T3DAppEvents.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class Platform;

    /**
     * \brief OS 应用事件监听器。SDL 轮询在 processEvents 之前通知所有监听者。
     */
    class T3D_PLATFORM_API IAppEventListener
    {
    public:
        virtual ~IAppEventListener() = default;
        virtual void onAppEvent(const AppEvent &event) = 0;
    };

    class T3D_PLATFORM_API Application : public Allocator, public Singleton<Application>
    {
    public:
        /**
         * \brief 应用类型
         */
        enum class Type : uint32_t
        {
            /// 控制台应用
            kConsoleApp = 0,
            /// 窗口应用
            kWindowApp = 1,
        };
        
        Application();
     
        ~Application();

        virtual Type getType() const = 0;

        /**
         * @brief 应用初始化，这个由具体平台来调用，用户不需要调用
         * @return 调用成功返回T3D_OK
         */
        virtual TResult init();

        /**
         * @brief 应用事件处理
         */
        virtual bool pollEvents();

        virtual bool processEvents(const AppEvent &event);

        /**
         * @brief 应用程序退出释放资源
         * @return void
         */
        virtual void release();

        virtual void *getNativeAppObject() const;

        virtual void setRunInBackground(bool enable);

        virtual bool isRunInBackground() const;
     
        virtual TResult applicationDidFinishLaunching(int32_t argc, char* argv[]) = 0;

        virtual void applicationDidEnterBackground() = 0;

        virtual void applicationWillEnterForeground() = 0;

        virtual void applicationWillTerminate() = 0;

        virtual void applicationLowMemory() = 0;

        virtual void applicationFocusGained();

        virtual void applicationFocusLost();

        /**
         * \brief 注册 OS 事件监听器；同一指针不会重复加入
         */
        void addEventListener(IAppEventListener *listener);

        /**
         * \brief 移除 OS 事件监听器
         */
        void removeEventListener(IAppEventListener *listener);

        /**
         * \brief 向所有监听器派发事件（SDL poll 在 processEvents 之前调用）
         */
        void notifyEventListeners(const AppEvent &event);

    private:
        Platform                    *mPlatform;     /// 具体平台系统对象
        TArray<IAppEventListener*>  mEventListeners;
    };

    #define T3D_APPLICATION     (Application::getInstance())

    T3D_EXTERN_SINGLETON(Application)
}



#endif  /*__T3D_APPLICATION_H__*/
