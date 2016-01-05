

#include "SG/Visual/T3DSGBox.h"
#include "SG/Renderable/T3DSGCube.h"


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
        SGNodePtr node = SGCube::create();

        SGNode *n = node;
        SGCube *cube = (SGCube *)n;
        mCube = cube;

        addChild(node);
    }

    SGBox::~SGBox()
    {
        mCube = nullptr;
    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    void SGBox::setMaxX(Real x)
    {
        if (mCube != nullptr)
        {
            
        }
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
