
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
        SGCube(uint32_t unID = E_NID_AUTOMATIC);

        virtual SGNodePtr clone() const override;
        virtual void cloneProperties(SGNode *node) const override;

        virtual void render() override;

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
