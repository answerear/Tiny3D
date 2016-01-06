

#ifndef __T3D_SG_VISUAL_H__
#define __T3D_SG_VISUAL_H__


#include "SG/Node/T3DSGTransformNode.h"
#include "Bound/T3DBound.h"
#include "Math/T3DTransform.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGVisual : public SGNode
    {
    protected:
        SGVisual(uint32_t unID = E_NID_AUTOMATIC);

    public:
        virtual ~SGVisual();

        virtual void setVisible(bool visible);
        bool isVisible() const;

        Bound *getBound();

        void setBoundEnabled(bool enable);
        bool isBoundEnabled() const;

        bool inFrustum() const;

        const Transform &getWorldTransform() const;

        virtual void cloneProperties(SGNode *node) const override;

    protected:
        virtual void updateTransform() override;
        virtual void updateBound();

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

    private:
        Bound   *mBound;

        Transform mWorldTransform;

        bool    mIsInFrustum;
        bool    mIsVisible;
    };
}


#include "T3DSGVisual.inl"


#endif  /*__T3D_SG_VISUAL_H__*/
