/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************************************/

#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph/T3DSGTransformNode.h"
#include "SceneGraph/T3DSGRenderable.h"
#include "T3DMath.h"
#include "T3DMatrix4.h"
#include "Misc/T3DEntrance.h"
#include "Render/T3DRenderer.h"
#include "T3DPlane.h"
#include "T3DFrustum.h"
#include "Bound/T3DFrustumBound.h"


namespace Tiny3D
{
    SGCameraPtr SGCamera::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGCameraPtr camera = new SGCamera(unID);
        camera->release();
        return camera;
    }

    SGCamera::SGCamera(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
        , mFovY(Real(45.0))
        , mFarDistance(Real(2000.0))
        , mNearDistance(Real(1.0))
        , mAspectRatio(Real(4.0)/Real(3.0))
        , mProjType(E_PT_PERSPECTIVE)
        , mIsFrustumDirty(false)
    {
        mBound = FrustumBound::create(unID, this);
    }

    SGCamera::~SGCamera()
    {

    }

    Node::Type SGCamera::getNodeType() const
    {
        return E_NT_CAMERA;
    }

    NodePtr SGCamera::clone() const
    {
        SGCameraPtr camera = create();
        cloneProperties(camera);
        return camera;
    }

    void SGCamera::cloneProperties(const NodePtr &node) const
    {
        SGNode::cloneProperties(node);

        const SGCameraPtr &newNode = smart_pointer_cast<SGCamera>(node);
        newNode->mFovY = mFovY;
        newNode->mFarDistance = mFarDistance;
        newNode->mNearDistance = mNearDistance;
        newNode->mAspectRatio = mAspectRatio;
        newNode->mProjType = mProjType;
        newNode->mViewMatrix = mViewMatrix;
        newNode->mProjMatrix = mProjMatrix;
    }

    void SGCamera::addChild(const NodePtr &child)
    {
        // Can't add any type node for child node.
        T3D_ASSERT(0);
    }

    void SGCamera::updateTransform()
    {
        bool isViewDirty = mIsViewDirty;
        bool isFrustumDirty = mIsFrustumDirty;

        getViewMatrix();
        getProjectionMatrix();

        // 相机比较特殊，直接先更新自身的frustum，避免其他物体无法做frustum culling
        if (isViewDirty || isFrustumDirty)
        {
            updateBound();
        }

        SGNode::updateTransform();
    }

    void SGCamera::setDirty(bool isDirty, bool recursive /* = false */)
    {
        SGNode::setDirty(isDirty, recursive);
        mIsViewDirty = isDirty;
    }

    void SGCamera::renderScene(const ViewportPtr &viewport)
    {
        T3D_SCENE_MGR.renderScene(this, viewport);
    }

    const Matrix4 &SGCamera::getViewMatrix() const
    {
        if (mIsViewDirty)
        {
            // 视图矩阵推导：
            // 其中C是相机进行世界变换的矩阵，T是平移变换，R是旋转变换，S是缩放变换
            // 由 C = T * R * S
            // 得 C(-1) = (T * R * S) (-1) = S(-1) * R(-1) * T(-1)
            // 
            Node *parent = getParent();
            SGTransformNode *node = (SGTransformNode *)parent;
            T3D_ASSERT(node->getNodeType() == E_NT_TRANSFORM);

            const Transform &transform = node->getLocalToWorldTransform();
            // 旋转矩阵
            Matrix4 R = transform.getOrientation();
            /// 旋转矩阵是正交矩阵，正交矩阵的逆矩阵是其转置矩阵
            Matrix4 invertR = R.transpose();
            // 平移矩阵
            Matrix4 invertT(false);
            invertT.makeTranslate(-transform.getTranslate());
            // 缩放矩阵
            Matrix4 invertS(false);
            const Vector3 &scale = transform.getScale();
            invertS[0][0] = Real(1.0) / scale.x();
            invertS[1][1] = Real(1.0) / scale.y();
            invertS[2][2] = Real(1.0) / scale.z();

            mViewMatrix = invertS * invertR * invertT;

            Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
            renderer->setViewTransform(mViewMatrix);

            mIsViewDirty = false;
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

            renderer->setProjectionTransform(mProjMatrix);

            mIsFrustumDirty = false;
        }

        return mProjMatrix;
    }

    void SGCamera::updateBound()
    {
        Matrix4 M = mProjMatrix * mViewMatrix;
        Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
        Plane plane[Frustum::E_MAX_FACE];

        renderer->updateFrustum(M, plane, Frustum::E_MAX_FACE);

        FrustumBoundPtr bound = smart_pointer_cast<FrustumBound>(mBound);
        bound->setParam(plane, Frustum::E_MAX_FACE);
    }
}
