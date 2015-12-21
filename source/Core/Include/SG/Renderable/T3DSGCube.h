
#ifndef __T3D_SG_CUBE_H__
#define __T3D_SG_CUBE_H__


#include "SG/Renderable/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCube : public SGRenderable
    {
    public:
        static SGNodePtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGCube();

        virtual SGNode::Type getNodeType() const override;

    protected:
        SGCube(uint32_t unID = E_NID_AUTOMATIC);

        virtual SGNodePtr clone() const override;
        virtual void cloneProperties(SGNode *node) const override;

    protected:
        Real    mMinX;
        Real    mMaxX;
        Real    mMinY;
        Real    mMaxY;
        Real    mMinZ;
        Real    mMaxZ;
    };
}


#endif  /*__T3D_SG_CUBE_H__*/
