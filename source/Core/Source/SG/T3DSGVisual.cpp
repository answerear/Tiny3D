


#include "SG/T3DSGVisual.h"
#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGVisual::SGVisual(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mBound(nullptr)
        , mIsVisible(true)
    {

    }

    SGVisual::~SGVisual()
    {

    }

    void SGVisual::setVisible(bool visible)
    {
        mIsVisible = visible;
    }

    void SGVisual::updateBound()
    {
        if (isDirty())
        {
            SGTransformNode *parent = (SGTransformNode *)getParent();

            if (mBound != nullptr)
            {
                const Matrix4 &mat = parent->getLocalToWorldTransform();
            }
        }
    }

    void SGVisual::update()
    {
        // update bound
        T3D_ASSERT(getParent()->getNodeType() == SGNode::E_NT_TRANSFORM);

        updateBound();

        // continue updating all children node
        SGNode::update();
    }

    void SGVisual::cloneProperties(SGNode *node)
    {
        SGNode::cloneProperties(node);

        SGVisual *src = (SGVisual *)node;
        mIsVisible = src->mIsVisible;
    }
}
