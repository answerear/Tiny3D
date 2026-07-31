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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API ObbBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static ObbBoundPtr create();

        ~ObbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        /// 世界空间 OBB，每帧由原始 OBB 与变换矩阵重算，不反射也不序列化
        const Obb &getObb() const { return mObb; }

        /// 局部空间 OBB
        const Obb& getOriginalObb() const { return mOriginalObb; }

        // 整体反射 Obb 会在 inspector 里多出一层可折叠子结构，与其余 bound 呈现不一致，
        // 因此拆成中心、三条轴向与半长直接平铺在组件面板上
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Box center in local space")
        const Vector3 &getCenter() const { return mOriginalObb.getCenter(); }

        TPROPERTY(RTTRFuncName="XAxis", RTTRFuncType="getter", "Description"="First axis direction in local space")
        const Vector3 &getXAxis() const { return mOriginalObb.getAxis(0); }

        TPROPERTY(RTTRFuncName="YAxis", RTTRFuncType="getter", "Description"="Second axis direction in local space")
        const Vector3 &getYAxis() const { return mOriginalObb.getAxis(1); }

        TPROPERTY(RTTRFuncName="ZAxis", RTTRFuncType="getter", "Description"="Third axis direction in local space")
        const Vector3 &getZAxis() const { return mOriginalObb.getAxis(2); }

        TPROPERTY(RTTRFuncName="Extents", RTTRFuncType="getter", "Description"="Half extents along the three axes in local space")
        Vector3 getExtents() const
        {
            return Vector3(mOriginalObb.getExtent(0), mOriginalObb.getExtent(1),
                mOriginalObb.getExtent(2));
        }

        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Box center in local space")
        void setCenter(const Vector3 &center)
        {
            mObb.setCenter(center);
            mOriginalObb.setCenter(center);
        }

        TFUNCTION()
        void setAxis(const Vector3 &axis0, const Vector3 &axis1, const Vector3 &axis2)
        {
            mObb.setAxis(axis0, axis1, axis2);
            mOriginalObb.setAxis(axis0, axis1, axis2);
        }

        TFUNCTION()
        void setExtent(Real extent0, Real extent1, Real extent2)
        {
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);

            mOriginalObb.setExtent(0, extent0);
            mOriginalObb.setExtent(1, extent1);
            mOriginalObb.setExtent(2, extent2);
        }

        Renderable *getRenderable() override;

    protected:
        ObbBound() = default;

        ObbBound(const UUID &uuid);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testCapsule(const Capsule &capsule) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;
        
        void update();

        void onUpdate() override;

    private:
        void setOriginalObb(const Obb &obb)
        {
            mOriginalObb = obb;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界 OBB
            mObb = obb;
        }

        // 三条轴向按整组写入，因此单独改一条时要先把另外两条取值拷出来，
        // 不能直接把 getXxxAxis() 的引用传回去——那是即将被覆盖的同一块内存
        TPROPERTY(RTTRFuncName="XAxis", RTTRFuncType="setter", "Description"="First axis direction in local space")
        void setXAxis(const Vector3 &axis)
        {
            const Vector3 y = getYAxis();
            const Vector3 z = getZAxis();
            setAxis(axis, y, z);
        }

        TPROPERTY(RTTRFuncName="YAxis", RTTRFuncType="setter", "Description"="Second axis direction in local space")
        void setYAxis(const Vector3 &axis)
        {
            const Vector3 x = getXAxis();
            const Vector3 z = getZAxis();
            setAxis(x, axis, z);
        }

        TPROPERTY(RTTRFuncName="ZAxis", RTTRFuncType="setter", "Description"="Third axis direction in local space")
        void setZAxis(const Vector3 &axis)
        {
            const Vector3 x = getXAxis();
            const Vector3 y = getYAxis();
            setAxis(x, y, axis);
        }

        // Extents 是由三个轴向半长拼出来的临时值，getter 只能按值返回，因此 setter 也得
        // 按值收参——RTTR 要求同一属性的 getter 与 setter 数据类型完全一致
        TPROPERTY(RTTRFuncName="Extents", RTTRFuncType="setter", "Description"="Half extents along the three axes in local space")
        void setExtents(Vector3 extents)
        {
            setExtent(extents.x(), extents.y(), extents.z());
        }
        
    private:
        /// 实时变换的OBB
        Obb mObb {};
        /// 不参与变换的原始OBB
        Obb mOriginalObb {};
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
