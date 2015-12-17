
#ifndef __T3D_SG_CUBE_H__
#define __T3D_SG_CUBE_H__


#include "SG/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCube : public SGRenderable
    {
    public:
        SGCube(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGCube();

        virtual SGNode::Type getNodeType() const override;

    protected:
        virtual SGNode *clone() const override;
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
