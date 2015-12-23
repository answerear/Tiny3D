

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SG/Visual/T3DSGVisual.h"
#include "Misc/T3DSmartPointer.h"


namespace Tiny3D
{
    typedef SmartPtr<SGCamera> SGCameraPtr;

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

        void setFovX(const Radian &radians);
        const Radian &getFovX() const;
        Radian &getFovX();

        void setFovY(const Radian &radians);
        const Radian &getFovY() const;
        Radian &getFovY();

        void setPerspective(Real fovx, Real ratio, Real near, Real far);

        const Matrix4 &getViewMatrix() const;
        const Matrix4 &getProjectionMatrix() const;

    private:    // from SGNode
        virtual void addChild(const SGNodePtr &child) override;

        virtual SGNodePtr clone() const override;
        virtual void cloneProperties(SGNode *node) const override;

    protected:
        virtual void updateTransform() override;
        virtual void updateBound() override;

        void renderScene(Viewport *viewport);

    protected:
        Radian  mFovX;
        Radian  mFovY;
        Real    mFarDistance;
        Real    mNearDistance;
        Real    mAspectRatio;

        ProjectionType  mProjType;

        mutable Matrix4 mViewMatrix;
        mutable Matrix4 mProjMatrix;

        mutable bool    mIsFrustumDirty;
    };
}


#include "T3DSGCamera.inl"

#endif  /*__T3D_SG_CAMERA_H__*/

