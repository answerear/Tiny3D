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


#ifndef __T3D_D3D11_CONTEXT_H__
#define __T3D_D3D11_CONTEXT_H__


#include "T3DD3D11Prerequisites.h"
#include "T3DD3D11ContextBase.h"


namespace Tiny3D
{
    class D3D11Context : public D3D11ContextBase
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
        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;
        
        /**
         * \brief 创建 RHI 渲染纹理
         * \param [in] buffer : 引擎渲染纹理
         * \param [in] shaderReadable : 在 shader 中是否可读
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;

        /**
         * 改变渲染窗口大小
         * @param [in] rw : 要改变大小的渲染窗口对象
         * @param [in] w : 要改变的宽度
         * @param [in] h : 要改变的高度 
         * @return 调用成功返回 T3D_OK
         */
        TResult resizeRenderWindow(D3D11RenderWindow *rw, uint32_t w, uint32_t h);

        /**
         * 改变渲染纹理大小
         * @param [in] rt : 要改变大小的渲染纹理对象
         * @param [in] w : 要改变的宽度
         * @param [in] h : 要改变的高度
         * @return 调用成功返回 T3D_OK
         */
        TResult resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h) override;

        /**
         * 改变渲染目标大小
         * @param [in] rt : 要改变大小的渲染目标
         * @param [in] w : 要改变的宽度 
         * @param [in] h : 要改变的高度
         * @return 调用成功返回 T3D_OK
         */
        TResult resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h) override;

        /**
         * \brief 设置当前渲染目标
         * \param [in] renderTarget : 渲染目标
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderTarget *renderTarget) override;

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
         * \brief 设置裁剪矩形（像素坐标，原点左上，Y 向下）
         */
        TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) override;
        
        /**
         * \brief 用指定颜色填充渲染目标的 framebuffer
         * \param [in] color : 渲染颜色 
         * \return 调用成功返回 T3D_OK
         */
        TResult clearColor(const ColorRGB &color) override;

        /**
         * @brief 用指定深度值填充渲染目标的 depth buffer
         * @param depth : 深度值
         * @return 调用成功返回 T3D_OK
         */
        TResult clearDepth(Real depth) override;

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
        RHIBlendStatePtr createBlendState(BlendState *state) override;

        /**
         * \brief 创建 RHI 深度模板状态对象
         * \param [in] state : 引擎深度模板状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state) override;

        /**
         * \brief 创建 RHI 光栅化状态对象
         * \param [in] state : 引擎光栅化状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIRasterizerStatePtr createRasterizerState(RasterizerState *state) override;

        /**
         * \brief 创建 RHI 纹理采样状态对象
         * \param [in] state : 引擎纹理采样状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        RHISamplerStatePtr createSamplerState(SamplerState *state) override;

        /**
         * \brief 设置颜色混合状态
         * \param [in] state : 混合状态 
         * \return 调用成功返回 T3D_OK
         */
        TResult setBlendState(BlendState *state) override;

        /**
         * \brief 设置深度模板状态
         * \param [in] state : 深度模板状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setDepthStencilState(DepthStencilState *state) override;

        /**
         * \brief 设置光栅化状态
         * \param [in] state : 光栅化状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setRasterizerState(RasterizerState *state) override;
        
        /**
         * \brief 创建 RHI 顶点格式对象
         * \param [in] decl : 顶点格式对象
         * \return 调用成功返回 T3D_OK
         */
        RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl) override;

        /**
         * \brief 设置顶点格式
         * \param [in] decl : 顶点格式
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexDeclaration(VertexDeclaration *decl) override;
        
        /**
         * \brief 创建 RHI 顶点缓冲区对象
         * \param [in] buffer : 引擎的顶点缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer) override;

        /**
         * \brief 设置渲染用的顶点缓冲对象
         * \param [in] startSlot : 缓冲区的插槽，对应 shader 寄存器索引
         * \param [in] buffers : 顶点缓冲对象
         * \param [in] strides : 顶点跨度，也就是一个顶点的大小
         * \param [in] offsets : 相对缓冲区开始的偏移
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) override;

        /**
         * \brief 创建 RHI 索引缓冲对象
         * \param [in] buffer : 引擎的索引缓冲对象
         * \return 调用成功返回 RHI 对象
         */
        RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer) override;

        /**
         * \brief 设置渲染用的索引缓冲对象
         * \param [in] buffer : 索引缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setIndexBuffer(IndexBuffer *buffer) override;

        /**
         * \brief 创建 RHI 常量缓冲对象
         * \param [in] buffer : 引擎的常量缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer) override;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer) override;

        /**
         * \brief 创建 RHI 立方体贴图像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer) override;
        
        /**
         * \brief 创建 RHI 顶点着色器对象
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的顶点着色器
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setVertexShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 vs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 vs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 vs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        
        /**
         * \brief 创建 RHI 像素着色器对象
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的像素着色器
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setPixelShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 ps 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 ps 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 ps 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        
        /**
         * \brief 创建 RHI 曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 T3D_OK
         */
        TResult setHullShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 hs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 hs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 hs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        
        /**
         * \brief 创建 RHI 域着色器
         * \param [in] shader : 引擎使用的域着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的域着色器
         * \param [in] shader : 引擎使用的域着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setDomainShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 ds 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 ds 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 ds 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        
        /**
         * \brief 创建 RHI 几何着色器
         * \param [in] shader : 引擎使用的几何着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的几何着色器
         * \param [in] shader : 引擎使用的几何着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setGeometryShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 gs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 gs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 gs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        
        /**
         * \brief 创建 RHI 计算着色器
         * \param [in] shader : 引擎使用的计算着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;

        /**
         * \brief 设置渲染使用的计算着色器
         * \param [in] shader : 引擎使用的计算着色器
         * \return 调用成功返回 T3D_OK
         */
        TResult setComputeShader(ShaderVariant *shader) override;

        /**
         * \brief 设置 cs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;

        /**
         * \brief 设置 cs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;

        /**
         * \brief 设置 cs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) override;

        /**
         * \brief 设置渲染图元类型
         * \param [in] primitive : 图元类型
         * \return 调用成功返回 T3D_OK
         */
        TResult setPrimitiveType(PrimitiveType primitive) override;
        
        /**
         * \brief 根据上下文设置好的资源、状态来渲染，带顶点索引的绘制
         * \param [in] indexCount : 索引数量
         * \param [in] startIndex : 索引缓冲区起始位置
         * \param [in] baseVertex : 用于索引的基础顶点位置。这个值会被加到索引值上，然后再用来索引顶点缓冲区
         * \return 调用成功返回 T3D_OK
         */
        TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override;

        /**
         * \brief 根据上下文设置好的资源、状态来渲染，不带顶点索引的绘制
         * \param [in] vertexCount : 顶点数量
         * \param [in] startVertex : 顶点缓冲区其实位置
         * \return 调用成功返回 T3D_OK
         */
        TResult render( uint32_t vertexCount, uint32_t startVertex) override;

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
        TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源纹理传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源纹理
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源渲染目标传输图像数据到目标纹理
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源纹理传输图像数据到目标纹理
         * \param [in] src : 源纹理
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;

        /**
         * \brief 从源缓冲区复制整个缓冲区数据到目标缓冲区
         * \param [in] src : 源渲染缓冲区
         * \param [in] dst : 目标渲染缓冲区
         * \param [in] srcOffset : 源缓冲区起始偏移
         * \param [in] size : 复制数据的大小
         * \param [in] dstOffset : 目标缓冲区起始偏移
         * \return 调用成功返回 T3D_OK
         */
        TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;

        /**
         * \brief 写 GPU 缓冲区，在写完之前 buffer 不能释放，写完之后，内部会去释放 buffer 空间。 调用本接口，renderBuffer 必须绑定 CPUAccessWrite
         * \param [in] renderBuffer : GPU 缓冲区 
         * \param [in] buffer : CPU 的数据缓冲区
         * \param [in] discardWholeBuffer : 映射到内存的资源用于写入，之前的资源数据将会被抛弃。 该参数为 true 的时候，renderBuffer 必须绑定 Usage::kDynamic
         * \return 调用成功返回 T3D_OK
         */
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;

        TResult beginRender() override { return T3D_OK; }
        TResult endRender() override { return T3D_OK; }

        TResult beginPass() override { return T3D_OK; }
        TResult endPass() override { return T3D_OK; }

        ID3D11Device *getD3DDevice() const { return mD3DDevice; }

        ID3D11DeviceContext *getD3DDeviceContext() const { return mD3DDeviceContext; }
        
    protected:
        D3D11Context();

        ~D3D11Context() override;

        void traceDebugInfo(const String &tag = "", const String &func = "");
        
        TResult clearColor(RenderWindow *window, const ColorRGB &color);

        TResult clearColor(const RenderTexturePtr *textures, uint32_t numOfTextures, const ColorRGB &color);

        TResult clearDepthStencil(RenderTexture *texture, const Real &depth, uint8_t stencil);

        /**
         * \brief 清除当前渲染目标的深度模板附件，clearDepth 与 clearDepthStencil 共用
         * \param [in] clearFlags : D3D11_CLEAR_DEPTH / D3D11_CLEAR_STENCIL 的组合
         * \param [in] depth : 深度清除值
         * \param [in] stencil : 模板清除值
         * \return 调用成功返回 T3D_OK；当前渲染目标没有任何深度模板附件时也返回 T3D_OK
         */
        TResult clearDepthStencilView(uint32_t clearFlags, Real depth, uint8_t stencil);

        void setupBlitQuad();

        /**
         * \brief 用全屏四边形把源纹理的一块矩形区域绘制到目标 RTV 的一块矩形区域
         * \param [in] pD3DSRV : 源纹理的着色器资源视图
         * \param [in] pD3DRTView : 目标渲染目标视图
         * \param [in] pD3DDSView : 目标深度模板视图，可为 nullptr
         * \param [in] srcWidth : 源纹理完整宽度，用于把 srcOffset / srcSize 归一化成 UV
         * \param [in] srcHeight : 源纹理完整高度
         * \param [in] srcOffset : 源矩形左上角（像素）
         * \param [in] srcSize : 源矩形尺寸（像素）
         * \param [in] dstOffset : 目标矩形左上角（像素）
         * \param [in] dstSize : 目标矩形尺寸（像素），与 srcSize 不等时发生缩放
         * \return 调用成功返回 T3D_OK
         */
        TResult blitRegion(ID3D11ShaderResourceView *pD3DSRV, ID3D11RenderTargetView *pD3DRTView, ID3D11DepthStencilView *pD3DDSView, uint32_t srcWidth, uint32_t srcHeight, const Vector3 &srcOffset, const Vector3 &srcSize, const Vector3 &dstOffset, const Vector3 &dstSize);

        /**
         * \brief 把扁平像素数据按 [arraySlice][mipLevel] 顺序切分为 D3D11 子资源数组
         * \param [in] data : CPU 数据首地址，可为 nullptr（表示不上传初始数据）
         * \param [in] dataSize : CPU 数据字节数，用于越界校验
         * \param [in] format : 像素格式，用于计算每像素字节数
         * \param [in] width : 顶层宽度
         * \param [in] height : 顶层高度，1D 纹理传 1
         * \param [in] depth : 顶层深度，非 3D 纹理传 1
         * \param [in] mipLevels : mip 层数，至少 1
         * \param [in] arraySize : 数组层数（Cubemap 传 6 * 立方体个数），至少 1
         * \param [out] outSubresources : 输出的子资源描述数组，data 为空时返回空数组
         * \return 像素格式未登记或数据不足以覆盖全部子资源时返回 T3D_ERR_INVALID_PARAM
         */
        TResult buildSubresourceData(const uint8_t *data, size_t dataSize, PixelFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t arraySize, TArray<D3D11_SUBRESOURCE_DATA> &outSubresources);

        /**
         * \brief blit 操作的一个端点（源或目标）解析结果
         */
        struct BlitEndpoint
        {
            /// 用于 CopyResource / CopySubresourceRegion 的资源
            ID3D11Resource              *Resource {nullptr};
            /// 用于全屏四边形路径的源视图；不可采样时为 nullptr
            ID3D11ShaderResourceView    *SRView {nullptr};
            /// 用于全屏四边形路径的目标视图；不可作为 RT 时为 nullptr
            ID3D11RenderTargetView      *RTView {nullptr};
            /// 目标深度模板视图，可为 nullptr
            ID3D11DepthStencilView      *DSView {nullptr};
            /// 资源宽度
            uint32_t                     Width {0};
            /// 资源高度
            uint32_t                     Height {0};
            /// 资源深度，非 3D 资源为 1
            uint32_t                     Depth {1};
            /// 资源的 DXGI 格式
            DXGI_FORMAT                  Format {DXGI_FORMAT_UNKNOWN};
            /// 资源的 MSAA 采样数
            uint32_t                     SampleCount {1};
        };

        /**
         * \brief 用底层资源自身的 GetDesc 填充端点的尺寸、格式与采样数
         * \param [in] resource : 已创建的 D3D11 纹理资源，为 nullptr 时不做任何事
         * \param [in,out] out : 待填充的端点，视图成员保持不变
         * \note 直接问 D3D11 而不是读引擎描述，能正确覆盖 MSAA Resolve 副本、BackBuffer 等场景
         */
        void describeD3DResource(ID3D11Resource *resource, BlitEndpoint &out);

        /**
         * \brief 从纹理解析出 blit 端点
         * \param [in] tex : 源或目标纹理
         * \param [in] asSource : true 表示作为拷贝源，MSAA 的 RenderTexture 会先做一次 Resolve
         * \param [out] out : 解析结果
         * \return 纹理类型不支持或底层资源未创建时返回对应错误码
         */
        TResult resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out);

        /**
         * \brief 从渲染目标解析出 blit 端点，纹理型只处理 attachment 0
         * \param [in] rt : 源或目标渲染目标
         * \param [in] asSource : true 表示作为拷贝源
         * \param [out] out : 解析结果
         * \return 底层资源未创建时返回对应错误码
         */
        TResult resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out);

        /**
         * \brief 判断两个端点能否直接用 D3D11 的拷贝 API 传输
         * \param [in] src : 源端点
         * \param [in] dst : 目标端点
         * \param [in] regionCopy : true 判断 CopySubresourceRegion，false 判断 CopyResource
         * \return 兼容返回 true
         */
        bool isDirectCopyCompatible(const BlitEndpoint &src, const BlitEndpoint &dst, bool regionCopy) const;

        /**
         * \brief 在两个已解析端点之间执行传输，按兼容性依次尝试直接拷贝、MSAA Resolve、全屏四边形绘制
         * \param [in] src : 源端点
         * \param [in] dst : 目标端点
         * \param [in] srcOffset : 源偏移
         * \param [in] size : 传输尺寸，Vector3::ZERO 表示整资源
         * \param [in] dstOffset : 目标偏移
         * \return 无可用路径时返回 T3D_ERR_D3D11_INCOMPATIBLE_COPY
         */
        TResult doBlit(const BlitEndpoint &src, const BlitEndpoint &dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset);

        /**
         * \brief 从渲染缓冲区取出底层 D3D11 资源
         * \param [in] buffer : 渲染缓冲区
         * \return 类型不支持或 RHI 资源未创建时返回 nullptr
         */
        ID3D11Resource *getD3DResource(RenderBuffer *buffer);

        /**
         * \brief 按描述为 d3dBuffer 创建纹理与视图，createRenderTexture 与 resizeRenderTexture 共用
         * \param [in] buffer : 引擎侧像素缓冲，提供描述与 Usage
         * \param [in] d3dBuffer : 待填充的 RHI 对象，调用前其 COM 成员必须为 nullptr
         * \return 调用成功返回 T3D_OK
         */
        TResult buildRenderTextureResources(PixelBuffer2D *buffer, D3D11PixelBuffer2D *d3dBuffer);

        /**
         * \brief 释放 d3dBuffer 上的全部 COM 对象，并先解绑管线上可能残留的引用
         * \param [in] d3dBuffer : 待清空的 RHI 对象
         */
        void releaseRenderTextureResources(D3D11PixelBuffer2D *d3dBuffer);

        using SetSamplerState = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11SamplerState * const *);
        
        TResult setSamplers(SetSamplerState setSamplerState, uint32_t startSlot, const Samplers &samplers);

        using SetShaderResources = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11ShaderResourceView * const *);
        
        TResult setPixelBuffers(SetShaderResources setShaderResources, uint32_t startSlot, const PixelBuffers &buffers);

        using SetConstantBuffers = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer * const *);
        
        TResult setConstantBuffers(SetConstantBuffers setConstantBuffers, uint32_t startSlot, const ConstantBuffers &buffers);

        TResult createRenderWindow(D3D11RenderWindow *pD3DRenderWindow, uint32_t w, uint32_t h, uint32_t MSAACount, uint32_t MSAAQuality);

        TResult setRenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil);

        TResult setRenderTarget(const RenderTexturePtr *renderTexture, uint32_t numOfTextures, RenderTexture *depthStencil);

        void backupRenderState();
        
    protected:
        /**
         * \brief 用于 blit 的 quad 顶点结构
         */
        struct BlitVertex
        {
            Vector3 position;
            Vector2 uv;
        };

        struct BackUpDX11State
        {
            ID3D11RenderTargetView      *RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] {nullptr};
            ID3D11DepthStencilView      *DepthStencilView {nullptr};

            UINT                        ScissorRectsCount {0}, ViewportsCount {0};
            D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
            D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

            ID3D11RasterizerState       *RasterizerState {nullptr};
            ID3D11BlendState            *BlendState {nullptr};
            FLOAT                       BlendFactor[4] {0.0f};
            UINT                        SampleMask {0};
            UINT                        StencilRef {0};
            ID3D11DepthStencilState     *DepthStencilState {nullptr};
            
            ID3D11ShaderResourceView    *VSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            ID3D11ShaderResourceView    *PSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            ID3D11ShaderResourceView    *GSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            ID3D11ShaderResourceView    *HSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            ID3D11ShaderResourceView    *DSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            ID3D11ShaderResourceView    *CSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] {nullptr};

            ID3D11SamplerState          *VSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            ID3D11SamplerState          *PSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            ID3D11SamplerState          *GSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            ID3D11SamplerState          *HSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            ID3D11SamplerState          *DSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            ID3D11SamplerState          *CSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] {nullptr};
            
            ID3D11PixelShader           *PS {nullptr};
            ID3D11VertexShader          *VS {nullptr};
            ID3D11GeometryShader        *GS {nullptr};
            ID3D11DomainShader          *DS {nullptr};
            ID3D11HullShader            *HS {nullptr};
            ID3D11ComputeShader         *CS {nullptr};
            
            UINT                        PSInstancesCount {0}, VSInstancesCount {0}, GSInstancesCount {0}, HSInstancesCount {0}, DSInstancesCount {0}, CSInstancesCount {0};
            
            ID3D11ClassInstance         *PSInstances[256] {nullptr};
            ID3D11ClassInstance         *VSInstances[256] {nullptr};
            ID3D11ClassInstance         *GSInstances[256] {nullptr};
            ID3D11ClassInstance         *HSInstances[256] {nullptr};
            ID3D11ClassInstance         *DSInstances[256] {nullptr};
            ID3D11ClassInstance         *CSInstances[256] {nullptr};   // 256 is max according to PSSetShader documentation
            
            D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology {D3D_PRIMITIVE_TOPOLOGY_UNDEFINED};
            ID3D11Buffer                *IndexBuffer {nullptr};
            ID3D11Buffer                *VertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] {nullptr};
            UINT                        VertexBufferStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] {0};
            UINT                        VertexBufferOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] {0};

            ID3D11Buffer                *VSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            ID3D11Buffer                *PSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            ID3D11Buffer                *GSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            ID3D11Buffer                *HSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            ID3D11Buffer                *DSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            ID3D11Buffer                *CSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
            
            UINT                        IndexBufferOffset {0};
            DXGI_FORMAT                 IndexBufferFormat {DXGI_FORMAT_UNKNOWN};
            ID3D11InputLayout           *InputLayout {nullptr};
        };
        
        /// The instance
        HINSTANCE           mInstance {nullptr};
        /// D3D11 设备对象
        ID3D11Device        *mD3DDevice {nullptr};
        /// 用于调试的设备对象
#if defined (T3D_DEBUG)
        ID3D11Debug         *mDebugDevice {nullptr};
#endif
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
        /// 用于 blit 的 sampler state
        ID3D11SamplerState      *mBlitSamplerState {nullptr};
        /// 用于 blit 的 blend state
        ID3D11BlendState        *mBlitBState {nullptr};
        /// 用于 blit 的 depth stencil state
        ID3D11DepthStencilState *mBlitDSState {nullptr};
        /// 用于 blit 的 rasterizer state
        ID3D11RasterizerState   *mBlitRState {nullptr};

        /// 当前渲染目标，调用 SetRenderTarget 设置进来
        RenderTargetPtr     mCurrentRenderTarget {nullptr};

        BackUpDX11State     mBackupState {};
    };
}


#endif  /*__T3D_D3D11_CONTEXT_H__*/
