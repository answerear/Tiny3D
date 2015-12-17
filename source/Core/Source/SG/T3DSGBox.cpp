

#include "SG/T3DSGBox.h"


namespace Tiny3D
{
    SGBox::SGBox(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGVisual(unID)
    {

    }

    SGBox::~SGBox()
    {

    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    SGNode *SGBox::clone() const
    {
        SGBox *node = new SGBox();
        cloneProperties(node);
        return node;
    }
}
