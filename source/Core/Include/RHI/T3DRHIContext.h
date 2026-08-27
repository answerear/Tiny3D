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
#include "RHI/T3DRHICapabilities.h"


namespace Tiny3D
{
    struct RenderWindowDesc;
    struct BlendDesc;
    struct DepthStencilDesc;
    struct RasterizerDesc;
    struct SamplerDesc;

    /**
     * \brief 变换矩阵状态槽位，用于区分视图/世界/投影矩阵
     */
    enum class TransformState : uint32_t
    {
        kView = 0,      ///< 视图变换矩阵
        kWorld,         ///< 世界变换矩阵
        kProjection,    ///< 投影变换矩阵
    };

    /**
     * \brief RHI 渲染上下文抽象基类，定义资源创建、管线状态绑定与 draw 等后端无关接口
     * \remarks 具体 Graphics API 由插件派生类实现；基类维护视图/投影矩阵缓存
     */
    class T3D_ENGINE_API RHIContext : public Object
    {
    public:
        /// 析构
        ~RHIContext() override;

        /**
         * \brief 设置视图与投影矩阵并更新联合矩阵
         * \param [in] viewMat : 视图变换矩阵
         * \param [in] projMat : 投影变换矩阵
         * \return 调用成功返回 T3D_OK；各后端可能对 projMat 做 NDC/深度 remap 或 Y 翻转
         */
        virtual TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat) = 0;

        /// 返回缓存的视图矩阵 mViewMatrix
        const Matrix4 &getViewMatrix() const { return mViewMatrix; }

        /// 返回缓存的投影矩阵 mProjMatrix（可能已过后端转换）
        const Matrix4 &getProjMatrix() const { return mProjMatrix; }

        /// 返回投影×视图联合矩阵 mProjViewMatrix
        const Matrix4 &getProjViewMatrix() const { return mProjViewMatrix; }

        /// 当前投影矩阵是否被 Y 翻转（OpenGL 渲染到 FBO 时由后端置 true）
        bool isProjectionFlipped() const { return mProjectionFlipped; }

        /**
         * \brief 返回当前后端的能力集
         * \return 后端在 init() 中填充的能力集；未填充时全部为 false / 0
         * \remarks 刻意不做成纯虚 getter：数据成员默认全 false，后端忘了填时上层
         *          查到「不支持」而走降级路径，只损失性能不产生错误结果。
         *          若改成纯虚，后端更容易随手返回一个全 true 的静态对象来过编译，
         *          失败方向反而变得不安全。
         */
        const RHICapabilities &getCapabilities() const { return mCapabilities; }

        /**
         * \brief 返回深度重映射矩阵，将光空间 Z 从平台 NDC 范围映射到 [0,1]。
         *        D3D11 的投影矩阵已内含 Z remap，默认返回单位矩阵。
         *        GL 系列（GL4/GLES3）保持原生 [-1,1] NDC，override 返回 Z remap 矩阵。
         * \return 深度重映射矩阵的常引用
         */
        virtual const Matrix4& getDepthRemapMatrix() const;

