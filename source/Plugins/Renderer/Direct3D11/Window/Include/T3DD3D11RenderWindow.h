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


#ifndef __T3D_D3D11_RENDER_WINDOW_H__
#define __T3D_D3D11_RENDER_WINDOW_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @brief   DirectX 11 渲染窗口.
     */
    class D3D11RenderWindow : public RHIRenderWindow
    {
    public:
        /**
         * @brief 创建渲染窗口对象
         * @return 返回一个渲染窗口对象
         */
        static D3D11RenderWindowPtr create(RenderWindow *renderWindow);

        /**
         * @brief   析构函数
         */
        virtual ~D3D11RenderWindow();

        /**
         * @brief   双缓冲中交换离屏缓存到显示缓存，显示出图像
         * @return  调用成功返回 T3D_OK.
         * @remarks  具体渲染系统子类实现本接口.
         */
        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;
        
        /// The d 3D swap chain
        IDXGISwapChain          *D3DSwapChain {nullptr};
        ID3D11Texture2D         *D3DBackBuffer {nullptr};
        /// The d 3D right view
        ID3D11RenderTargetView  *D3DRTView {nullptr};
        ID3D11Texture2D         *D3DDSBuffer {nullptr};
        ///The d 3D ds view
        ID3D11DepthStencilView  *D3DDSView {nullptr};
        /// 窗口自带深度模板缓冲的像素格式，创建与 resize 时都按该格式重建
        PixelFormat             DepthStencilFormat {PixelFormat::E_PF_D24_UNORM_S8_UINT};
        /// Present 的 SyncInterval，由 RenderWindowDesc::IsVsync 决定，1 为开启垂直同步
        UINT                    PresentSyncInterval {0};
        
    protected:
        /**
         * @brief   Constructor
         */
        D3D11RenderWindow();
    };
}



#endif  /*__T3D_D3D11_RENDER_WINDOW_H__*/
