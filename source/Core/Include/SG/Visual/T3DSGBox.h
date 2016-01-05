
#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGBox : public SGVisual
    {
    protected:
        SGBox(uint32_t unID = E_NID_AUTOMATIC);

    public:
        static SGNodePtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGBox();

        virtual SGNode::Type getNodeType() const override;

        void setMaxX(Real x);
        void setMinX(Real x);
        void setMaxY(Real y);
        void setMinY(Real y);
        void setMaxZ(Real z);
        void setMinZ(Real z);

    protected:
        virtual SGNodePtr clone() const override;

        SGCubePtr   mCube;
    };
}


#endif  /*__T3D_SG_BOX_H__*/
