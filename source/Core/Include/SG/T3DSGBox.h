
#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SG/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGBox : public SGVisual
    {
    public:
        SGBox(uint32_t unID = E_NID_AUTOMATIC);
        virtual ~SGBox();

        virtual SGNode::Type getNodeType() const override;

    protected:
        virtual SGNode *clone() const override;
    };
}


#endif  /*__T3D_SG_BOX_H__*/
