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

    TCLASS()
    class T3D_ENGINE_API Renderable : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        ~Renderable() override = default;
        
        virtual Material *getMaterial() = 0;

        virtual PrimitiveType getPrimitiveType() const = 0;

        virtual VertexDeclaration *getVertexDeclaration() const = 0;

        virtual const VertexBuffers &getVertexBuffers() const = 0;

        virtual IndexBuffer *getIndexBuffer() const = 0;

        virtual const VertexStrides &getVertexStrides() const = 0;

        virtual const VertexOffsets &getVertexOffsets() const = 0;

        /**
         * \brief 获取用于视锥剔除的包围体（由 mesh 种子实例化时播种）
         * \return 剔除用 Bound；未设置返回 nullptr
         */
        Bound *getRenderBound() const { return mRenderBound; }

        /**
         * \brief 设置用于视锥剔除的包围体
         * \remarks 与该 Renderable 同属一个 GameObject，生命周期一致，故用裸指针。
         */
        void setRenderBound(Bound *bound) { mRenderBound = bound; }
        
    protected:
        Renderable() = default;

        Renderable(const UUID &uuid);

    protected:
        /// 视锥剔除用包围体（不持有所有权，由 GameObject 的组件表持有）
        Bound *mRenderBound {nullptr};
    };
}


#endif  /*__T3D_RENDERABLE_H__*/