        /**
         * \brief 创建与引擎 RenderWindow 绑定的 RHI 渲染窗口
         * \param [in] renderWindow : 引擎 RenderWindow 对象
         * \return 调用成功返回新建的 RHIRenderTargetPtr；失败时返回 nullptr（由后端决定）
         */
        virtual RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) = 0;

        /**
         * \brief 为引擎 PixelBuffer2D 创建对应的 RHI 渲染纹理
         * \param [in] buffer : 引擎二维像素缓冲（渲染纹理）
         * \return 调用成功返回新建的 RHIPixelBuffer2DPtr；失败时返回 nullptr
         */
        virtual RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) = 0;

        /**
         * \brief 按渲染纹理当前的描述重建底层 GPU 资源
         * \param [in] rt : 目标渲染纹理，调用前其描述已被更新为新尺寸
         * \param [in] width : 新宽度，用于与描述做一致性校验
         * \param [in] height : 新高度
         * \return 调用成功返回 T3D_OK；后端未实现时返回 T3D_ERR_NOT_IMPLEMENT
         * \note 由 RenderTexture::resize 驱动，后端只负责重建 RHI 资源，不修改引擎侧描述
         */
        virtual TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height) { return T3D_ERR_NOT_IMPLEMENT; }

        /**
         * \brief 改变渲染目标及其全部附件的大小
         * \param [in] rt : 目标渲染目标，窗口型分派到 swapchain 重建，纹理型逐个附件重建
         * \param [in] width : 新宽度
         * \param [in] height : 新高度
         * \return 调用成功返回 T3D_OK；后端未实现时返回 T3D_ERR_NOT_IMPLEMENT
         */
        virtual TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height) { return T3D_ERR_NOT_IMPLEMENT; }

        /**
         * \brief 设置当前渲染目标
         * \param [in] renderTarget : 目标 RenderTarget，可为 nullptr 表示解绑（由后端实现）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setRenderTarget(RenderTarget *renderTarget) = 0;

        /**
         * \brief 清除当前渲染目标绑定，恢复默认/back buffer
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult resetRenderTarget() = 0;

        /**
         * \brief 设置视口矩形
         * \param [in] viewport : 视口参数（原点、宽高、深度范围）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setViewport(const Viewport &viewport) = 0;

        /**
         * \brief 设置裁剪矩形（像素坐标，原点左上，Y 向下）
         * \param [in] x : 矩形左上角 X（像素）
         * \param [in] y : 矩形左上角 Y（像素，Y 向下）
         * \param [in] width : 矩形宽度（像素）
         * \param [in] height : 矩形高度（像素）
         * \return 调用成功返回 T3D_OK
         * \note 须配合 RasterizerDesc::ScissorEnable=true 才生效。
         *       OpenGL 后端会在内部翻成 glScissor 的左下原点。
         *       与 setViewport 独立，不在 setViewport 中顺带修改。
         */
        virtual TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;
        
        /**
         * \brief 用指定颜色清屏（color buffer）
         * \param [in] color : 清屏颜色
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult clearColor(const ColorRGB &color) = 0;

        /**
         * \brief 用指定深度值清屏（depth buffer）
         * \param [in] depth : 深度清屏值
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult clearDepth(Real depth) = 0;

        /**
         * \brief 用指定深度与模板值清屏
         * \param [in] depth : 深度清屏值
         * \param [in] stencil : 模板清屏值
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
         * \brief 创建 RHI 顶点声明对象
         * \param [in] decl : 引擎 VertexDeclaration
         * \return 调用成功返回新建的 RHIVertexDeclarationPtr；失败时返回 nullptr
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
         * \brief 创建 RHI 结构化缓冲对象
         * \param [in] buffer : 引擎的结构化缓冲对象
         * \return 调用成功返回 RHI 对象；后端不支持或创建失败时返回 nullptr
         * \note 是否创建 SRV / UAV 由 buffer->getGPUAccess() 决定。
         *       普通纹理与顶点/索引缓冲的 UAV 不走这里，它们在各自已有的
         *       createPixelBufferXD / createVertexBuffer 里按 getGPUAccess() 顺带建出。
         */
        virtual RHIStructuredBufferPtr createStructuredBuffer(StructuredBuffer *buffer) = 0;

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
         * \brief 创建 RHI 立方体贴图像素缓冲区对象
         * \param [in] buffer : 引擎像素缓冲区对象
         * \return 调用成功返回 RHI 对象，不支持立方体贴图的后端返回 nullptr
         */
        virtual RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer) = 0;

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
         * \return 调用成功返回 T3D_OK
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
         * \brief 设置 vs 的只读结构化缓冲（SRV）
         * \param [in] startSlot : 插槽，对应 shader 中 t# 寄存器索引
         * \param [in] buffers : 结构化缓冲数组；元素为 nullptr 表示解绑该槽
         * \return 调用成功返回 T3D_OK
         * \note 结构化缓冲与像素缓冲共享同一组 t# 寄存器，插槽范围不要与
         *       setVSPixelBuffers 重叠，否则后绑定的一方会覆盖前者
         */
        virtual TResult setVSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) = 0;

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
         * \return 调用成功返回 T3D_OK
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
         * \brief 设置 ps 的只读结构化缓冲（SRV）
         * \param [in] startSlot : 插槽，对应 shader 中 t# 寄存器索引
         * \param [in] buffers : 结构化缓冲数组；元素为 nullptr 表示解绑该槽
         * \return 调用成功返回 T3D_OK
         * \note 插槽范围不要与 setPSPixelBuffers 重叠
         */
        virtual TResult setPSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) = 0;
        
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
         * \return 调用成功返回 T3D_OK
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
         * \return 调用成功返回 T3D_OK
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
         * \return 调用成功返回 T3D_OK
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
         * \return 调用成功返回 T3D_OK
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
         * \brief 设置 cs 的只读结构化缓冲（SRV）
         * \param [in] startSlot : 插槽，对应 shader 中 t# 寄存器索引
         * \param [in] buffers : 结构化缓冲数组；元素为 nullptr 表示解绑该槽
         * \return 调用成功返回 T3D_OK
         * \note 插槽范围不要与 setCSPixelBuffers 重叠
         */
        virtual TResult setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) = 0;

        /**
         * \brief 设置 cs 的无序访问视图（UAV）
         * \param [in] startSlot : UAV 插槽，对应 shader 中 u# 寄存器索引
         * \param [in] buffers : 可读写资源数组，元素可为像素缓冲、结构化缓冲或
         *                       带 kGPUUnorderedAccess 的顶点缓冲；元素为 nullptr 表示解绑该槽
         * \param [in] initialCounts : Append/Consume 与 Counter 缓冲的初始计数值，
         *                             长度须为 0 或与 buffers 等长；
         *                             取 kKeepUAVCounter 表示保持当前计数；
         *                             长度为 0 时等价于全部 kKeepUAVCounter
         * \return 调用成功返回 T3D_OK；资源未带 kGPUUnorderedAccess 或槽位越界时
         *         返回 T3D_ERR_INVALID_PARAM
         * \note 同一资源不能同时绑定为 UAV 与 SRV。切换用途前须调用 uavBarrier，
         *       否则 D3D11 会静默解除 SRV 绑定，其它后端行为未定义。
         * \note 纹理资源当前只提供 mip 0 的 UAV。需要逐 mip 写入（GPU 生成 mip 链、
         *       Hi-Z、体素化）时须先扩展后端的视图数组与本接口的 mip 参数。
         */
        virtual TResult setCSUnorderedAccessBuffers(uint32_t startSlot,
            const UnorderedAccessBuffers &buffers,
            const UAVInitialCounts &initialCounts = UAVInitialCounts()) = 0;

        /**
         * \brief 派发 compute shader
         * \param [in] groupCountX : X 维线程组数量
         * \param [in] groupCountY : Y 维线程组数量
         * \param [in] groupCountZ : Z 维线程组数量
         * \return 调用成功返回 T3D_OK；任一维为 0 或超出
         *         getCapabilities().maxDispatchGroupCount 时返回 T3D_ERR_INVALID_PARAM
         * \note 调用前须已通过 setComputeShader / setCS* 系列完成资源绑定
         */
        virtual TResult dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

        /**
         * \brief 按 GPU 缓冲中的参数间接派发 compute shader
         * \param [in] argsBuffer : 参数缓冲，须带 kGPUIndirectArgs，
         *                          内容布局为 DispatchIndirectArgs
         * \param [in] argsOffset : 参数在缓冲中的字节偏移，须为 4 的倍数
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) = 0;

        /**
         * \brief UAV 写后读同步点：保证之前的 UAV 写入对后续读取可见，并解除 UAV 绑定
         * \param [in] buffers : 需要同步的资源列表；为空表示对所有当前绑定的 UAV 生效
         * \return 调用成功返回 T3D_OK
         * \remarks 各后端语义：
         *          - D3D11：将这些资源占用的 CS UAV 槽置空。驱动自行跟踪 hazard，
         *            无需显式 barrier；但不解绑就无法把同一资源当作 SRV 使用。
         *          - Vulkan：vkCmdPipelineBarrier，含 storage image 的 layout 转换。
         *          - OpenGL：glMemoryBarrier(SHADER_STORAGE | TEXTURE_FETCH |
         *            SHADER_IMAGE_ACCESS | COMMAND_BARRIER)。
         */
        virtual TResult uavBarrier(const UnorderedAccessBuffers &buffers) = 0;

        /**
         * \brief 把 Append/Counter UAV 的当前元素计数拷贝到目标缓冲的指定偏移
         * \param [in] dstBuffer : 目标缓冲，通常是 indirect 参数缓冲
         * \param [in] dstOffset : 目标字节偏移，须为 4 的倍数
         * \param [in] srcBuffer : 源结构化缓冲，创建时须置 hasCounter 或 isAppendConsume
         * \return 调用成功返回 T3D_OK
         * \note 这是纯 GPU 侧搬运，不涉及 CPU 同步，是 GPU-driven 剔除的关键原语：
         *       compute 用 Append 缓冲收集可见实例，计数直接落进
         *       DrawIndexedIndirectArgs::instanceCount 字段。
         */
        virtual TResult copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset,
            RenderBuffer *srcBuffer) = 0;

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
         * \brief 运行时反射着色器的纹理采样器绑定信息（需要 RHI shader 已创建）
         * \param [in] shader : 要反射的着色器（RHI shader 已创建）
         * \param [in,out] samplerParams : 已有的纹理采样参数，更新其 binding 索引
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams) = 0;

        /**
         * \brief 设置渲染图元类型
         * \param [in] primitive : 图元类型
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult setPrimitiveType(PrimitiveType primitive) = 0;

        /**
         * \brief 索引绘制：按当前绑定的管线状态与资源提交 draw call
         * \param [in] indexCount : 索引数量
         * \param [in] startIndex : 索引缓冲区起始位置
         * \param [in] baseVertex : 加到每个索引值上的顶点基址，用于索引顶点缓冲
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) = 0;

        /**
         * \brief 非索引绘制：按当前绑定的管线状态与资源提交 draw call
         * \param [in] vertexCount : 顶点数量
         * \param [in] startVertex : 顶点缓冲区起始位置
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(uint32_t vertexCount, uint32_t startVertex) = 0;

        /**
         * \brief 索引实例化绘制
         * \param [in] indexCount : 单个实例的索引数量
         * \param [in] instanceCount : 实例数量
         * \param [in] startIndex : 索引缓冲区起始位置
         * \param [in] baseVertex : 加到每个索引值上的顶点基址，可为负
         * \param [in] startInstance : 起始实例编号，非 0 时需要
         *                             getCapabilities().supportsBaseInstance
         * \return 调用成功返回 T3D_OK
         * \note 刻意不复用 render 重载：render(a,b,c) 与 render(a,b) 已经靠参数个数
         *       区分索引/非索引，再加 4/5 参重载后写错参数不会编译报错，只会画出
         *       错误的东西。
         */
        virtual TResult renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
            uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) = 0;

        /**
         * \brief 非索引实例化绘制
         * \param [in] vertexCount : 单个实例的顶点数量
         * \param [in] instanceCount : 实例数量
         * \param [in] startVertex : 顶点缓冲区起始位置
         * \param [in] startInstance : 起始实例编号，非 0 时需要
         *                             getCapabilities().supportsBaseInstance
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult renderInstanced(uint32_t vertexCount, uint32_t instanceCount,
            uint32_t startVertex, uint32_t startInstance) = 0;

        /**
         * \brief 按 GPU 缓冲中的参数进行索引间接绘制
         * \param [in] argsBuffer : 参数缓冲，须带 kGPUIndirectArgs，
         *                          内容布局为 DrawIndexedIndirectArgs
         * \param [in] argsOffset : 参数字节偏移，须为 4 的倍数
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) = 0;

        /**
         * \brief 按 GPU 缓冲中的参数进行非索引间接绘制
         * \param [in] argsBuffer : 参数缓冲，须带 kGPUIndirectArgs，
         *                          内容布局为 DrawIndirectArgs
         * \param [in] argsOffset : 参数字节偏移，须为 4 的倍数
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) = 0;

        /**
         * \brief 清除所有状态、渲染资源，包括 RenderTarget
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult reset() = 0;

        /**
         * \brief 图像块传输：RenderTarget → RenderTarget
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源区域起点（3D 偏移，按 src 维度解释）
         * \param [in] size : 传输区域尺寸（3D，按 src 维度解释）
         * \param [in] dstOffset : 目标区域起点（3D 偏移，按 dst 维度解释）
         * \return 调用成功返回 T3D_OK；src 与 dst 维度须匹配
         */
        virtual TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 图像块传输：Texture → RenderTarget
         * \param [in] src : 源纹理
         * \param [in] dst : 目标渲染目标
         * \param [in] srcOffset : 源区域起点（3D 偏移，按 src 维度解释）
         * \param [in] size : 传输区域尺寸（3D，按 src 维度解释）
         * \param [in] dstOffset : 目标区域起点（3D 偏移，按 dst 维度解释）
         * \return 调用成功返回 T3D_OK；src 与 dst 维度须匹配
         */
        virtual TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 图像块传输：RenderTarget → Texture
         * \param [in] src : 源渲染目标
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源区域起点（3D 偏移，按 src 维度解释）
         * \param [in] size : 传输区域尺寸（3D，按 src 维度解释）
         * \param [in] dstOffset : 目标区域起点（3D 偏移，按 dst 维度解释）
         * \return 调用成功返回 T3D_OK；src 与 dst 维度须匹配
         */
        virtual TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) = 0;

        /**
         * \brief 图像块传输：Texture → Texture
         * \param [in] src : 源纹理
         * \param [in] dst : 目标纹理
         * \param [in] srcOffset : 源区域起点（3D 偏移，按 src 维度解释）
         * \param [in] size : 传输区域尺寸（3D，按 src 维度解释）
         * \param [in] dstOffset : 目标区域起点（3D 偏移，按 dst 维度解释）
         * \return 调用成功返回 T3D_OK；src 与 dst 维度须匹配
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
         * \brief 将 CPU 侧 Buffer 数据写入 GPU RenderBuffer
         * \param [in] renderBuffer : 目标 GPU 渲染缓冲
         * \param [in] buffer : CPU 数据源（Data/DataSize）
         * \param [in] discardWholeBuffer : 为 true 时表示丢弃原有 GPU 数据后整缓冲重写（动态缓冲路径，具体约束由后端实现）
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) = 0;

        /**
         * \brief 发起线性缓冲（VB / IB / CB / StructuredBuffer）的 GPU→CPU 读回
         * \param [in] src : 源缓冲，创建时必须带 kCPURead 许可
         * \param [in] offset : 起始字节偏移
         * \param [in] size : 读取字节数，0 表示从 offset 到末尾
         * \return 成功返回读回票据；参数非法或后端不支持时返回 ReadbackHandle::invalid()
         * \remarks 必须在 beginRender / endRender 之间调用（应用层即 onRender）。
         *          本接口只把 Copy→staging 录进当前命令流，立即返回，不做任何 Map。
         *          kCPURead 是引擎的读回许可，不会让源资源带上原生 CPU_ACCESS_READ。
         */
        virtual ReadbackHandle beginReadBuffer(RenderBuffer *src, size_t offset, size_t size) = 0;

        /**
         * \brief 消费 beginReadBuffer 的结果
         * \param [in] handle : beginReadBuffer 返回的票据
         * \param [out] dst : 由实现分配并填充的紧凑字节数据，调用方负责 release()
         * \return 调用成功返回 T3D_OK；无效票据返回 T3D_ERR_INVALID_PARAM
         * \warning **会阻塞等待 GPU 完成**。这是验证 / CI / 按键截帧的路径，
         *          禁止放进游戏热路径。必须在 endRender 之后调用（应用层即 onPostRender）。
         */
        virtual TResult endReadBuffer(ReadbackHandle handle, Buffer &dst) = 0;

        /**
         * \brief 发起纹理 / 渲染纹理（1D/2D/3D/Cube）的 GPU→CPU 读回
         * \param [in] src : 源像素缓冲，创建时必须带 kCPURead 许可
         * \param [in] region : 待读回的子区域，mip + arraySlice 选定子资源
         * \return 成功返回读回票据；参数非法或后端不支持时返回 ReadbackHandle::invalid()
         * \remarks 时序约束同 beginReadBuffer。MSAA 源由实现先 Resolve 再 Copy。
         */
        virtual ReadbackHandle beginReadTexture(RenderBuffer *src, const ReadbackRegion &region) = 0;

        /**
         * \brief 消费 beginReadTexture 的结果
         * \param [in] handle : beginReadTexture 返回的票据
         * \param [out] dst : 由实现分配并填充的紧凑像素数据，调用方负责 release()
         * \return 调用成功返回 T3D_OK；无效票据返回 T3D_ERR_INVALID_PARAM
         * \remarks dst 是紧凑排布：rowPitch = width * bpp，slicePitch = rowPitch * height，
         *          不含 GPU 侧的行对齐 padding。
         * \warning 阻塞语义同 endReadBuffer。
         */
        virtual TResult endReadTexture(ReadbackHandle handle, Buffer &dst) = 0;

        /**
         * \brief 获取主渲染 context 的原生句柄
         * \return GL 后端返回 HGLRC/GLXContext，D3D11 等返回 nullptr
         */
        virtual void* getNativeContext() const { return nullptr; }

        /**
         * \brief 恢复主窗口的渲染 context（multi-viewport 子窗口渲染后调用）
         */
        virtual void restoreNativeContext() {}

        /**
         * \brief 开始一帧的渲染命令录制。
         *        对于 Vulkan 后端，执行 waitFence + acquireNextImage + beginCommandBuffer。
         *        对于 D3D11/OpenGL4 后端，空实现。
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult beginRender() = 0;

        /**
         * \brief 结束一帧的渲染命令录制并提交。
         *        对于 Vulkan 后端，执行 endCommandBuffer + queueSubmit。
         *        对于 D3D11/OpenGL4 后端，空实现。
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult endRender() = 0;

        /**
         * \brief 开始一个渲染 Pass。
         *        对于 Vulkan 后端，执行 vkCmdBeginRenderPass。
         *        对于 D3D11/OpenGL4 后端，空实现。
         *        应在 clear 操作之后、draw call 之前调用。
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult beginPass() = 0;

        /**
         * \brief 结束一个渲染 Pass。
         *        对于 Vulkan 后端，执行 vkCmdEndRenderPass。
         *        对于 D3D11/OpenGL4 后端，空实现。
         *        应在所有 draw call 之后、blit/reset 之前调用。
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult endPass() = 0;

    protected:
        /// 受保护构造，由后端 RHIContext 派生类调用
        RHIContext();

    protected:
        /// 投影变换矩阵（可能已过后端 NDC/深度转换）
        Matrix4 mProjMatrix {false};
        /// 视图变换矩阵
        Matrix4 mViewMatrix {false};
        /// 投影×视图联合矩阵
        Matrix4 mProjViewMatrix {false};
        /// 投影矩阵是否被 Y 翻转（OpenGL 渲染到 FBO 时由后端置 true）
        bool mProjectionFlipped {false};
        /// 后端能力集，由派生类在 init() 中填充
        RHICapabilities mCapabilities {};
    };
}


#include "T3DRHIContext.inl"


#endif  /*__T3D_RHI_CONTEXT_H__*/
