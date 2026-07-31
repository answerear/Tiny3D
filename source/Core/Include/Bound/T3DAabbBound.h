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


#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API AabbBound : public Bound
    {
         TRTTI_ENABLE(Bound)
         TRTTI_FRIEND

    public:
        static AabbBoundPtr create();

        ~AabbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        /// 世界空间包围盒，每帧由原始包围盒与变换矩阵重算，不反射也不序列化
        const Aabb &getAabb() const { return mAabb; }

        /// 局部空间包围盒
        const Aabb& getOriginalAabb() const { return mOriginalAabb; }

        // 整体反射 Aabb 会在 inspector 里多出一层可折叠子结构，与其余 bound 呈现不一致，
        // 因此拆成 Center / Size 两个 Vector3 直接平铺在组件面板上
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Box center in local space")
        const Vector3 &getCenter() const { return mOriginalAabb.getCenter(); }

        TPROPERTY(RTTRFuncName="Size", RTTRFuncType="getter", "Description"="Box size along the three axes in local space")
        Vector3 getSize() const
        {
            return Vector3(mOriginalAabb.getWidth(), mOriginalAabb.getHeight(),
                mOriginalAabb.getDepth());
        }

        TFUNCTION()
        void setParams(Real minX, Real maxX, Real minY, Real maxY, Real minZ, Real maxZ)
        {
            Aabb aabb;
            aabb.setParam(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
            setOriginalAabb(aabb);
        }

        Renderable *getRenderable() override;
        
    protected:
        AabbBound() = default;

        AabbBound(const UUID &uuid);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testCapsule(const Capsule &capsule) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;
        
        void update();

        void onUpdate() override;

    private:
        void setOriginalAabb(const Aabb &aabb)
        {
            mOriginalAabb = aabb;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界包围盒
            mAabb = aabb;
        }

        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Box center in local space")
        void setCenter(const Vector3 &center)
        {
            resetOriginalAabb(center, getSize());
        }

        // Size 是由 min/max 算出来的临时值，getter 只能按值返回，因此 setter 也得按值
        // 收参——RTTR 要求同一属性的 getter 与 setter 数据类型完全一致
        TPROPERTY(RTTRFuncName="Size", RTTRFuncType="setter", "Description"="Box size along the three axes in local space")
        void setSize(Vector3 size)
        {
            resetOriginalAabb(getCenter(), size);
        }

        void resetOriginalAabb(const Vector3 &center, const Vector3 &size)
        {
            const Vector3 half = size * REAL_HALF;
            Aabb aabb;
            aabb.setParam(center - half, center + half);
            setOriginalAabb(aabb);
        }

    private:
        /// 实时变换的AABB（世界空间），由 update() 每帧重算，不参与序列化
        Aabb    mAabb {};
        /// 不参与变换的原始AABB（局部空间）
        Aabb    mOriginalAabb {};
    };
}

#endif  /*__T3D_AABB_BOUND_H__*/
