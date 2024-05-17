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
     * @brief 渲染窗口
     */
    class T3D_ENGINE_API RHIRenderWindow : public RHIRenderTarget
    {
    public:        
        ResourceType getResourceType() const override { return ResourceType::kRenderWindow; }

        /**
         * @fn  virtual TResult D3D11RenderWindow::swapBuffers() override;
         * @brief   双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return  调用成功返回 T3D_OK.
         * @remarks  具体渲染系统子类实现本接口.
         */
        virtual TResult swapBuffers() = 0;

        virtual TResult resize(uint32_t w, uint32_t h) = 0;

    protected:
        RHIRenderWindow();

        ~RHIRenderWindow() override;
    };
}


#endif  /*__T3D_RHI_RENDER_WINDOW_H__*/
