

#include "SG/Visual/T3DSGCamera.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "Math/T3DMath.h"


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
        , mIsFrustumDirty(false)
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
            mViewMatrix[0][3] = -mViewMatrix[0][3];
            mViewMatrix[1][3] = -mViewMatrix[1][3];
            mViewMatrix[2][3] = -mViewMatrix[2][3];
            mViewMatrix[3][3] = Real(1.0);
        }
        
        return mViewMatrix;
    }

    const Matrix4 &SGCamera::getProjectionMatrix() const
    {
        if (mIsFrustumDirty)
        {
            if (mProjType == E_PT_ORTHOGRAPHIC)
            {
                Radian thetaY = mFovY / Real(2.0);
                Real sinThetaY = Math::Sin(thetaY);
                Radian thetaX = thetaY * mAspectRatio;
                Real sinThetaX = Math::Sin(thetaX);
                Real w = 1.0 / (sinThetaX * mNearDistance);
                Real h = 1.0 / (sinThetaY * mNearDistance);
                Real q = 1.0 / (mFarDistance - mNearDistance);

                mProjMatrix.makeZero();
                mProjMatrix[0][0] = w;
                mProjMatrix[1][1] = h;
                mProjMatrix[2][2] = -q;
                mProjMatrix[3][3] = 1.0;
            }
            else
            {

            }

            mIsFrustumDirty = false;
        }

        return mProjMatrix;
    }
}
