/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __T3D_SG_TRANSFORM_NODE_H__
#define __T3D_SG_TRANSFORM_NODE_H__


#include "SceneGraph/T3DSGNode.h"
#include "T3DMath.h"
#include "T3DVector3.h"
#include "T3DMatrix3.h"
#include "T3DMatrix4.h"
#include "T3DTransform.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGTransformNode : public SGNode
    {
    protected:
        SGTransformNode(uint32_t unID = E_NID_AUTOMATIC);

    public:
        static SGTransformNodePtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGTransformNode();

        virtual Type getNodeType() const override;

        void setPosition(const Vector3 &rkPos);
        void setPosition(Real x, Real y, Real z);
        const Vector3 &getPosition() const;

        void setOrientation(const Quaternion &rkQ);
        void setOrientation(Real w, Real x, Real y, Real z);
        void setOrientation(const Radian &radian, const Vector3 &axis);
        const Quaternion &getOrientation() const;

        void setScale(const Vector3 &rkScale);
        void setScale(Real x, Real y, Real z);
        const Vector3 &getScale() const;

        void translate(const Vector3 &rkOffset);
        void translate(Real x, Real y, Real z);
        void translate(const Matrix3 &axis, const Vector3 &rkOffset);
        void translate(const Matrix3 &axis, Real x, Real y, Real z);

        void rotate(const Quaternion &rkQ);
        void rotate(const Vector3 &axis, const Radian &radians);
        void rotate(const Vector3 &axis, const Degree &degrees);

        void pitch(const Degree &degrees);
        void yaw(const Degree &degrees);
        void roll(const Degree &degrees);

        void pitch(const Radian &radians);
        void yaw(const Radian &radians);
        void roll(const Radian &radians);

        void scale(const Vector3 &rkScale);
        void scale(Real x, Real y, Real z);

        void setLocalMatrix(const Matrix4 &m);

        /**
         * @brief This method only be used by SGCamera for child.
         * @param [in] pos : 
         * @param [in] obj : 
         * @param [in] up : 
         */
        void lookAt(const Vector3 &pos, const Vector3 &obj, const Vector3 &up);

        virtual const Transform &getLocalToWorldTransform();
//         Transform getWorldToLocalTransform();
        Transform getLocalTransform() const;

        virtual void addChild(const NodePtr &node) override;

        virtual NodePtr clone() const override;

    protected:
        virtual void onAttachParent(const NodePtr &parent) override;
        virtual void onDetachParent(const NodePtr &parent) override;

        virtual void updateTransform() override;

        virtual void cloneProperties(const NodePtr &node) const override;

    private:
        Vector3     mPosition;
        Quaternion  mOrientation;
        Vector3     mScale;

        Transform   mWorldTransform;
    };
}


#include "T3DSGTransformNode.inl"


#endif  /*__T3D_SG_TRANSFORM_NODE_H__*/
