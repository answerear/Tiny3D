﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderWindow.h"
#include "Kernel/T3DCommon.h"


namespace Tiny3D
{
    /**
     * @class   Renderer
     * @brief   渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API Renderer : public Object
    {
    public:
        static const char * const REFERENCE3D;  /**< The reference 3D */
        static const char * const DIRECT3D9;    /**< Direct3D9 渲染器 */
        static const char * const DIRECT3D11;   /**< Direct3D11 渲染器 */
        static const char * const DIRECT3D12;   /**< Direct3D12 渲染器 */
        static const char * const OPENGL3;      /**< OpenGL 3.x 渲染器 */
        static const char * const OPENGLES2;    /**< OpenGL ES 2 渲染器 */
        static const char * const OPENGLES3;    /**< OpenGL ES 3 渲染器 */
        static const char * const VULKAN;       /**< Vulkan 渲染器 */
        static const char * const METAL;        /**< Metal 渲染器 */

        /**
         * @enum    Capability
         * @brief   渲染器能力值
         */
        enum Capability : uint32_t
        {
            E_CAP_RENDER_TO_TARGET = 0,     /**< 是否支持渲染到 surface */
            E_CAP_HARDWARE_TRANSFORM,       /**< 是否支持硬件变换 */
            E_CAP_MULTI_TEXTURE,            /**< 是否支持多重纹理 */
            E_CAP_MIP_MAP,                  /**< 是否支持 Mipmap */
            E_CAP_STENCIL_BUFFER,           /**< 是否支持 stencil buffer */
            E_CAP_VERTEX_SHADER_1_1,        /**< 是否支持 1.1 版本顶点着色器 */
            E_CAP_VERTEX_SHADER_2_0,        /**< 是否支持 2.0 版本顶点着色器 */
            E_CAP_VERTEX_SHADER_3_0,        /**< 是否支持 3.0 版本顶点着色器 */
            E_CAP_PIXEL_SHADER_1_1,         /**< 是否支持 1.1 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_2,         /**< 是否支持 1.2 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_3,         /**< 是否支持 1.3 版本片段着色器 */
            E_CAP_PIXEL_SHADER_1_4,         /**< 是否支持 1.4 版本片段着色器 */
            E_CAP_PIXEL_SHADER_2_0,         /**< 是否支持 2.0 版本片段着色器 */
            E_CAP_PIXEL_SHADER_3_0,         /**< 是否支持 3.0 版本片段着色器 */
            E_CAP_TEXTURE_NSQUARE,          /**< 是否支持非正方形纹理 */
            E_CAP_TEXTURE_NPOT,             /**< 是否支持非 2 次幂纹理 */
            E_CAP_FRAMEBUFFER,              /**< 是否支持帧缓冲 */
            E_CAP_MAX
        };

        /**
         * @enum    TransformState
         * @brief   设置变换矩阵状态
         */
        enum TransformState : uint32_t
        {
            VIEW = 0,      /**< 视口变换矩阵 */
            WORLD,         /**< 世界变换矩阵 */
            PROJECTION,    /**< 投影变换矩阵 */
            MAX
        };

        /**
         * @enum    PrimitiveType
         * @brief   渲染图元类型
         */
        enum PrimitiveType
        {
            E_PT_POINT_LIST = 0,    /**< 点列表图元 */
            E_PT_LINE_LIST,         /**< 线列表图元 */
            E_PT_LINE_STRIP,        /**< 线带图元 */
            E_PT_TRIANGLE_LIST,     /**< 三角形列表图元 */
            E_PT_TRIANGLE_STRIP,    /**< 三角形带图元 */
            E_PT_TRIANGLE_FAN,      /**< 三角形扇形图元 */
            E_PT_MAX
        };

        /**
         * @enum    ClearFlags
         * @brief   Values that represent clear flags
         */
        enum ClearFlags
        {
            E_CLEAR_STENCIL = 0,
            E_CLEAR_TARGET,
            E_CLEAR_ZBUFFER,
        };

        /**
         * @fn  Renderer::Renderer();
         * @brief   构造函数
         */
        Renderer();

        /**
         * @fn  virtual Renderer::~Renderer();
         * @brief   析构函数
         */
        virtual ~Renderer();

        /**
         * @fn  virtual TResult Renderer::init() = 0;
         * @brief   初始化渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult init() = 0;

        /**
         * @fn  virtual TResult Renderer::destroy() = 0;
         * @brief   销毁渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult destroy() = 0;

        /**
         * @fn  const String Renderer::&getName() const;
         * @brief   获取渲染器名称
         * @returns The name.
         */
        const String &getName() const;

