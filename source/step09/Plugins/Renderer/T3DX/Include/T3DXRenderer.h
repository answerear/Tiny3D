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


#ifndef __T3DX_RENDERER_H__
#define __T3DX_RENDERER_H__


#include "T3DXPrerequisites.h"


namespace Tiny3D
{
    class T3DXRenderer : public Renderer
    {
    public:
        static T3DXRendererPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~T3DXRenderer();

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
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult beginRender() override;

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
         * @brief 开启或者关闭某种能力
         * @param [in] cap : 能力枚举值
         * @param [in] enabled : 开关标记
         * @return 调用成功返回 T3D_OK
         * @remarks 如果具体渲染系统本身不具备相应的能力，则本接口调用无效
         * @see Renderer::Capability
         */
        virtual TResult enableCapability(Capability cap, bool enabled) override;

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
         * @param [in] fovY : 纵向的视角角度
         * @param [in] aspect : 宽高比
         * @param [in] zNear : 观察空间的近平面
         * @param [in] zFar : 观察空间的远平面
         * @return 返回一个平台相关的透视投影矩阵
         * @remarks 因为投影矩阵在不同渲染平台在NDC坐标系里z轴上会有的范围，
         *      因此，这里矩阵的获取通过不同渲染平台直接计算出来，屏蔽差异。
         *      Direct3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Open GL : x∈[-1, 1], y∈[-1, 1], z∈[-1, 1]
         *      Vulkan : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Metal : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         *      Reference3D : x∈[-1, 1], y∈[-1, 1], z∈[0, 1]
         */
        virtual Matrix4 perspective(Real fovY, Real aspect, Real zNear,
            Real zFar) override;

        /**
         * @brief 获取正交投影矩阵
         * @param [in] left : 观察空间的左边
         * @param [in] right : 观察空间的右边
         * @param [in] top : 观察空间的上边
         * @param [in] bottom : 观察空间的下边
         * @param [in] zNear : 观察空间的近平面
         * @param [in] zFar : 观察空间的远平面
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
            Real bottom, Real zNear, Real zFar) override;

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
         * @brief 绘制顶点数组
         * @param [in] vao : 顶点数组对象
         * @return 调动成功返回 T3D_OK
         */
        virtual TResult drawVertexArray(VertexArrayPtr vao) override;

        /**
         * @brief 绘制顶点列表
         * @param [in] priType : 图元类型
         * @param [in] vbo : 顶点缓冲
         * @param [in] startIdx : 顶点缓冲区的起始位置
         * @param [in] priCount : 图元数量
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult drawVertexList(PrimitiveType priType,
            HardwareVertexBufferPtr vbo, size_t startIdx, 
            size_t priCount) override;

        /**
        * @brief 绘制索引列表
        * @param [in] priType : 图元类型
        * @param [in] vbo : 顶点缓冲
        * @param [in] ibo : 索引缓冲
        * @param [in] startIdx : 顶点索引起始位置偏移
        * @param [in] priCount : 图元数量
        * @return 调用成功返回 T3D_OK
        */
        virtual TResult drawIndexList(PrimitiveType priType,
            HardwareVertexBufferPtr vbo, HardwareIndexBufferPtr ibo,
            size_t startIdx, size_t priCount) override;

    protected:
        /**
         * @brief 构造函数
         */
        T3DXRenderer();

        T3DXRenderWindowPtr     mRenderWindow;  /**< 渲染窗口 */
    };
}


#endif  /*__T3DX_RENDERER_H__*/
