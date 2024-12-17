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


#ifndef __T3D_NULLD3D11_CONTEXT_H__
#define __T3D_NULLD3D11_CONTEXT_H__


#include "T3DNullD3D11Prerequisites.h"


namespace Tiny3D
{
    class NullD3D11Context : public RHIContext
    {
    public:
        static NullD3D11ContextPtr create();

        virtual ~NullD3D11Context();

        /**
         * \brief 设置视图变换矩阵和投影变换矩阵
         * \param [in] viewMat : 视图变换矩阵
         * \param [in] projMat : 投影变换矩阵
         * \return 调用成功返回 T3D_OK
         */
        TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) override;
        
        /**
         * \brief 创建 RHI 渲染窗口
         * \param [in] renderWindow : 引擎渲染窗口
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;

        /**
         * \brief 创建 RHI 渲染纹理
         * \param [in] buffer : 引擎渲染纹理
         * \return 调用成功返回新建的 RHI 对象
         */
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;
        
        /**
         * \brief 设置当前渲染窗口
         * \param [in] renderWindow : 渲染窗口
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderWindow *renderWindow) override;

        /**
         * \brief 设置当前渲染紋理
         * \param [in] renderTexture : 渲染紋理
         * \return 调用成功返回 T3D_OK
         */
        TResult setRenderTarget(RenderTexture *renderTexture) override;

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
         * \brief 编译着色器
         * \param [in,out] shader : 着色器变体对象
         * \return 调用成功返回 T3D_OK
         */
        TResult compileShader(ShaderVariant *shader) override;

        /**
         * \brief 反射着色器常量绑定信息、纹理绑定信息和纹理采样器绑定信息
         * \param [in] shader : 要反射的着色器
         * \param [out] constantParams : 绑定的常量缓冲区信息
         * \param [out] samplerParams : 绑定的纹理采样信息
         * \return 调用成功返回 T3D_OK
         */
        TResult reflectShaderAllBindings(ShaderVariantPtr shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;

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
        TResult render(uint32_t vertexCount, uint32_t startVertex) override;

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

        /**
         * \brief 写 GPU 缓冲区，在写完之前 buffer 不能释放，写完之后，内部会去释放 buffer 空间。 调用本接口，renderBuffer 必须绑定 CPUAccessWrite
         * \param [in] renderBuffer : GPU 缓冲区 
         * \param [in] buffer : CPU 的数据缓冲区
         * \param [in] discardWholeBuffer : 映射到内存的资源用于写入，之前的资源数据将会被抛弃。 该参数为 true 的时候，renderBuffer 必须绑定 Usage::kDynamic
         * \return 调用成功返回 T3D_OK
         */
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;
        
    protected:
        NullD3D11Context();

        TResult collectInformation();
    };
}


#endif  /*__T3D_NULLD3D11_CONTEXT_H__*/