        /**
         * @fn  virtual TResult Renderer::render();
         * @brief   渲染一帧
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult render();

        /**
         * @fn  virtual RenderWindowPtr Renderer::createRenderWindow(
         *      const String &name, const RenderWindowCreateParam &param, 
         *      const RenderWindowCreateParamEx &paramEx) = 0;
         * @brief   创建渲染窗口
         * @param [in]  name    : 用于标识窗口的名称，不需要跟窗口标题栏相同.
         * @param [in]  param   : 创建渲染窗口必要数据.
         * @param [in]  paramEx : 创建渲染窗口额外数据.
         * @returns 调用成功返回一个渲染窗口对象，失败返回nullptr.
         *
         * ### remarks  具体渲染系统实现本接口以创建渲染系统相关的渲染窗口对象.
         */
        virtual RenderWindowPtr createRenderWindow(
            const String &name,
            const RenderWindowCreateParam &param,
            const RenderWindowCreateParamEx &paramEx) = 0;

        /**
         * @fn  virtual TResult 
         *      Renderer::attachRenderTarget(RenderTargetPtr target);
         * @brief   关联渲染目标到渲染器上
         * @param [in]  target  : 渲染目标.
         * @returns 成功返回 T3D_OK.
         */
        virtual TResult attachRenderTarget(RenderTargetPtr target);

        /**
         * @fn  virtual TResult Renderer::detachRenderTarget(const String &name);
         * @brief   从渲染器上分离渲染目标
         * @param [in]  name    : 渲染目标名称.
         * @returns 成功返回 T3D_OK.
         */
        virtual TResult detachRenderTarget(const String &name);

        /**
         * @fn  RenderTargetPtr Renderer::getRenderTarget(const String &name);
         * @brief   获取渲染器上绑定的渲染目标
         * @param [in]  name    : 渲染目标名称.
         * @returns 成功返回渲染目标对象.
         */
        RenderTargetPtr getRenderTarget(const String &name);

        /**
         * @fn  virtual TResult Renderer::clear(const ColorRGB &color, 
         *      uint32_t clearFlags, Real depth, uint32_t stencil);
         * @brief   Clears this object to its blank/initial state
         * @param   color       The color.
         * @param   clearFlags  The clear flags.
         * @param   depth       The depth.
         * @param   stencil     The stencil.
         * @returns A TResult.
         */
        virtual TResult clear(const ColorRGB &color, uint32_t clearFlags,
            Real depth, uint32_t stencil);

        /**
         * @fn  virtual bool Renderer::queryCapability(Capability cap) const = 0;
         * @brief   查询渲染器能力
         * @param [in]  cap : 能力枚举值.
         * @returns 具备cap对应的能力则返回true，否则返回false.
         * @sa  Renderer::Capability
         */
        virtual bool queryCapability(Capability cap) const = 0;

        /**
         * @fn  virtual TResult Renderer::setTransform(TransformState state, 
         *      const Matrix4 &mat);
         * @brief   设置变换矩阵
         * @param [in]  state   : 变换矩阵类型.
         * @param [in]  mat     : 变换矩阵.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult setTransform(TransformState state, 
            const Matrix4 &mat);

        /**
         * @fn  TResult Renderer::setViewTransform(const Matrix4 &mat);
         * @brief   设置视图变换矩阵
         * @param [in]  mat : 变换矩阵.
         * @returns 调用成功返回 T3D_OK.
         */
        TResult setViewTransform(const Matrix4 &mat);

        /**
         * @fn  TResult Renderer::setWorldTransform(const Matrix4 &mat);
         * @brief   设置世界变换矩阵
         * @param [in]  mat : 变换矩阵.
         * @returns 调用成功返回 T3D_OK.
         */
        TResult setWorldTransform(const Matrix4 &mat);

        /**
         * @fn  TResult Renderer::setProjectionTransform(const Matrix4 &mat);
         * @brief   设置投影变换矩阵
         * @param [in]  mat : 变换矩阵.
         * @returns 调用成功返回 T3D_OK.
         */
        TResult setProjectionTransform(const Matrix4 &mat);

        /**
         * @fn  virtual const Matrix4 &
         *      Renderer::getTransform(TransformState state) const;
         * @brief   获取对应类型的变换矩阵
         * @param [in]  state   : 变换矩阵类型.
         * @returns 返回对应类型的变换矩阵.
         */
        virtual const Matrix4 &getTransform(TransformState state) const;

        /**
         * @brief 获取透视投影矩阵
         * @param [in] fovY : top和bottom的夹角
         * @param [in] aspect : 宽高比
         * @param [in] nearDist : 观察空间的近平面
         * @param [in] farDist : 观察空间的远平面
         * @return 返回一个平台相关的透视投影矩阵
         * @remarks 因为投影矩阵在不同渲染平台在NDC坐标系里z轴上会有的范围，
         *      因此，这里矩阵的获取通过不同渲染平台直接计算出来，屏蔽差异。
         *      Direct3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Open GL : x∈[-1, 1], y∈[-1, 1], z∈[-1, 1]
         *      Vulkan : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Metal : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Reference3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         */
        virtual Matrix4 perspective(const Radian &fovY, Real aspect, 
            Real nearDist, Real farDist) = 0;

