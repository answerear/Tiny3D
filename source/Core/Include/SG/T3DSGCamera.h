

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SG/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCamera : public SGVisual
    {
        friend class Viewport;
        friend class SceneManager;

    protected:
        SGCamera(uint32_t unID = E_NID_AUTOMATIC);

    public:
        static SGCamera *create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGCamera();

        virtual Type getNodeType() const override;
        virtual void cloneProperties(SGNode *node) override;

        void setAspectRatio(Real ratio);
        Real getAspectRatio() const;

        void setNearPlaneDistance(Real d);
        Real getNearPlaneDistance() const;

        void setFarPlaneDistance(Real d);
        Real getFarPlaneDistance() const;

        void setFovX(Real x);
        Real getFovX() const;

        void setFoxY(Real y);
        Real getFovY() const;

        void setPerspective(Real fovx, Real ratio, Real near, Real far);

    private:    // from SGNode
        virtual void addChild(SGNode *child) override;

    protected:
        virtual void updateBound() override;

        void renderScene(Viewport *viewport);
    };
}

#endif  /*__T3D_SG_CAMERA_H__*/
