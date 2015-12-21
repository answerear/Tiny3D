

#include "SG/Visual/T3DSGCamera.h"
#include "SG/Node/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGCameraPtr SGCamera::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGCamera *camera = new SGCamera(unID);
        SGCameraPtr ptr(camera);
        camera->release();
        return ptr;
    }

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

    SGNodePtr SGCamera::clone() const
    {
        SGCamera *node = new SGCamera();
        SGNodePtr ptr(node);
        node->release();
        cloneProperties(node);
        return ptr;
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

    void SGCamera::addChild(const SGNodePtr &child)
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
            SGNode *parent = getParent();
            SGTransformNode *node = (SGTransformNode *)parent;
            T3D_ASSERT(node->getNodeType() == E_NT_TRANSFORM);

            const Transform &transform = node->getLocalToWorldTransform();
            mViewMatrix = transform.getAffineMatrix();
            mViewMatrix[3][0] = -mViewMatrix[3][0];
            mViewMatrix[3][1] = -mViewMatrix[3][1];
            mViewMatrix[3][2] = -mViewMatrix[3][2];
        }
        
        return mViewMatrix;
    }
}