        /**
         * @brief 获取正交投影矩阵
         * @param [in] width : 投影平面宽度
         * @param [in] height : 投影平面高度
         * @param [in] nearDist : 观察空间的近平面
         * @param [in] farDist : 观察空间的远平面
         * @return 返回一个平台相关的正交投影矩阵
         * @remarks 因为投影矩阵在不同渲染平台在NDC坐标系里z轴上会有不同的w范围，
         *      因此，这里矩阵的获取通过不同渲染平台直接计算出来，屏蔽差异。
         *      Direct3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Open GL : x∈[-1, 1], y∈[-1, 1], z∈[-1, 1]
         *      Vulkan : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Metal : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Reference3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         */
        virtual Matrix4 orthographic(Real width, Real height, 
            Real nearDist, Real farDist) = 0;

        /**
         * @brief 根据视口生成渲染器相关的视口变换矩阵
         * @param [in] viewport : 视口对象
         * @return 返回的视口变换矩阵
         */
        virtual Matrix4 makeViewportMatrix(ViewportPtr viewport) = 0;

        /**
         * @brief 更新视锥体的多个平面
         * @param [in] m : 投影变换矩阵和观察矩阵的连接，即 (M_proj * M_view)
         * @param [in][out] bound : 需要更新的视锥体碰撞体
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult updateFrustum(const Matrix4 &m, 
            FrustumBoundPtr bound) = 0;

        /**
         * @brief 设置裁剪模式
         * @param [in] mode : 裁剪模式
         * @return 成功返回 T3D_OK
         */
        virtual TResult setCullingMode(CullingMode mode) = 0;

        /**
         * @brief 获取裁剪模式
         */
        CullingMode getCullingMode() const;

        /**
         * @brief 设置渲染模式
         * @param [in] mode : 渲染模式
         * @return 成功返回 T3D_OK
         * @see enum PolygonMode
         */
        virtual TResult setPolygonMode(PolygonMode mode) = 0;

        /**
         * @brief 获取渲染模式
         */
        PolygonMode getPolygonMode() const;

        /**
         * @brief 设置渲染视口
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setViewport(ViewportPtr viewport) = 0;

        /**
         * @brief 获取渲染视口
         */
        ViewportPtr getViewport() const;

        /**
         * @fn  virtual TResult Renderer::setConstantBuffer(size_t slot, 
         *      HardwareConstantBufferPtr buffer) = 0;
         * @brief   设置当前渲染对象在 GPU 程序中使用的常量缓冲区对象
         * @param [in]  slot    常量缓冲区槽索引.
         * @param [in]  buffer  常量缓冲区对象.
         * @returns 调用成功返回 T3D_OK.
         * @remarks 具体渲染平台实现类需要实现本接口
         */
        virtual TResult setConstantBuffer(size_t slot, 
            HardwareConstantBufferPtr buffer) = 0;

        /**
         * @fn  virtual HardwareConstantBufferPtr 
         *      Renderer::getConstantBuffer(size_t slot) const;
         * @brief   获取指定槽的常量缓冲区对象
         * @param [in]  slot    指定槽索引.
         * @returns 调用成功返回常量缓冲区对象，否则返回nullptr.
         */
        HardwareConstantBufferPtr getConstantBuffer(size_t slot) const;

        /**
         * @fn  virtual TResult 
         *      Renderer::setGPUProgram(GPUProgramPtr program) = 0;
         * @brief   设置当前渲染对象用的 GPU 程序对象
         * @param [in]  program GPU 程序对象.
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult setGPUProgram(GPUProgramPtr program) = 0;

        /**
         * @fn  virtual GPUProgramPtr Renderer::getGPUProgram() const;
         * @brief   Gets GPU program
         * @returns The GPU program.
         */
        GPUProgramPtr getGPUProgram() const;

        /**
         * @brief 绘制顶点数组
         * @param [in] vao : 顶点数组对象
         * @return 调动成功返回 T3D_OK
         */
        virtual TResult drawVertexArray(VertexArrayObjectPtr vao) = 0;

    protected:
        /**
         * @fn  virtual TResult Renderer::postInit();
         * @brief   Posts the initialize
         * @returns A TResult.
         */
        virtual TResult postInit();

        /**
         * @fn  TResult Renderer::updateBufferPerObject();
         * @brief   Updates the buffer per object
         * @return  A TResult.
         */
        TResult updateBufferPerObject();

        /**
         * @fn  TResult Renderer::updateBufferPerFrame();
         * @brief   Updates the buffer per frame
         * @return  A TResult.
         */
        TResult updateBufferPerFrame();

