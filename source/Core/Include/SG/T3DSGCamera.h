

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SG/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCamera : public SGVisual
    {
        friend class Viewport;
        friend class SceneManager;

    public:
        enum ProjectionType
        {
            E_PT_ORTHOGRAPHIC = 0,
            E_PT_PERSPECTIVE,
        };

        SGCamera(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGCamera();

        virtual Type getNodeType() const override;
        virtual void cloneProperties(SGNode *node) override;

        void setProjectionType(ProjectionType eType);
        ProjectionType getProjectionType() const;

        void setAspectRatio(Real ratio);
        Real getAspectRatio() const;

        void setNearPlaneDistance(Real d);
        Real getNearPlaneDistance() const;

        void setFarPlaneDistance(Real d);
        Real getFarPlaneDistance() const;

        void setFovX(Real x);
        Real getFovX() const;

        void setFovY(Real y);
        Real getFovY() const;

        void setPerspective(Real fovx, Real ratio, Real near, Real far);

        const Matrix4 &getViewMatrix() const;
        const Matrix4 &getProjectionMatrix() const;

    private:    // from SGNode
        virtual void addChild(SGNode *child) override;

    protected:
        virtual void updateTransform() override;
        virtual void updateBound() override;

        void renderScene(Viewport *viewport);

    protected:
        Real    mFovX;
        Real    mFovY;
        Real    mFarDistance;
        Real    mNearDistance;
        Real    mAspectRatio;

        ProjectionType  mProjType;

        mutable Matrix4 mViewMatrix;
        Matrix4 mProjMatrix;
    };
}


#include "T3DSGCamera.inl"

#endif  /*__T3D_SG_CAMERA_H__*/

