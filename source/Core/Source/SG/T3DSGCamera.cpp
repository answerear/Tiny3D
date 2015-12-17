

#include "SG/T3DSGCamera.h"
#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGCamera::SGCamera(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGVisual(unID)
    {

    }

    SGCamera::~SGCamera()
    {

    }

    SGNode::Type SGCamera::getNodeType() const
    {
        return E_NT_CAMERA;
    }

    SGNode *SGCamera::clone() const
    {
        SGCamera *node = new SGCamera();
        cloneProperties(node);
        return node;
    }

    void SGCamera::cloneProperties(SGNode *node) const
    {
        SGVisual::cloneProperties(node);

        SGCamera *newNode = (SGCamera *)node;
        newNode->mFovX = mFovX;
        newNode->mFovY = mFovY;
        newNode->mFarDistance = mFarDistance;
        newNode->mNearDistance = mNearDistance;
        newNode->mAspectRatio = mAspectRatio;
        newNode->mProjType = mProjType;
        newNode->mViewMatrix = mViewMatrix;
        newNode->mProjMatrix = mProjMatrix;
    }

    void SGCamera::addChild(SGNode *child)
    {
        // Can't add any type node for child node.
        T3D_ASSERT(0);
    }

    void SGCamera::updateTransform()
    {
        getViewMatrix();
        SGVisual::updateTransform();
    }

    void SGCamera::updateBound()
    {

    }

    void SGCamera::renderScene(Viewport *viewport)
    {

    }

    const Matrix4 &SGCamera::getViewMatrix() const
    {
        if (isDirty())
        {
            SGTransformNode *node = (SGTransformNode *)getParent();
            T3D_ASSERT(node->getNodeType() == E_NT_TRANSFORM);

            const Transform &transform = node->getLocalToWorldTransform();
            mViewMatrix = transform.getAffineMatrix();
        }
        
        return mViewMatrix;
    }
}