        /**
         * @fn  TResult Renderer::updateBufferRarely();
         * @brief   Updates the buffer rarely
         * @return  A TResult.
         */
        TResult updateBufferRarely();

    protected:
        typedef TMap<String, RenderTargetPtr>       RenderTargetList;
        typedef RenderTargetList::iterator          RenderTargetListItr;
        typedef RenderTargetList::const_iterator    RenderTargetListConstItr;
        typedef RenderTargetList::value_type        RenderTargetListValue;

        typedef TArray<HardwareConstantBufferPtr>   ConstantBufferList;
        typedef ConstantBufferList::iterator        ConstantBufferListItr;
        typedef ConstantBufferList::const_iterator  ConstantBufferListConstItr;

        struct GPUConstUpdatePerObject
        {
            GPUConstUpdatePerObject() {}

            Matrix4 mWorldMatrix;                       /**< The world matrix */
            Matrix4 mInverseWorldM;                     /**< The inverse world matrix */
            Matrix4 mTransposeWorldM;                   /**< The transpose world matrix */
            Matrix4 mInverseTransposeWorldM;            /**< The inverse transpose world matrix */

            Matrix4 mWorldViewMatrix;                   /**< The world view matrix */
            Matrix4 mInverseWorldViewM;                 /**< The inverse world view m */
            Matrix4 mTransposeWorldViewM;               /**< The transpose world view m */
            Matrix4 mInverseTransposeWorldViewM;        /**< The inverse transpose world view m */

            Matrix4 mWorldViewProjMatrix;               /**< The world view project matrix */
            Matrix4 mInverseWorldViewProjM;             /**< The inverse world view project m */
            Matrix4 mTransposeWorldViewProjM;           /**< The transpose world view project m */
            Matrix4 mInverseTransposeWorldViewProjM;    /**< The inverse transpose world view project m */
        };

        struct GPUConstUpdatePerFrame
        {
            GPUConstUpdatePerFrame() {}

            Matrix4 mViewMatrix;                /**< The view matrix */
            Matrix4 mInverseViewM;              /**< The inverse view matrix */
            Matrix4 mTransposeViewM;            /**< The transpose view matrix */
            Matrix4 mInverseTransposeViewM;     /**< The inverse transpose view matrix */

            Matrix4 mViewProjMatrix;            /**< The view project matrix */
            Matrix4 mInverseViewProjM;          /**< The inverse view project m */
            Matrix4 mTransposeViewProjM;        /**< The transpose view project m */
            Matrix4 mInverseTransposeViewProjM; /**< The inverse transpose view project m */
        };

        struct GPUConstUpdateRarely
        {
            GPUConstUpdateRarely() {}

            Matrix4 mProjMatrix;                /**< The project matrix */
            Matrix4 mInverseProjM;              /**< The inverse project m */
            Matrix4 mTransposeProjM;            /**< The transpose project m */
            Matrix4 mInverseTransposeProjM;     /**< The inverse transpose project m */
        };

        String              mName;              /**< 渲染器名称 */

        RenderTargetList    mRenderTargets;     /**< 渲染目标列表 */

        RenderTargetPtr     mRenderTarget;      /**< 当前渲染目标 */

        ViewportPtr         mViewport;          /**< 当前渲染视口对象 */

        CullingMode         mCullingMode;       /**< 裁剪模式 */
        PolygonMode         mPolygonMode;       /**< 多边形渲染模式 */

        GPUConstBufferPtr           mGPUBufferUpdateObject; /**< 每个对象都更新的GPU常量缓冲 */
        GPUConstBufferPtr           mGPUBufferUpdateFrame;  /**< 每帧都更新的GPU常量缓冲 */
        GPUConstBufferPtr           mGPUBufferUpdateRarely; /**< 很少更新的GPU常量缓冲 */

        GPUConstUpdatePerObject     mGPUConstUpdateObject;  /**< 对应每个对象都更新的GPU常量缓冲数据 */
        GPUConstUpdatePerFrame      mGPUConstUpdateFrame;   /**< 对应每帧都更新的GPU常量缓冲数据 */
        GPUConstUpdateRarely        mGPUConstUpdateRarely;  /**< 对应很少更新的GPU常量缓冲 */

        bool    mIsWorldMatrixDirty;    /**< 世界变换矩阵是否更新标识 */
        bool    mIsViewMatrixDirty;     /**< 视图变换矩阵是否更新标识 */
        bool    mIsProjMatrixDirty;     /**< 投影变换矩阵是否更新标识 */

        ConstantBufferList  mConstBuffers;      /**< 外部传入的常量缓冲区对象 */
        GPUProgramPtr       mGPUProgram;        /**< 当前渲染用GPU程序 */
    };
}


#include "T3DRenderer.inl"


#endif  /*__T3D_RENDERER_H__*/
