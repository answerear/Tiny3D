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
     * \brief 渲染窗口，封装 OS 窗口与 RHI 交换链
     */
    class T3D_ENGINE_API RenderWindow : public Object
    {
    public:
        /**
         * \brief 创建并初始化渲染窗口
         * \param [in] name : 窗口名称
         * \param [in] desc : 窗口创建参数
         * \return init 成功返回 RenderWindow；失败返回 nullptr
         */
        static RenderWindowPtr create(const String &name, const RenderWindowDesc &desc);

        /**
         * \brief 创建 OS 窗口与 RHI 渲染窗口
         * \param [in] param : 窗口创建参数
         * \return 调用成功返回 T3D_OK；窗口已创建或 RHI 创建失败时返回对应错误码
         */
        TResult init(const RenderWindowDesc &param);

        /**
         * \brief 交换前后缓冲，将渲染结果显示到屏幕
         * \return 调用成功返回 T3D_OK；mRHIRenderWindow 为空时返回 T3D_ERR_INVALID_POINTER
         */
        virtual TResult swapBuffers();

        /**
         * \brief 获取底层 OS 窗口的系统信息
         * \param [out] info : 输出系统窗口信息
         * \return mWindow 非空且获取成功返回 true
         */
        bool getSystemInfo(SysWMInfo &info) const;

        /**
         * \brief 返回底层 OS 原生窗口对象
         * \return mWindow 非空时返回其 native 对象；否则返回 nullptr
         */
        void *getNativeObject() const;

        /// 返回底层 OS 窗口对象；未 init 时为 nullptr
        Window *getOSWindow() const { return mWindow; }

        /// 返回窗口描述（init/resize 后 Width/Height 可能为实际尺寸）
        const RenderWindowDesc &getDescriptor() const { return mDesc; }

        /// 返回 RHI 渲染窗口对象；未 init 时为 nullptr
        RHIRenderWindow *getRHIRenderWindow() const { return mRHIRenderWindow; }

        // RHIPixelBuffer2D *getRHIDepthStencilTex() const { return mRHIDepthStencilTex; }

        /// 返回窗口名称
        const String &getName() const { return mName; }

        /**
         * \brief 调整窗口与 RHI 渲染窗口尺寸
         * \param [in] w : 新宽度
         * \param [in] h : 新高度
         * \return 调用成功返回 T3D_OK；mRHIRenderWindow 为空时返回 T3D_ERR_INVALID_POINTER
         */
        TResult resize(uint32_t w, uint32_t h);
        
    protected:
        /**
         * \brief 构造渲染窗口（仅保存名称，不创建 OS/RHI 对象）
         * \param [in] name : 窗口名称
         */
        RenderWindow(const String &name);

        ~RenderWindow() override;

        /**
         * \brief 加载并设置窗口图标（当前实现为空操作，始终返回 T3D_OK）
         * \param [in] iconPath : 图标文件路径
         * \return 调用成功返回 T3D_OK
         */
        TResult loadIcon(const String &iconPath);

        /**
         * \brief 销毁 RHI 渲染窗口与 OS 窗口
         * \return 调用成功返回 T3D_OK；mWindow 为空时返回 T3D_ERR_INVALID_POINTER
         */
        TResult destroy();
        
        /// OS 层窗口对象
        Window              *mWindow {nullptr};
        /// 窗口描述（init 后 Width/Height 可能被回写为实际尺寸）
        RenderWindowDesc    mDesc {};
        /// 窗口名称
        String              mName {};
        /// RHI 渲染窗口
        RHIRenderWindowPtr  mRHIRenderWindow {nullptr};
        // RHIPixelBuffer2DPtr mRHIDepthStencilTex {nullptr};
    };

    /**
     * \brief 空实现渲染窗口，swapBuffers 直接返回 T3D_OK（用于无显示后端场景）
     */
    class T3D_ENGINE_API NullRenderWindow : public RenderWindow
    {
    public:
        /**
         * \brief 创建 NullRenderWindow 实例
         * \param [in] name : 窗口名称
         * \return 新创建的 NullRenderWindow 指针
         */
        static NullRenderWindowPtr create(const String &name) { return T3D_NEW NullRenderWindow(name); }

        /**
         * \brief 空操作，直接返回 T3D_OK
         * \return 调用成功返回 T3D_OK
         */
        TResult swapBuffers() override { return T3D_OK; }

    protected:
        NullRenderWindow(const String &name) : RenderWindow(name) {}
    };
}


#endif  /*__T3D_RENDER_WINDOW_H__*/
