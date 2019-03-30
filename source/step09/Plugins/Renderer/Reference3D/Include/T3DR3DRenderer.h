/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


#ifndef __T3D_R3D_RENDERER_H__
#define __T3D_R3D_RENDERER_H__


#include "T3DR3DPrerequisites.h"


namespace Tiny3D
{
    class R3DRenderer : public Renderer
    {
    public:
        static R3DRendererPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~R3DRenderer();

        /**
         * @brief 初始化渲染器
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult init() override;

        /**
         * @brief 销毁渲染器
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult destroy() override;

        /**
         * @brief 创建渲染窗口
         * @param [in] name : 用于标识窗口的名称，不需要跟窗口标题栏相同
         * @param [in] param : 创建渲染窗口必要数据
         * @param [in] paramEx : 创建渲染窗口额外数据
         * @return 调用成功返回一个渲染窗口对象，失败返回nullptr
         * @remarks 具体渲染系统实现本接口以创建渲染系统相关的渲染窗口对象
         */
        virtual RenderWindowPtr createRenderWindow(
            const String &name,
            const RenderWindowCreateParam &param,
            const RenderWindowCreateParamEx &paramEx) override;

        /**
        * @brief 开始渲染
        * @param [in] count : 清除矩形区域的数量
        * @param [in] pRects : 矩形区域数组
        * @param [in] clearFlags : 清除标记
        * @param [in] color : 用于清除的背景颜色
        * @param [in] z : 用于清除的深度值
        * @param [in] stencil : 用于清除模板缓冲的值
        * @return 调用成功返回 T3D_OK
        */
        virtual TResult beginRender(size_t count, Rect *pRects,
            uint32_t clearFlags, const Color3f &color, Real z,
            uint32_t stencil) override;

        /**
         * @brief 结束渲染
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult endRender() override;

        /**
         * @brief 查询渲染器能力
         * @param [in] cap : 能力枚举值
         * @return 具备cap对应的能力则返回true，否则返回false
         * @see Renderer::Capability
         */
        virtual bool queryCapability(Capability cap) const override;

        /**
         * @brief 设置变换矩阵
         * @param [in] state : 变换矩阵类型
         * @param [in] mat : 变换矩阵
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setTransform(TransformState state,
            const Matrix4 &mat) override;
        
        /**
         * @brief 获取对应类型的变换矩阵
         * @param [in] state : 变换矩阵类型
         * @return 返回对应类型的变换矩阵
         */
        virtual const Matrix4 &getTransform(
            TransformState state) const override;

        /**
         * @brief 获取透视投影矩阵
         * @param [in] left : 观察空间的左边
         * @param [in] right : 观察空间的右边
         * @param [in] top : 观察空间的上边
         * @param [in] bottom : 观察空间的下边
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
        virtual Matrix4 perspective(Real left, Real right, Real top,
            Real bottom, Real nearDist, Real farDist) override;

        /**
         * @brief 获取正交投影矩阵
         * @param [in] left : 观察空间的左边
         * @param [in] right : 观察空间的右边
         * @param [in] top : 观察空间的上边
         * @param [in] bottom : 观察空间的下边
         * @param [in] nearDist : 观察空间的近平面
         * @param [in] farDist : 观察空间的远平面
         * @return 返回一个平台相关的正交投影矩阵
         * @remarks 因为投影矩阵在不同渲染平台在NDC坐标系里z轴上会有不同的w范围，
         *       因此，这里矩阵的获取通过不同渲染平台直接计算出来，屏蔽差异。
         *      Direct3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *       Open GL : x∈[-1, 1], y∈[-1, 1], z∈[-1, 1]
         *      Vulkan : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Metal : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Reference3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         */
        virtual Matrix4 orthographic(Real left, Real right, Real top, 
            Real bottom, Real nearDist, Real farDist) override;

        /**
         * @brief 根据视口生成渲染器相关的视口变换矩阵
         * @param [in] viewport : 视口对象
         * @return 返回的视口变换矩阵
         */
        virtual Matrix4 makeViewportMatrix(ViewportPtr viewport) override;

        /**
         * @brief 更新视锥体的多个平面
         * @param [in] m : 投影变换矩阵和观察矩阵的连接，即 (M_proj * M_view)
         * @param [in][out] frustum : 需要更新的视锥体
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult updateFrustum(const Matrix4 &m, 
            FrustumBoundPtr bound) override;

        /**
         * @brief 设置裁剪模式
         * @param [in] mode : 裁剪模式
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setCullingMode(CullingMode mode) override;

        /**
         * @brief 设置渲染模式
         * @param [in] mode : 渲染模式
         * @return 成功返回 T3D_OK
         * @see enum RenderMode
         */
        virtual TResult setRenderMode(RenderMode mode) override;

        /**
         * @brief 设置渲染视口
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setViewport(ViewportPtr viewport) override;

        /**
         * @brief 设置渲染材质
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setMaterial(MaterialPtr material) override;

        /**
         * @brief 绘制顶点数组
         * @param [in] vao : 顶点数组对象
         * @return 调动成功返回 T3D_OK
         */
        virtual TResult drawVertexArray(VertexArrayObjectPtr vao) override;

        /**
         * @brief 绘制顶点列表
         * @param [in] priType : 图元类型
         * @param [in] decl : 顶点声明
         * @param [in] vbo : 顶点缓冲
         * @param [in] startIdx : 顶点缓冲区的起始位置
         * @param [in] priCount : 图元数量
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult drawVertexList(PrimitiveType priType,
            VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo) override;

        /**
        * @brief 绘制索引列表
        * @param [in] priType : 图元类型
        * @param [in] decl : 顶点声明
        * @param [in] vbo : 顶点缓冲
        * @param [in] ibo : 索引缓冲
        * @param [in] startIdx : 顶点索引起始位置偏移
        * @param [in] priCount : 图元数量
        * @return 调用成功返回 T3D_OK
        */
        virtual TResult drawIndexList(PrimitiveType priType,
            VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo, 
            HardwareIndexBufferPtr ibo) override;

    protected:
        /**
         * @brief 构造函数
         */
        R3DRenderer();

        TResult clearFramebuffer(size_t count, Rect *pRects, 
            const Color3f &color);

    protected:
        R3DFramebufferPtr           mFramebuffer;

        R3DRenderWindowPtr          mRenderWindow;          /**< 渲染窗口 */
        HardwareBufferManagerPtr    mHardwareBufferMgr;     /**< 硬件缓冲管理器 */
        R3DHardwareBufferManagerPtr mR3DHardwareBufferMgr;  /**< 渲染器相关的缓冲区管理对象 */

        Matrix4                     mMatrices[E_TS_MAX];    /**< 各种变换矩阵 */
    };
}


#endif  /*__T3DX_RENDERER_H__*/
