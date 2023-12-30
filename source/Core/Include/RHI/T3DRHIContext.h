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


#ifndef __T3D_RHI_CONTEXT_H__
#define __T3D_RHI_CONTEXT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"


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
     * @enum    PrimitiveType
     * @brief   渲染图元类型
     */
    enum class PrimitiveType : uint32_t
    {
        kPointList = 0,     /**< 点列表图元 */
        kLineList,          /**< 线列表图元 */
        kLineStrip,         /**< 线带图元 */
        kTriangleList,      /**< 三角形列表图元 */
        kTriangleStrip,     /**< 三角形带图元 */
        kTriangleFan,       /**< 三角形扇形图元 */
    };

    /**
     * @enum    ClearFlags
     * @brief   Values that represent clear flags
     */
    enum ClearFlags : uint32_t
    {
        kClearStencil = 0,
        kClearTarget,
        kClearZBuffer,
    };

    /**
     * \brief 着色器变量绑定信息
     */
    struct ShaderVariableBinding
    {
        /// 名称
        String      name;
        /// 变量在缓冲区中的偏移
        uint32_t    offset;
        /// 变量大小
        uint32_t    size;
    };

    using ShaderVariableBindings = TMap<String, ShaderVariableBinding>;

    /**
     * \brief 着色器常量绑定信息
     */
    struct ShaderConstantBinding
    {
        /// 名称
        String      name {};
        /// 绑定点
        uint32_t    binding {0};
        /// 缓冲区大小
        uint32_t    size {0};
        /// 常量缓冲区包含的变量列表
        ShaderVariableBindings  variables;
    };

    /**
     * \brief 着色器纹理绑定信息
     */
    struct ShaderTextureBinding
    {
        /// 名称
        String          name {};
        /// 绑定点
        uint32_t        binding {0};
        /// 绑定数量，用于纹理数组
        uint32_t        bindingCount {0};
        /// 纹理类型
        TEXTURE_TYPE    texType {TEXTURE_TYPE::TT_2D};
    };

    /**
     * \brief 着色器纹理采样器绑定信息
     */
    struct ShaderSamplerBinding
    {
        /// 名称
        String      name {};
        /// 绑定点
        uint32_t    binding {0};
    };

    struct ShaderTexSamplerBinding
    {
        ShaderTextureBinding    texBinding {};
        ShaderSamplerBinding    samplerBinding {};
    };

    using ShaderConstantBindings = TMap<String, ShaderConstantBinding>;
    using ShaderTexSamplerBindings = TMap<String, ShaderTexSamplerBinding>;
    
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

        // virtual TResult init() = 0;
        //
        // virtual TResult renderAllTargets();
        //
        // virtual TResult attachRenderTarget(RenderTargetPtr target);
        //
        // virtual TResult detachRenderTarget(const String &name);
        //
        // RenderTargetPtr getRenderTarget(const String &name);

        virtual TResult setTransform(TransformState state, const Matrix4 &mat);

        TResult setWorldTransform(const Matrix4 &mat);
        
        TResult setViewTransform(const Matrix4 &mat);

        TResult setProjectionTransform(const Matrix4 &mat);

        virtual const Matrix4 &getTransform(TransformState state) const;

        // virtual Matrix4 makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const = 0;
        //
        // virtual Matrix4 makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist) = 0;
        //
        // virtual Matrix4 makeViewportMatrix(Viewport *viewport) = 0;
        //
        // virtual RHIRenderWindowPtr createRenderWindow(RenderWindowPtr renderWindow, const RenderWindowDesc &param) = 0;

        /**
         * \brief 创建 RHI 渲染窗口
         * \param [in] renderWindow : 引擎渲染窗口
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIRenderTargetPtr createRenderWindow(RenderWindowPtr renderWindow) = 0;

        /**
         * \brief 创建 RHI 渲染纹理
         * \param [in] buffer : 引擎渲染纹理
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2DPtr buffer) = 0;
        
        /**
         * \brief 设置当前渲染窗口
         * \param [in] renderWindow : 渲染窗口
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRenderTarget(RenderWindowPtr renderWindow) = 0;

        /**
         * \brief 设置当前渲染紋理
         * \param [in] renderTexture : 渲染紋理
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRenderTarget(RenderTexturePtr renderTexture) = 0;

        /**
         * \brief 设置当前渲染目标
         * \param [in] renderTarget : 渲染目标
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRenderTarget(RenderTargetPtr renderTarget) = 0;

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
        virtual RHIBlendStatePtr createBlendState(BlendStatePtr state) = 0;

        /**
         * \brief 创建 RHI 深度模板状态对象
         * \param [in] state : 引擎深度模板状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIDepthStencilStatePtr createDepthStencilState(DepthStencilStatePtr state) = 0;

        /**
         * \brief 创建 RHI 光栅化状态对象
         * \param [in] state : 引擎光栅化状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHIRasterizerStatePtr createRasterizerState(RasterizerStatePtr state) = 0;

        /**
         * \brief 创建 RHI 纹理采样状态对象
         * \param [in] state : 引擎纹理采样状态对象
         * \return 调用成功返回新建的 RHI 对象
         */
        virtual RHISamplerStatePtr createSamplerState(SamplerStatePtr state) = 0;

        /**
         * \brief 设置颜色混合状态
         * \param [in] state : 混合状态 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setBlendState(BlendStatePtr state) = 0;

        /**
         * \brief 设置深度模板状态
         * \param [in] state : 深度模板状态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDepthStencilState(DepthStencilStatePtr state) = 0;

        /**
         * \brief 设置光栅化状态
         * \param [in] state : 光栅化状态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRasterizerState(RasterizerStatePtr state) = 0;

        /**
         * \brief 设置纹理采样状态
         * \param [in] state : 纹理采样状态
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setSamplerState(SamplerStatePtr state) = 0;

        /**
         * \brief 创建 RHI 顶点格式对象
         * \param [in] decl : 顶点格式对象
         * \return 调用成功返回 T3D_OK
         */
        virtual RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclarationPtr decl) = 0;

        /**
         * \brief 设置顶点格式
         * \param [in] decl : 顶点格式
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexDeclaration(VertexDeclarationPtr decl) = 0;
        
        /**
         * \brief 创建 RHI 顶点缓冲区对象
         * \param [in] buffer : 引擎的顶点缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIVertexBufferPtr createVertexBuffer(VertexBufferPtr buffer) = 0;

        /**
         * \brief 设置渲染用的顶点缓冲对象
         * \param [in] buffer : 顶点缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexBuffer(VertexBufferPtr buffer) = 0;

        /**
         * \brief 创建 RHI 索引缓冲对象
         * \param [in] buffer : 引擎的索引缓冲对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIIndexBufferPtr createIndexBuffer(IndexBufferPtr buffer) = 0;

        /**
         * \brief 设置渲染用的索引缓冲对象
         * \param [in] buffer : 索引缓冲对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setIndexBuffer(IndexBufferPtr buffer) = 0;

        /**
         * \brief 创建 RHI 常量缓冲对象
         * \param [in] buffer : 引擎的常量缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIConstantBufferPtr createConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1DPtr buffer) = 0;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPixelBuffer1D(PixelBuffer1DPtr buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2DPtr buffer) = 0;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPixelBuffer2D(PixelBuffer2DPtr buffer) = 0;

        /**
         * \brief 创建 RHI 像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3DPtr buffer) = 0;

        /**
         * \brief 设置渲染用的像素缓冲区对象
         * \param [in] buffer : 引擎的像素缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPixelBuffer3D(PixelBuffer3DPtr buffer) = 0; 

        /**
         * \brief 创建 RHI 顶点着色器对象
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createVertexShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的顶点着色器
         * \param [in] shader : 引擎使用的顶点着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 vs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 vs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setVSSampler(SamplerStatePtr sampler) = 0;

        /**
         * \brief 创建 RHI 像素着色器对象
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createPixelShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的像素着色器
         * \param [in] shader : 引擎使用的像素着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPixelShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 ps 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 ps 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPSSampler(SamplerStatePtr sampler) = 0;
        
        /**
         * \brief 创建 RHI 曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createHullShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的曲面细分着色器
         * \param [in] shader : 引擎使用的曲面细分着色器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHullShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 hs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 hs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setHSSampler(SamplerStatePtr sampler) = 0;
        
        /**
         * \brief 创建 RHI 域着色器
         * \param [in] shader : 引擎使用的域着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createDomainShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的域着色器
         * \param [in] shader : 引擎使用的域着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDomainShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 ds 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 ds 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setDSSampler(SamplerStatePtr sampler) = 0;
        
        /**
         * \brief 创建 RHI 几何着色器
         * \param [in] shader : 引擎使用的几何着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createGeometryShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的几何着色器
         * \param [in] shader : 引擎使用的几何着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGeometryShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 gs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 gs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setGSSampler(SamplerStatePtr sampler) = 0;
        
        /**
         * \brief 创建 RHI 计算着色器
         * \param [in] shader : 引擎使用的计算着色器对象 
         * \return 调用成功返回 RHI 对象
         */
        virtual RHIShaderPtr createComputeShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置渲染使用的计算着色器
         * \param [in] shader : 引擎使用的计算着色器
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setComputeShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 设置 cs 的常量缓冲区
         * \param [in] buffer : 常量缓冲区对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setCSConstantBuffer(ConstantBufferPtr buffer) = 0;

        /**
         * \brief 设置 cs 纹理采样器
         * \param [in] sampler : 纹理采样器对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setCSSampler(SamplerStatePtr sampler) = 0;

        /**
         * \brief 编译着色器
         * \param [in,out] shader : 着色器变体对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult compileShader(ShaderVariantPtr shader) = 0;

        /**
         * \brief 反射着色器常量绑定信息、纹理绑定信息和纹理采样器绑定信息
         * \param [in] shader : 要反射的着色器
         * \param [out] constantBindings : 绑定的常量缓冲区信息
         * \param [out] texSamplerBindings : 绑定的纹理采样信息
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reflectShaderAllBindings(ShaderVariantPtr shader, ShaderConstantBindings &constantBindings, ShaderTexSamplerBindings texSamplerBindings) = 0;

        /**
         * \brief 根据上下文设置好的资源、状态来渲染
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render() = 0;

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
        virtual TResult blit(RenderTargetPtr src, RenderTargetPtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源纹理传输图像数据到目标渲染目标，其中 src 和 dst 维度要相同
         * \param [in] src : 源纹理
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(TexturePtr src, RenderTargetPtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源渲染目标传输图像数据到目标纹理
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(RenderTargetPtr src, TexturePtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源纹理传输图像数据到目标纹理
         * \param [in] src : 源纹理
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源偏移，一个 3D 的偏移，按照 src 资源的维度去取 srcOffset 的维度
         * \param [in] size : 传输的大小，一个 3D 体积的大小，按照 src 资源的维度去取 size 的维度
         * \param [in] dstOffset : 目标便宜，一个 3D 的偏移，按照 src 资源的维度去取 dstOffset 的维度
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult blit(TexturePtr src, TexturePtr dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 从源缓冲区复制整个缓冲区数据到目标缓冲区
         * \param [in] src : 源渲染缓冲区
         * \param [in] dst : 目标渲染缓冲区
         * \param [in] srcOffset : 源缓冲区起始偏移
         * \param [in] size : 复制数据的大小
         * \param [in] dstOffset : 目标缓冲区起始偏移
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult copyBuffer(RenderBufferPtr src, RenderBufferPtr dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) = 0;

    protected:
        RHIContext();
    };
}


#include "T3DRHIContext.inl"


#endif  /*__T3D_RHI_CONTEXT_H__*/
