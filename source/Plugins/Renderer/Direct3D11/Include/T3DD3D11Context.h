/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_D3D11_CONTEXT_H__
#define __T3D_D3D11_CONTEXT_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    class D3D11Context : public RHIContext
    {
    public:
        static D3D11ContextPtr create();

        TResult init();

        TResult swapBackBuffer(D3D11RenderWindow *renderWindow);
        
        /**
         * \brief 创建 RHI 渲染窗口
         * \param [in] renderWindow : 引擎渲染窗口
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIRenderTargetPtr createRenderWindow(RenderWindowPtr renderWindow) override;

        /**
         * \brief 创建 RHI 渲染纹理
         * \param [in] buffer : 引擎渲染纹理
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2DPtr buffer) override;
        
        /**
         * \brief 设置当前渲染目标
         * \param [in] renderWindow : 渲染窗口
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderWindowPtr renderWindow) override;

        /**
         * \brief 设置当前渲染紋理
         * \param [in] renderTexture : 渲染紋理
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderTexturePtr renderTexture) override;

        /**
         * \brief 设置当前渲染目标
         * \param [in] renderTarget : 渲染目标
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderTargetPtr renderTarget) override;

        /**
         * \brief 清除渲染目标
         * \return 调用成功返回 T3D_OK
         */
        TResult resetRenderTarget() override;

        /**
         * \brief 设置视口
         * \param [in] viewport : 视口 
         * \return 
         */
        TResult setViewport(const Viewport &viewport) override;
        
        /**
         * \brief 用指定颜色填充渲染目标的 framebuffer
         * \param [in] color : 渲染颜色 
         * \return 调用成功返回 T3D_OK
         */
        TResult clearColor(const ColorRGB &color) override;

        /**
         * \brief 用指定深度值和模板值填充渲染目标的 depth buffer 和 stencil buffer
         * \param [in] depth : 深度值
         * \param [in] stencil : 模板值
         * \return 调用成功返回 T3D_OK
         */
        TResult clearDepthStencil(Real depth, uint32_t stencil) override;

        /**
         * \brief 创建 RHI 颜色混合状态对象
         * \param [in] state : 引擎颜色混合状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIBlendStatePtr createBlendState(BlendStatePtr state) override;

        /**
         * \brief 创建 RHI 深度模板状态对象
         * \param [in] state : 引擎深度模板状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIDepthStencilStatePtr createDepthStencilState(DepthStencilStatePtr state) override;

        /**
         * \brief 创建 RHI 光栅化状态对象
         * \param [in] state : 引擎光栅化状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIRasterizerStatePtr createRasterizerState(RasterizerStatePtr state) override;

        /**
         * \brief 创建 RHI 纹理采样状态对象
         * \param [in] state : 引擎纹理采样状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHISamplerStatePtr createSamplerState(SamplerStatePtr state) override;

        /**
         * \brief 设置颜色混合状态
         * \param [in] state : 混合状态 
         * \return 调用成功返回 T3D_OK
         */
        TResult setBlendState(BlendStatePtr state) override;

        /**
         * \brief 设置深度模板状态
         * \param [in] state : 深度模板状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setDepthStencilState(DepthStencilStatePtr state) override;

        /**
         * \brief 设置光栅化状态
         * \param [in] state : 光栅化状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setRasterizerState(RasterizerStatePtr state) override;

        /**
         * \brief 设置纹理采样状态
         * \param [in] state : 纹理采样状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setSamplerState(SamplerStatePtr state) override;

        /**
         * \brief 创建 RHI 顶点格式对象
         * \param [in] decl : 顶点格式对象
         * \return 调用成功返回 T3D_OK
         */
        RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclarationPtr decl) override;

        /**
         * \brief 设置顶点格式
         * \param [in] decl : 顶点格式
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexDeclaration(VertexDeclarationPtr decl) override;
        
        /**
         * \brief 创建 RHI 顶点缓冲区对象
         * \param [in] buffer : 引擎的顶点缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIVertexBufferPtr createVertexBuffer(VertexBufferPtr buffer) override;

        /**
         * \brief 设置渲染用的顶点缓冲对象
         * \param [in] buffer : 顶点缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexBuffer(VertexBufferPtr buffer) override;

        /**
         * \brief 创建 RHI 索引缓冲对象
         * \param [in] buffer : 引擎的索引缓冲对象
         * \return 调用成功返回 RHI 对象
         */
        RHIIndexBufferPtr createIndexBuffer(IndexBufferPtr buffer) override;

        /**
         * \brief 设置渲染用的索引缓冲对象
         * \param [in] buffer : 索引缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setIndexBuffer(IndexBufferPtr buffer) override;

        /**
         * \brief 创建 RHI 常量缓冲对象
         * \param [in] buffer : 引擎的常量缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIConstantBufferPtr createConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1DPtr buffer) override;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPixelBuffer1D(PixelBuffer1DPtr buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2DPtr buffer) override;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPixelBuffer2D(PixelBuffer2DPtr buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3DPtr buffer) override;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPixelBuffer3D(PixelBuffer3DPtr buffer) override;
        
        /**
         * \brief 创建 RHI 顶点着色器对象
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createVertexShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的顶点着色器
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 vs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setVSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 vs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setVSSampler(SamplerStatePtr sampler) override;
        
        /**
         * \brief 创建 RHI 像素着色器对象
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createPixelShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的像素着色器
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPixelShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 ps 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 ps 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPSSampler(SamplerStatePtr sampler) override;
        
        /**
         * \brief 创建 RHI 曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createHullShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setHullShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 hs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setHSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 hs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setHSSampler(SamplerStatePtr sampler) override;

        /**
         * \brief 创建 RHI 域着色器
         * \param [in] shader : 引擎使用的域着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createDomainShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的域着色器
         * \param [in] shader : 引擎使用的域着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setDomainShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 ds 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setDSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 ds 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setDSSampler(SamplerStatePtr sampler) override;
        
        /**
         * \brief 创建 RHI 几何着色器
         * \param [in] shader : 引擎使用的几何着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createGeometryShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的几何着色器
         * \param [in] shader : 引擎使用的几何着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setGeometryShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 gs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setGSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 gs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setGSSampler(SamplerStatePtr sampler) override;
        
        /**
         * \brief 创建 RHI 计算着色器
         * \param [in] shader : 引擎使用的计算着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createComputeShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置渲染使用的计算着色器
         * \param [in] shader : 引擎使用的计算着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setComputeShader(ShaderVariantPtr shader) override;

        /**
         * \brief 设置 cs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setCSConstantBuffer(ConstantBufferPtr buffer) override;

        /**
         * \brief 设置 cs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setCSSampler(SamplerStatePtr sampler) override;
        
        /**
         * \brief 根据上下文设置好的资源、状态来渲染
         * \return 调用成功返回 T3D_OK
         */
        TResult render() override;

        /**
         * \brief 清除所有状态、渲染资源，包括 RenderTarget
         * \return 调用成功返回 T3D_OK
         */
        TResult reset() override;

        /**
         * \brief 从源渲染目标传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(RenderTargetPtr src, RenderTargetPtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源纹理传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源纹理
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(TexturePtr src, RenderTargetPtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源渲染目标传输图像数据到目标纹理
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(RenderTargetPtr src, TexturePtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源纹理传输图像数据到目标纹理
         * \param [in] src : 源纹理
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(TexturePtr src, TexturePtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源缓冲区复制整个缓冲区数据到目标缓冲区
         * \param [in] src : 源渲染缓冲区
         * \param [in] dst : 目标渲染缓冲区
         * \param [in] srcOffset : 源缓冲区起始偏移
         * \param [in] size : 复制数据的大小
         * \param [in] dstOffset : 目标缓冲区起始偏移
         * \return 调用成功返回 T3D_OK
         */
        TResult copyBuffer(RenderBufferPtr src, RenderBufferPtr dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;

    protected:
        D3D11Context();

        ~D3D11Context() override;

        TResult clearColor(RenderWindowPtr window, const ColorRGB &color);

        TResult clearColor(RenderTexturePtr texture, const ColorRGB &color);

        void setupBlitQuad();

        TResult blitAll(ID3D11Resource *pD3DSrc, D3D11RenderWindow *pDst);
        
        TResult blitRegion(ID3D11ShaderResourceView *pD3DSRV, D3D11RenderWindow *pDst);

    protected:
        struct BlitVertex
        {
            Vector3 position;
            Vector2 uv;
        };
        
        /// The instance
        HINSTANCE           mInstance {nullptr};
        /// D3D11 设备对象
        ID3D11Device        *mD3DDevice {nullptr};
        /// D3D11 上下文对象
        ID3D11DeviceContext *mD3DDeviceContext {nullptr};
        /// D3D 特性级别
        D3D_FEATURE_LEVEL   mFeatureLevel {D3D_FEATURE_LEVEL_11_0};

        /// 用于 blit 的 vertex buffer
        ID3D11Buffer        *mBlitVB {nullptr};
        /// 用于 blit 的 input layout
        ID3D11InputLayout   *mBlitLayout {nullptr};
        /// 用于 blit 的 vertex shader
        ID3D11VertexShader  *mBlitVS {nullptr};
        /// 用于 blit 的 pixel shader
        ID3D11PixelShader   *mBlitPS {nullptr};

        RenderWindowPtr     mCurrentRenderWindow {nullptr};
        RenderTexturePtr    mCurrentRenderTexture {nullptr};
    };
}


#endif  /*__T3D_D3D11_CONTEXT_H__*/
