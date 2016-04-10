

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCamera : public SGVisual
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

    protected:
        virtual void addChild(const NodePtr &child) override;

        virtual void cloneProperties(const NodePtr &node) const override;

        virtual void updateTransform() override;

        virtual void updateBound() override;

        void renderScene(const ViewportPtr &viewport);

    protected:
        Radian  mFovY;
        Real    mFarDistance;
        Real    mNearDistance;
        Real    mAspectRatio;

        ProjectionType  mProjType;

        mutable Matrix4 mViewMatrix;
        mutable Matrix4 mProjMatrix;

        mutable bool    mIsViewDirty;
        mutable bool    mIsFrustumDirty;
    };
}


#include "T3DSGCamera.inl"

#endif  /*__T3D_SG_CAMERA_H__*/

