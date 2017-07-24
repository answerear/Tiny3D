

#include "SG/Node/T3DSGNode.h"
#include "SG/T3DRenderQueue.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    SGNode::SGNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : Node(unID)
        , mUserData(0)
        , mUserObject(nullptr)
        , mIsDirty(true)
    {

    }

    SGNode::~SGNode()
    {

    }

    void SGNode::updateTransform()
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
            node->updateTransform();
            ++itr;
        }
    }

    void SGNode::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
            node->frustumCulling(bound, queue);
            ++itr;
        }
    }

    void SGNode::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
                node->setDirty(isDirty, recursive);
                ++itr;
            }
        }
    }

    void SGNode::cloneProperties(const NodePtr &node) const
    {
        Node::cloneProperties(node);

        const SGNodePtr &newNode = smart_pointer_cast<SGNode>(node);
        newNode->mUserData = mUserData;
        newNode->mUserObject = mUserObject;
    }

    void SGNode::setVisible(bool visible)
    {
        mIsVisible = visible;
    }
}
