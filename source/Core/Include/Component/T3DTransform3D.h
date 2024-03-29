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

#ifndef __T3D_TRANSFORM3D_H__
#define __T3D_TRANSFORM3D_H__


#include "Component/T3DTransformNode.h"
#include "Kernel/T3DTransform.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Transform3D : public TransformNode
    {
        TRTTI_ENABLE(TransformNode)
        TRTTI_FRIEND
        
    public:
        static Transform3DPtr create();

        ~Transform3D() override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="Position", RTTRFuncType="setter")
        void setPosition(const Vector3 &pos);

        void setPosition(Real x, Real y, Real z);

        TPROPERTY(RTTRFuncName="Position", RTTRFuncType="getter")
        const Vector3 &getPosition() const;

        TPROPERTY(RTTRFuncName="Orientation",  RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation);

        void setOrientation(Real w, Real x, Real y, Real z);

        void setOrientation(const Radian &radian, const Vector3 &axis);

        TPROPERTY(RTTRFuncName="Orientation",  RTTRFuncType="getter")
        const Quaternion &getOrientation() const;

        TPROPERTY(RTTRFuncName="Scaling",  RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling);

        void setScaling(Real x, Real y, Real z);

        TPROPERTY(RTTRFuncName="Scaling",  RTTRFuncType="getter")
        const Vector3 &getScaling() const;

        void translate(const Vector3 &offset);

        void translate(Real x, Real y, Real z);

        void translate(const Vector3 &dir, Real step);

        void rotate(const Quaternion &orientation);

        void rotate(const Vector3 &axis, const Radian &radians);

        void rotate(const Vector3 &axis, const Degree &degrees);

        void pitch(const Degree &degrees);

        void pitch(const Radian &radians);

        void yaw(const Degree &degrees);

        void yaw(const Radian &radians);

        void roll(const Degree &degrees);

        void roll(const Radian &radians);

        void scale(const Vector3 &scaling);

        void scale(Real x, Real y, Real z);

        void setLocalMatrix(const Matrix4 &m);

        virtual const Transform &getLocalToWorldTransform() const;

        Transform getLocalTransform() const;

        void update() override;

        virtual void setDirty(bool isDirty, bool recursive = false);

        bool isDirty() const;
        
    protected:
        Transform3D();

        TResult cloneProperties(const Component * const src) override;

        void onAttachParent(TransformNodePtr parent) override;

        void onDetachParent(TransformNodePtr parent) override;
        
        void onDestroy() override;

    private:
        /// 父节点坐标系下的局部位置
        Vector3             mPosition {};
        ///  父节点坐标系下的局部朝向
        Quaternion          mOrientation {};
        /// 父节点坐标系下的局部大小
        Vector3             mScaling {REAL_ONE, REAL_ONE, REAL_ONE};

        /// 从局部到世界的变换对象
        mutable Transform   mWorldTransform {};
        
        /// 结点数据脏标记，脏时需要重新计算
        mutable bool        mIsDirty {false};
    };
}


#include "T3DTransform3D.inl"


#endif    /*__T3D_TRANSFORM3D_H__*/
