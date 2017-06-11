

#ifndef __T3D_SG_VISUAL_H__
#define __T3D_SG_VISUAL_H__


#include "SG/Node/T3DSGTransformNode.h"
#include "Bound/T3DBound.h"
#include "T3DTransform.h"


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

        virtual void cloneProperties(const NodePtr &node) const override;

    protected:
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

    protected:
        bool    mIsVisible;
    };
}


#include "T3DSGVisual.inl"


#endif  /*__T3D_SG_VISUAL_H__*/
