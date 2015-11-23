


#include "SG/T3DSGVisual.h"


namespace Tiny3D
{
    SGVisual::SGVisual(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mIsVisible(true)
        , mBoundEnabled(true)
    {

    }

    SGVisual::~SGVisual()
    {

    }

    void SGVisual::setVisible(bool visible)
    {
        mIsVisible = visible;
    }

    void SGVisual::update()
    {
        SGNode::update();
        updateBound();
    }

    void SGVisual::cloneProperties(SGNode *node)
    {
        SGNode::cloneProperties(node);

        SGVisual *src = (SGVisual *)node;
        mIsVisible = src->mIsVisible;
        mBoundEnabled = src->mBoundEnabled;
    }
}
