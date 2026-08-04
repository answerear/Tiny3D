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

#ifndef __T3D_RENDERABLE_H__
#define __T3D_RENDERABLE_H__


#include "Component/T3DComponent.h"
#include "Kernel/T3DConstant.h"

namespace Tiny3D
{
    class Bound;

    /**
     * \brief 可渲染组件抽象基类，向渲染管线暴露顶点/索引/材质等绘制数据
     */
    TCLASS()
    class T3D_ENGINE_API Renderable : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        /// 析构
        ~Renderable() override = default;

        /**
         * \brief enabled 开关对本组件有效
         * \return 恒返回 true；关闭后不再提交渲染
         */
        bool supportsEnabled() const override { return true; }

        /**
         * \brief 返回渲染使用的材质
         * \return 材质指针；子类未就绪时可能为 nullptr
         */
        virtual Material *getMaterial() = 0;

        /**
         * \brief 返回图元类型
         * \return 子类据 mesh/submesh 决定；未就绪时子类可返回默认值
         */
        virtual PrimitiveType getPrimitiveType() const = 0;

        /**
         * \brief 返回顶点声明
         * \return 顶点声明指针；mesh 未加载时可能为 nullptr
         */
        virtual VertexDeclaration *getVertexDeclaration() const = 0;

        /**
         * \brief 返回顶点缓冲集合
         * \return 顶点缓冲引用；mesh 未加载时子类可返回空静态容器
         */
        virtual const VertexBuffers &getVertexBuffers() const = 0;

        /**
         * \brief 返回索引缓冲
         * \return 索引缓冲指针；mesh/submesh 未就绪时可能为 nullptr
         */
        virtual IndexBuffer *getIndexBuffer() const = 0;

        /**
         * \brief 返回各顶点流 stride
         * \return stride 集合；mesh 未加载时子类可返回空静态容器
         */
        virtual const VertexStrides &getVertexStrides() const = 0;

        /**
         * \brief 返回各顶点流 offset
         * \return offset 集合；mesh 未加载时子类可返回空静态容器
         */
        virtual const VertexOffsets &getVertexOffsets() const = 0;

        /**
         * \brief 获取视锥剔除用包围体
         * \return 剔除用 Bound 指针；未设置时返回 nullptr
         */
        Bound *getRenderBound() const { return mRenderBound; }

        /**
         * \brief 设置视锥剔除用包围体
         * \param [in] bound : 与 Renderable 同属一个 GameObject 的 Bound，不持有所有权
         */
        void setRenderBound(Bound *bound) { mRenderBound = bound; }
        
    protected:
        /// 默认构造
        Renderable() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        Renderable(const UUID &uuid);

    protected:
        /// 视锥剔除用包围体（不持有所有权，由 GameObject 组件表持有）
        Bound *mRenderBound {nullptr};
    };
}


#endif  /*__T3D_RENDERABLE_H__*/
