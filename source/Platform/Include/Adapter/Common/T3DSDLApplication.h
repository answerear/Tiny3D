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

#ifndef __T3D_SDL_APPLICATION_H__
#define __T3D_SDL_APPLICATION_H__

#include "Adapter/T3DApplicationInterface.h"
#include <SDL.h>

namespace Tiny3D
{
    class SDLApplication : public IApplication
    {
    public:
        SDLApplication();
        
        virtual ~SDLApplication();

        /**
         * @brief 初始化应用程序
         * @return 调用成功返回T3D_OK
         */
        TResult init() override;

        /**
         * @brief 轮询处理应用程序事件
         * @return 返回false时表示需要退出程序
         */
        bool pollEvents() override;

        /**
         * @brief 处理应用程序事件
         * @param [in] event : 事件，不同平台不一样定义，自己解析
         * @return 返回 false 时表示需要退出程序
         */
        bool processEvents(const AppEvent &event) override;

        /**
         * @brief 释放应用程序对象资源
         */
        void release() override;

        /**
         * @brief 获取平台原生应用对象
         */
        void *getNativeAppObject() override;

        void setRunInBackground(bool enable) override { mRunInBackground = enable; }

        bool isRunInBackground() const override { return mRunInBackground; }

    protected:
        enum class WindowState : uint32_t
        {
            /// 一般状态
            kNormal,
            /// 最小化
            kMinimized,
            /// 最大化
            kMaxmized,
        };
        
        /// 窗口状态
        WindowState mState {WindowState::kNormal};
        /// 能否后台运行标记
        bool mRunInBackground {false};
    };
}


#endif  /*__T3D_SDL_APPLICATION_H__*/