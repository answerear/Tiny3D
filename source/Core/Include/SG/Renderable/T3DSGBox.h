
#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SG/Renderable/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGBox : public SGRenderable
    {
    public:
        static SGBoxPtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGBox();

        virtual Type getNodeType() const override;

        void setMinX(Real x);
        void setMaxX(Real x);
        void setMinY(Real y);
        void setMaxY(Real y);
        void setMinZ(Real z);
        void setMaxZ(Real z);

        Real getMinX() const;
        Real getMaxX() const;
        Real getMinY() const;
        Real getMaxY() const;
        Real getMinZ() const;
        Real getMaxZ() const;

    protected:
        SGBox(uint32_t unID = E_NID_AUTOMATIC);

        virtual SGNodePtr clone() const override;
        virtual void cloneProperties(SGNode *node) const override;

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        static const uint32_t MAX_FACE_NUM;
        static const uint32_t MAX_VERTICES_NUM;
        static const uint32_t INDICES_PER_FACE;
    };
}


#endif  /*__T3D_SG_BOX_H__*/
