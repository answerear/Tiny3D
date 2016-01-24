


#include "SG/Visual/T3DSGVisual.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "SG/Visual/T3DSGCamera.h"
#include "SG/T3DSceneManager.h"
#include "Misc/T3DEntrance.h"


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
        mBound = nullptr;
    }

    void SGVisual::setVisible(bool visible)
    {
        mIsVisible = visible;
    }

    void SGVisual::updateBound()
    {
        SGNode *node = getParent();
        SGTransformNode *parent = (SGTransformNode *)node;

        if (isDirty())
        {
            mWorldTransform = parent->getLocalToWorldTransform();

            if (mBound != nullptr)
            {
                mBound->setTransform(mWorldTransform);
            }

            setDirty(false);
        }
    }

    void SGVisual::updateTransform()
    {
        // update bound
        T3D_ASSERT(getParent()->getNodeType() == SGNode::E_NT_TRANSFORM);

        updateBound();

        SGNode::updateTransform();
        // check this node in frustum
//         if (getNodeType() != SGNode::E_NT_CAMERA)
//         {
//             // 不是相机结点，才需要判断是否可见，是否需要渲染
//             SGCamera *camera = T3D_SCENE_MGR.getCurCamera();
// 
//             BoundPtr bound = camera->getBound();
// 
//             if (bound != nullptr)
//             {
//                 mIsInFrustum = bound->test(mBound);
//             }
// 
//             // continue updating all children node
//             SGNode::updateTransform();
//         }
    }

    void SGVisual::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (mIsVisible)
        {
            if (bound != nullptr && mBound != nullptr)
            {
                mIsInFrustum = bound->test(mBound);
            }
            
            if (mIsInFrustum)
            {
                SGNode::frustumCulling(bound, queue);
            }
        }
    }

    void SGVisual::cloneProperties(const SGNodePtr &node) const
    {
        SGNode::cloneProperties(node);

        const SGVisualPtr &newNode = (const SGVisualPtr &)node;
        newNode->mWorldTransform = mWorldTransform;

        if (mBound != nullptr)
        {
            newNode->mBound = mBound->clone();
        }
        else
        {
            newNode->mBound = nullptr;
        }

        newNode->mIsVisible = mIsVisible;
    }
}
