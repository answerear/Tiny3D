

#ifndef __T3D_SG_SHAPE_H__
#define __T3D_SG_SHAPE_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGShape : public SGVisual
    {
    public:
        static SGShapePtr create(uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGShape();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

    protected:
        SGShape(uint32_t uID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;
    };
}


#endif  /*__T3D_SG_SHAPE_H__*/
