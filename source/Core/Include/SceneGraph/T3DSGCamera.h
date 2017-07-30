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

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SceneGraph/T3DSGNode.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCamera : public SGNode
    {
        friend class Viewport;
        friend class SceneManager;

    protected:
        SGCamera(uint32_t unID = E_NID_AUTOMATIC);

    public:
        enum ProjectionType
        {
            E_PT_ORTHOGRAPHIC = 0,
            E_PT_PERSPECTIVE,
        };

        static SGCameraPtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGCamera();

        virtual Type getNodeType() const override;

        void setProjectionType(ProjectionType eType);
        ProjectionType getProjectionType() const;

        void setAspectRatio(Real ratio);
        Real getAspectRatio() const;

        void setNearPlaneDistance(Real d);
        Real getNearPlaneDistance() const;

        void setFarPlaneDistance(Real d);
        Real getFarPlaneDistance() const;

        void setFovY(const Radian &radians);
        const Radian &getFovY() const;

        void setPerspective(const Radian &fovy, Real aspect, Real nearDist, Real farDist);

        const Matrix4 &getViewMatrix() const;
        const Matrix4 &getProjectionMatrix() const;

        virtual void setDirty(bool isDirty, bool recursive /* = false */) override;
        virtual NodePtr clone() const override;

        BoundPtr getBound() const   { return mBound; }

    protected:
        virtual void addChild(const NodePtr &child) override;

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual void updateTransform() override;

        virtual void updateBound();

        void renderScene(const ViewportPtr &viewport);

    protected:
        Radian  mFovY;
        Real    mFarDistance;
        Real    mNearDistance;
        Real    mAspectRatio;

        ProjectionType  mProjType;

        BoundPtr        mBound;

        mutable Matrix4 mViewMatrix;
        mutable Matrix4 mProjMatrix;

        mutable bool    mIsViewDirty;
        mutable bool    mIsFrustumDirty;
    };
}


#include "T3DSGCamera.inl"

#endif  /*__T3D_SG_CAMERA_H__*/

