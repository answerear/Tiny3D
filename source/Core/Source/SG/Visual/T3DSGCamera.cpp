

#include "SG/Visual/T3DSGCamera.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "Math/T3DMath.h"
#include "Math/T3DMatrix4.h"
#include "Misc/T3DEntrance.h"
#include "Render/T3DRenderer.h"


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
        , mFovY(Real(45.0))
        , mFarDistance(Real(2000.0))
        , mNearDistance(Real(1.0))
        , mAspectRatio(Real(4.0)/Real(3.0))
        , mProjType(E_PT_PERSPECTIVE)
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
        getProjectionMatrix();

        SGVisual::updateTransform();
    }

    void SGCamera::updateBound()
    {

    }

    void SGCamera::renderScene(const ViewportPtr &viewport)
    {
        T3D_SCENE_MGR.renderScene(this, viewport);
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

            Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
            renderer->setTransform(Renderer::E_TS_VIEW, mViewMatrix);
        }
        
        return mViewMatrix;
    }

    const Matrix4 &SGCamera::getProjectionMatrix() const
    {
        if (mIsFrustumDirty)
        {
            Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();

            if (mProjType == E_PT_ORTHOGRAPHIC)
            {
                renderer->makeProjectionMatrix(mFovY, mAspectRatio, mNearDistance, mFarDistance, true, mProjMatrix);
            }
            else
            {
                renderer->makeProjectionMatrix(mFovY, mAspectRatio, mNearDistance, mFarDistance, false, mProjMatrix);
            }

            renderer->setTransform(Renderer::E_TS_PROJECTION, mProjMatrix);

            mIsFrustumDirty = false;
        }

        return mProjMatrix;
    }
}
