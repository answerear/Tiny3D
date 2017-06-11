


#include "SG/Visual/T3DSGVisual.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "SG/Visual/T3DSGCamera.h"
#include "SG/T3DSceneManager.h"
#include "Misc/T3DEntrance.h"


namespace Tiny3D
{
    SGVisual::SGVisual(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
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

    void SGVisual::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (mIsVisible)
        {
            SGNode::frustumCulling(bound, queue);
        }
    }

    void SGVisual::cloneProperties(const NodePtr &node) const
    {
        SGNode::cloneProperties(node);
        const SGVisualPtr &newNode = (const SGVisualPtr &)node;
        newNode->mIsVisible = mIsVisible;
    }
}
