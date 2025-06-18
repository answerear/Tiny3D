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


#ifndef __T3D_RHI_CONTEXT_H__
#define __T3D_RHI_CONTEXT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderBinding.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    struct RenderWindowDesc;
    struct BlendDesc;
    struct DepthStencilDesc;
    struct RasterizerDesc;
    struct SamplerDesc;

    /**
     * @enum    TransformState
     * @brief   设置变换矩阵状态
     */
    enum class TransformState : uint32_t
    {
        kView = 0,      /**< 视口变换矩阵 */
        kWorld,         /**< 世界变换矩阵 */
        kProjection,    /**< 投影变换矩阵 */
    };

    /**
     * @class   RHIContext
     * @brief   渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API RHIContext : public Object
    {
    public:
        /**
         * @brief   析构函数
         */
        ~RHIContext() override;

        /**
         * \brief 设置视图变换矩阵和投影变换矩阵
         * \param [in] viewMat : 视图变换矩阵
         * \param [in] projMat : 投影变换矩阵
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) = 0;

        const Matrix4 &getViewMatrix() const { return mViewMatrix; }

        const Matrix4 &getProjMatrix() const { return mProjMatrix; }

        const Matrix4 &getProjViewMatrix() const { return mProjViewMatrix; }

        /**
         * \brief 创建 RHI 渲染窗口
         * \param [in] renderWindow : 引擎渲染窗口
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) = 0;

        /**
         * \brief 创建 RHI 渲染纹理
         * \param [in] buffer : 引擎渲染纹理
         * \param [in] shaderReadable : 在 shader 中是否可读
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) = 0;
        
        // /**
        //  * \brief 设置当前渲染窗口
        //  * \param [in] renderWindow : 渲染窗口
        //  * \return 调用成功返回 T3D_OK
        //  */
        // virtual TResult setRenderTarget(RenderWindow *renderWindow) = 0;
        //
        // /**
        //  * \brief 设置当前渲染紋理
        //  * \param [in] renderTexture : 渲染紋理
        //  * \return 调用成功返回 T3D_OK
        //  */
        // virtual TResult setRenderTarget(RenderTexture *renderTexture) = 0;

        /**
         * \brief 设置当前渲染目标
         * \param [in] renderTarget : 渲染目标
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRenderTarget(RenderTarget *renderTarget) = 0;

        /**
         * \brief 清除渲染目标
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult resetRenderTarget() = 0;

        /**
         * \brief 设置视口
         * \param [in] viewport : 视口 
         * \return 
         */
        virtual TResult setViewport(const Viewport &viewport) = 0;
        
        /**
         * \brief 用指定颜色填充渲染目标的 framebuffer
         * \param [in] color : 渲染颜色 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult clearColor(const ColorRGB &color) = 0;

        /**
         * @brief 用指定深度值填充渲染目标的 depth buffer
         * @param depth : 深度值
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult clearDepth(Real depth) = 0;

        /**
         * \brief 用指定深度值和模板值填充渲染目标的 depth buffer 和 stencil buffer
         * \param [in] depth : 深度值
         * \param [in] stencil : 模板值
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult clearDepthStencil(Real depth, uint32_t stencil) = 0;

        /**
         * \brief 创建 RHI 颜色混合状态对象
         * \param [in] state : 引擎颜色混合状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIBlendStatePtr createBlendState(BlendState *state) = 0;

        /**
         * \brief 创建 RHI 深度模板状态对象
         * \param [in] state : 引擎深度模板状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state) = 0;

        /**
         * \brief 创建 RHI 光栅化状态对象
         * \param [in] state : 引擎光栅化状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIRasterizerStatePtr createRasterizerState(RasterizerState *state) = 0;

        /**
         * \brief 创建 RHI 纹理采样状态对象
         * \param [in] state : 引擎纹理采样状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHISamplerStatePtr createSamplerState(SamplerState *state) = 0;

        /**
         * \brief 设置颜色混合状态
         * \param [in] state : 混合状态 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setBlendState(BlendState *state) = 0;

        /**
         * \brief 设置深度模板状态
         * \param [in] state : 深度模板状态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDepthStencilState(DepthStencilState *state) = 0;

        /**
         * \brief 设置光栅化状态
         * \param [in] state : 光栅化状态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRasterizerState(RasterizerState *state) = 0;

        /**
         * \brief 创建 RHI 顶点格式对象
         * \param [in] decl : 顶点格式对象
         * \return 调用成功返回 T3D_OK
         */
        virtual RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl) = 0;

        /**
         * \brief 设置顶点格式
         * \param [in] decl : 顶点格式
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexDeclaration(VertexDeclaration *decl) = 0;
        
        /**
         * \brief 创建 RHI 顶点缓冲区对象
         * \param [in] buffer : 引擎的顶点缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer) = 0;

        /**
         * \brief 设置渲染用的顶点缓冲对象
         * \param [in] startSlot : 缓冲区的插槽，对应 shader 寄存器索引
         * \param [in] buffers : 顶点缓冲对象
         * \param [in] strides : 顶点跨度，也就是一个顶点的大小
         * \param [in] offsets : 相对缓冲区开始的偏移
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) = 0;

        /**
         * \brief 创建 RHI 索引缓冲对象
         * \param [in] buffer : 引擎的索引缓冲对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer) = 0;

        /**
         * \brief 设置渲染用的索引缓冲对象
         * \param [in] buffer : 索引缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setIndexBuffer(IndexBuffer *buffer) = 0;

        /**
         * \brief 创建 RHI 常量缓冲对象
         * \param [in] buffer : 引擎的常量缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer) = 0;

        /**
         * \brief 创建 RHI 顶点着色器对象
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createVertexShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的顶点着色器
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 vs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 vs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;

        /**
         * \brief 设置 vs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;

        /**
         * \brief 创建 RHI 像素着色器对象
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createPixelShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的像素着色器
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPixelShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 ps 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 ps 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        
        /**
         * \brief 设置 ps 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;
        
        /**
         * \brief 创建 RHI 曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createHullShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHullShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 hs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 hs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        
        /**
         * \brief 设置 hs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;
        
        /**
         * \brief 创建 RHI 域着色器
         * \param [in] shader : 引擎使用的域着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createDomainShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的域着色器
         * \param [in] shader : 引擎使用的域着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDomainShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 ds 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 ds 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        
        /**
         * \brief 设置 ds 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;
        
        /**
         * \brief 创建 RHI 几何着色器
         * \param [in] shader : 引擎使用的几何着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createGeometryShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的几何着色器
         * \param [in] shader : 引擎使用的几何着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGeometryShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 gs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 gs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        
        /**
         * \brief 设置 gs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;
        
        /**
         * \brief 创建 RHI 计算着色器
         * \param [in] shader : 引擎使用的计算着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createComputeShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置渲染使用的计算着色器
         * \param [in] shader : 引擎使用的计算着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setComputeShader(ShaderVariant *shader) = 0;

        /**
         * \brief 设置 cs 的常量缓冲区
         * \param [in] startSlot : 常量缓冲区的插槽，对应 shader 中常量寄存器索引
         * \param [in] buffers : 常量缓冲区对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;

        /**
         * \brief 设置 cs 的像素缓冲区
         * \param [in] startSlot : 像素缓冲区的插槽，对应 shader 中像素寄存器索引
         * \param [in] buffers : 像素缓冲区对象数组
         * \return 调用成功返回 3D_OK
         */
        virtual TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        
        /**
         * \brief 设置 cs 纹理采样器
         * \param [in] startSlot : 采样器的插槽，对应 shader 中采样器寄存器索引
         * \param [in] samplers : 纹理采样器对象数组
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) = 0;

        /**
         * \brief 编译着色器
         * \param [in,out] shader : 着色器变体对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult compileShader(ShaderVariant *shader) = 0;

        /**
         * \brief 反射着色器常量绑定信息、纹理绑定信息和纹理采样器绑定信息
         * \param [in] shader : 要反射的着色器
         * \param [out] constantParams : 绑定的常量缓冲区信息
         * \param [out] samplerParams : 绑定的纹理采样信息
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) = 0;

        /**
         * \brief 设置渲染图元类型
         * \param [in] primitive : 图元类型
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPrimitiveType(PrimitiveType primitive) = 0;

        /**
         * \brief 根据上下文设置好的资源、状态来渲染，带顶点索引的绘制
         * \param [in] indexCount : 索引数量
         * \param [in] startIndex : 索引缓冲区起始位置
         * \param [in] baseVertex : 用于索引的基础顶点位置。这个值会被加到索引值上，然后再用来索引顶点缓冲区
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) = 0;

        /**
         * \brief 根据上下文设置好的资源、状态来渲染，不带顶点索引的绘制
         * \param [in] vertexCount : 顶点数量
         * \param [in] startVertex : 顶点缓冲区其实位置
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(uint32_t vertexCount, uint32_t startVertex) = 0;

        /**
         * \brief 清除所有状态、渲染资源，包括 RenderTarget
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reset() = 0;

        /**
         * \brief 从源渲染目标传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源纹理传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源纹理
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源渲染目标传输图像数据到目标纹理
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源纹理传输图像数据到目标纹理
         * \param [in] src : 源纹理
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源缓冲区复制整个缓冲区数据到目标缓冲区
         * \param [in] src : 源渲染缓冲区
         * \param [in] dst : 目标渲染缓冲区
         * \param [in] srcOffset : 源缓冲区起始偏移
         * \param [in] size : 复制数据的大小
         * \param [in] dstOffset : 目标缓冲区起始偏移
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) = 0;

        /**
         * \brief 写 GPU 缓冲区，在写完之前 buffer 不能释放，写完之后，内部会去释放 buffer 空间。 调用本接口，renderBuffer 必须绑定 CPUAccessWrite
         * \param [in] renderBuffer : GPU 缓冲区 
         * \param [in] buffer : CPU 的数据缓冲区
         * \param [in] discardWholeBuffer : 映射到内存的资源用于写入，之前的资源数据将会被抛弃。 该参数为 true 的时候，renderBuffer 必须绑定 Usage::kDynamic
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) = 0;

    protected:
        RHIContext();

    protected:
        /// 投影变换矩阵
        Matrix4 mProjMatrix {false};
        /// 视口变换矩阵
        Matrix4 mViewMatrix {false};
        /// 投影和视口变换的联合变换
        Matrix4 mProjViewMatrix {false};
    };
}


#include "T3DRHIContext.inl"


#endif  /*__T3D_RHI_CONTEXT_H__*/
