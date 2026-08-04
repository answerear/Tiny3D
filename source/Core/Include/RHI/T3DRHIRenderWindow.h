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


#ifndef __T3D_RHI_RENDER_WINDOW_H__
#define __T3D_RHI_RENDER_WINDOW_H__


#include "RHI/T3DRHIRenderTarget.h"


namespace Tiny3D
{
    /**
     * \brief RHI 层渲染窗口，封装交换链及与系统窗口绑定的渲染目标
     */
    class T3D_ENGINE_API RHIRenderWindow : public RHIRenderTarget
    {
    public:
        /**
         * \brief 返回资源类型 kRenderWindow
         * \return ResourceType::kRenderWindow
         */
        ResourceType getResourceType() const override { return ResourceType::kRenderWindow; }

        /**
         * \brief 交换前后缓冲，将离屏内容呈现到屏幕
         * \return 调用成功返回 T3D_OK；由具体后端实现（如 SwapBuffers、vkQueuePresent 等）
         */
        virtual TResult swapBuffers() = 0;

        /**
         * \brief 调整渲染窗口/交换链尺寸
         * \param [in] w : 新宽度（像素）
         * \param [in] h : 新高度（像素）
         * \return 调用成功返回 T3D_OK；由具体后端实现
         */
        virtual TResult resize(uint32_t w, uint32_t h) = 0;

    protected:
        /// 受保护构造，由 RHIContext::createRenderWindow 及后端派生类创建
        RHIRenderWindow();

        /// 析构
        ~RHIRenderWindow() override;
    };
}


#endif  /*__T3D_RHI_RENDER_WINDOW_H__*/
