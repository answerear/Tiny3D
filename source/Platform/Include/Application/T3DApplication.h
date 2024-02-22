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


namespace Tiny3D
{
    class Platform;

    class T3D_PLATFORM_API Application : public Singleton<Application>
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

        /**
         * @brief 应用程序退出释放资源
         * @return void
         */
        virtual void release();

        virtual void *getNativeAppObject() const;
     
        virtual bool applicationDidFinishLaunching(int32_t argc, char* argv[]) = 0;

        virtual void applicationDidEnterBackground() = 0;

        virtual void applicationWillEnterForeground() = 0;

        virtual void applicationWillTerminate() = 0;

        virtual void applicationLowMemory() = 0;

    private:
        Platform    *mPlatform;     /// 具体平台系统对象
    };

    #define T3D_APPLICATION     (Application::getInstance())
}



#endif  /*__T3D_APPLICATION_H__*/
