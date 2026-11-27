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

        /// 由 D3D11Context::createRenderWindow 在主线程确定采样数后写入
        void setMSAADesc(const MultisamplerDesc &desc) { mMSAA = desc; }

        /// 释放交换链缓冲上的全部引用与窗口自带的附件，ResizeBuffers 之前必须调用
        void releaseBuffers();
        
        IDXGISwapChain          *D3DSwapChain {nullptr};
        /// 交换链当前的后缓冲，Flip 模型下永远是单采样
        ID3D11Texture2D         *D3DBackBuffer {nullptr};
        /// 后缓冲的 SRV，交换链带 DXGI_USAGE_SHADER_INPUT，创建失败时为 nullptr
        ID3D11ShaderResourceView *D3DBackBufferSRView {nullptr};
        /// 窗口开 MSAA 时实际绘制的离屏颜色缓冲，Present 前 Resolve 到后缓冲；不开 MSAA 时为 nullptr
        ID3D11Texture2D         *D3DMSAAColorBuffer {nullptr};
        /// 窗口绘制用的 RTV：开 MSAA 时指向 D3DMSAAColorBuffer，否则指向后缓冲
        ID3D11RenderTargetView  *D3DRTView {nullptr};
        /// 窗口自带的深度模板缓冲，采样数与 D3DRTView 一致
        ID3D11Texture2D         *D3DDSBuffer {nullptr};
        ID3D11DepthStencilView  *D3DDSView {nullptr};
        /// 窗口自带深度模板缓冲的像素格式，创建与 resize 时都按该格式重建
        PixelFormat             DepthStencilFormat {PixelFormat::E_PF_D24_UNORM_S8_UINT};
        /// Present 的 SyncInterval，由 RenderWindowDesc::IsVsync 决定，1 为开启垂直同步
        UINT                    PresentSyncInterval {0};
        /// 创建交换链时的 Flags，ResizeBuffers 必须传同样的值
        UINT                    SwapChainFlags {0};
        /// 交换链带 DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING，关闭垂直同步且非独占全屏时 Present 带 DXGI_PRESENT_ALLOW_TEARING
        bool                    AllowTearing {false};
        
    protected:
        /**
         * @brief   Constructor
         */
        D3D11RenderWindow();
    };
}



#endif  /*__T3D_D3D11_RENDER_WINDOW_H__*/
