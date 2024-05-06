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


#ifndef __T3D_RENDER_WINDOW_H__
#define __T3D_RENDER_WINDOW_H__


#include "T3DRenderTargetDesc.h"


namespace Tiny3D
{
    /**
     * @brief 渲染窗口
     */
    class T3D_ENGINE_API RenderWindow : public Object
    {
    public:
        static RenderWindowPtr create(const String &name, const RenderWindowDesc &desc);

        /**
         * @brief 创建渲染窗口实体
         * @param [in] param : 渲染窗口需要必要参数
         * @return 调用成功返回 T3D_OK
         * @remarks 具体渲染系统子类实现本接口
         */
        TResult init(const RenderWindowDesc &param);

        virtual TResult swapBuffers();

        bool getSystemInfo(SysWMInfo &info) const;

        void *getNativeObject() const;

        const RenderWindowDesc &getDescriptor() const { return mDesc; }

        RHIRenderWindowPtr getRHIRenderWindow() const { return mRHIRenderWindow; }

        const String &getName() const { return mName; }
        
    protected:
        RenderWindow(const String &name);

        ~RenderWindow() override;

        TResult loadIcon(const String &iconPath);

        TResult destroy();
        
        /// OS 相关的窗口对象
        Window              *mWindow {nullptr};
        RenderWindowDesc    mDesc {};
        String              mName {};
        RHIRenderWindowPtr  mRHIRenderWindow {nullptr};
    };

    class T3D_ENGINE_API NullRenderWindow : public RenderWindow
    {
    public:
        static NullRenderWindowPtr create(const String &name) { return new NullRenderWindow(name); }

        /**
         * @brief 双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return 调用成功返回 T3D_OK
         * @remarks 具体渲染系统子类实现本接口
         */
        TResult swapBuffers() override { return T3D_OK; }

    protected:
        NullRenderWindow(const String &name) : RenderWindow(name) {}
    };
}


#endif  /*__T3D_RENDER_WINDOW_H__*/
