

#include "SG/Visual/T3DSGBox.h"


namespace Tiny3D
{
    SGNodePtr SGBox::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGBox *box = new SGBox(unID);
        SGNodePtr ptr(box);
        box->release();
        return ptr;
    }
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

    SGNodePtr SGBox::clone() const
    {
        SGBox *node = new SGBox();
        SGNodePtr ptr(node);
        node->release();
        cloneProperties(node);
        return ptr;
    }
}
