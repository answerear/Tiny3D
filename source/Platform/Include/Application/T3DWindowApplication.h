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


#ifndef __T3D_WINDOW_APPLICATION_H__
#define __T3D_WINDOW_APPLICATION_H__


#include "Application/T3DApplication.h"


namespace Tiny3D
{
    class IApplication;

    class T3D_PLATFORM_API WindowApplication : public Application
    {
    public:
        /**
         * Constructor
         */
        WindowApplication();

        /**
         * Destructor
         */
        ~WindowApplication() override;

        Type getType() const override { return Type::kWindowApp; }

        /**
         * @brief 应用初始化，这个由具体平台来调用，用户不需要调用
         * @return 调用成功返回T3D_OK
         */
        TResult init() override;

        /**
         * @brief 应用事件处理
         */
        bool pollEvents() override;

        bool processEvents(const AppEvent &event) override;

        /**
         * @brief 应用程序退出释放资源
         * @return void
         */
        void release() override;

        void *getNativeAppObject() const override;

        void setRunInBackground(bool enable) override;

        bool isRunInBackground() const override;     
    private:
        IApplication    *mApp;          /// 具体平台应用程序对象
    };

    #define T3D_APPLICATION     (Application::getInstance())
}



#endif  /*__T3D_WINDOW_APPLICATION_H__*/
