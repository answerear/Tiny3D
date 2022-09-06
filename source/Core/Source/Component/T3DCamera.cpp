/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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
 ******************************************************************************/


#include "Component/T3DCamera.h"
#include "Component/T3DTransform3D.h"
#include "Scene/T3DSceneNode.h"
#include "Render/T3DViewport.h"
#include "Render/T3DRenderContext.h"
#include "Kernel/T3DAgent.h"
#include "Bound/T3DFrustumBound.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    CameraPtr Camera::create(ID uID /* = E_CID_AUTOMATIC */)
    {
        CameraPtr camera = new Camera(uID);
        camera->release();

        if (camera->init() != T3D_OK)
        {
            camera = nullptr;
        }

        return camera;
    }

    //--------------------------------------------------------------------------

    Camera::Camera(ID uID /* = E_CID_AUTOMATIC */)
        : Component(uID)
        , mBound(nullptr)
        , mProjType(Type::PERSPECTIVE)
        , mObjectMask(0)
        , mFovY(Math::PI * REAL_HALF)
        , mAspectRatio(16.0f/9.0f)
        , mWidth(REAL_ZERO)
        , mHeight(REAL_ZERO)
        , mNear(REAL_ZERO)
        , mFar(REAL_ZERO)
        , mViewMatrix(false)
        , mProjMatrix(false)
        , mIsViewDirty(true)
        , mIsFrustumDirty(true)
    {

    }

    //--------------------------------------------------------------------------

    Camera::~Camera()
    {
        mBound = nullptr;
    }
    
    //--------------------------------------------------------------------------

    TResult Camera::init()
    {
        TResult ret = T3D_OK;

        mBound = FrustumBound::create();

        return ret;
    }

    //--------------------------------------------------------------------------

    void Camera::lookAt(const Vector3 &pos, const Vector3 &obj, 
        const Vector3 &up)
    {
        SceneNode *node = getSceneNode();
        T3D_ASSERT(node->getTransform3D() != nullptr);

        Transform3D *xform = node->getTransform3D();

        Vector3 N = obj - pos;
        N.normalize();
        Vector3 V = up;
        V.normalize();
        Vector3 U = N.cross(V);
        U.normalize();
        V = U.cross(N);
        V.normalize();

        xform->setPosition(pos);

        Matrix3 mat;
        mat.setColumn(0, U);
        mat.setColumn(1, V);
        mat.setColumn(2, -N);
        Quaternion orientation;
        orientation.fromRotationMatrix(mat);
        xform->setOrientation(orientation);

        xform->setScaling(Vector3::UNIT_SCALE);
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getViewMatrix() const
    {
        SceneNode *node = getSceneNode();
        Transform3D *xform = node->getTransform3D();
        const Transform &transform = xform->getLocalToWorldTransform();

        if (mIsViewDirty)
        {
            // 视图矩阵推导：
            // 其中C是相机进行世界变换的矩阵，
            //  T是平移变换
            //  R是旋转变换
            //  S是缩放变换
            //
            // 由 C = T * R * S
            // 得 C(-1) = (T * R * S) (-1) = S(-1) * R(-1) * T(-1)
            // 

            // 旋转矩阵
            Matrix4 R = transform.getOrientation();
            // 旋转矩阵是正交矩阵，正交矩阵的逆矩阵是其转置矩阵
            Matrix4 invertR = R.transpose();
            // 平移矩阵
            Matrix4 invertT(false);
            invertT.makeTranslate(-transform.getTranslation());
            // 缩放矩阵
            Matrix4 invertS(false);
            const Vector3 &scale = transform.getScaling();
            invertS[0][0] = REAL_ONE / scale.x();
            invertS[1][1] = REAL_ONE / scale.y();
            invertS[2][2] = REAL_ONE / scale.z();

            mViewMatrix = invertS * invertR * invertT;

            RenderContextPtr renderer = T3D_AGENT.getActiveRenderer();
            renderer->setViewTransform(mViewMatrix);

            mIsViewDirty = false;
        }

        return mViewMatrix;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getProjectionMatrix() const
    {
        if (mIsFrustumDirty)
        {
            RenderContextPtr renderer = T3D_AGENT.getActiveRenderer();

            switch (mProjType)
            {
            case Type::ORTHOGRAPHIC:
                {
                    mProjMatrix = renderer->orthographic(mWidth, mHeight, 
                        mNear, mFar);
                }
                break;
            case Type::PERSPECTIVE:
            default:
                {
                    mProjMatrix = renderer->perspective(mFovY, mAspectRatio, 
                        mNear, mFar);
                }
                break;
            }

            renderer->setProjectionTransform(mProjMatrix);

            mIsFrustumDirty = false;
        }

        return mProjMatrix;
    }

    //--------------------------------------------------------------------------

    ComponentPtr Camera::clone() const
    {
        CameraPtr camera = create();
        
        if (cloneProperties(camera) != T3D_OK)
        {
            camera = nullptr;
        }

        return camera;
    }

    //--------------------------------------------------------------------------

    TResult Camera::cloneProperties(ComponentPtr newObj) const
    {
        TResult ret = Component::cloneProperties(newObj);

        if (ret == T3D_OK)
        {
            CameraPtr camera = smart_pointer_cast<Camera>(newObj);
            camera->mProjType = mProjType;
            camera->mObjectMask = mObjectMask;
            camera->mFovY = mFovY;
            camera->mAspectRatio = mAspectRatio;
            camera->mNear = mNear;
            camera->mFar = mFar;
            camera->mViewMatrix = mViewMatrix;
            camera->mProjMatrix = mProjMatrix;
            camera->mIsViewDirty = mIsViewDirty;
            camera->mIsFrustumDirty = mIsFrustumDirty;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Camera::update()
    {
        bool isViewDirty = mIsViewDirty;
        bool isFrustumDirty = mIsFrustumDirty;

        const Matrix4 &V = getViewMatrix();
        const Matrix4 &P = getProjectionMatrix();

        if (isViewDirty || isFrustumDirty)
        {
            // 相机比较特殊，直接先更新自身的frustum，
            // 避免其他物体无法做frustum culling
            Matrix4 M = P * V;
            RenderContext *renderer = T3D_AGENT.getActiveRenderer();
            FrustumBoundPtr bound = smart_pointer_cast<FrustumBound>(mBound);
            renderer->updateFrustum(M, bound);
        }
    }

    //--------------------------------------------------------------------------

    void Camera::updateTransform(const Transform3D *xform)
    {
        mIsViewDirty = true;
    }
}

//------------------------------------------------------------------------------
//                                  RTTR
//------------------------------------------------------------------------------

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Tiny3D::Camera>("Tiny3D::Camera")
        .enumeration<Tiny3D::Camera::Type>("Tiny3D::Camera::Type")
        (
            value("ORTHOGRAPHIC", Tiny3D::Camera::Type::ORTHOGRAPHIC),
            value("PERSPECTIVE", Tiny3D::Camera::Type::PERSPECTIVE)
        )
        .constructor(&Tiny3D::Camera::create)
        .method("clone", &Tiny3D::Camera::clone)
        .method("lookAt", &Tiny3D::Camera::lookAt)
        .method("setObjectMask", &Tiny3D::Camera::setObjectMask)
        .method("getObjectMask", &Tiny3D::Camera::getObjectMask)
        .method("setProjectionType", &Tiny3D::Camera::setProjectionType)
        .method("getProjectionType", &Tiny3D::Camera::getProjectionType)
        .method("setPerspectiveParams", &Tiny3D::Camera::setPerspectiveParams)
        .method("setOrthographicParams", &Tiny3D::Camera::setOrthographicParams)
        .method("getAspectRatio", &Tiny3D::Camera::getAspectRatio)
        .method("getFovY", &Tiny3D::Camera::getFovY)
        .method("getNearPlaneDistance", &Tiny3D::Camera::getNearPlaneDistance)
        .method("getFarPlaneDistance", &Tiny3D::Camera::getFarPlaneDistance)
        .method("getBound", &Tiny3D::Camera::getBound)
        .method("getViewMatrix", &Tiny3D::Camera::getViewMatrix)
        .method("getProjectionMatrix", &Tiny3D::Camera::getProjectionMatrix)
        .method("update", &Tiny3D::Camera::update)
        .property("ProjectionType", &Tiny3D::Camera::mProjType, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The projection type. It could be orthographic or perspective.")
        )
        .property("ObjectMask", &Tiny3D::Camera::mObjectMask, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The object masks that this camera could render.")
        )
        .property("FovY", &Tiny3D::Camera::mFovY, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The field of view in perspective camera.")
        )
        .property("AspectRatio", &Tiny3D::Camera::mAspectRatio, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The aspect ratio of view in perspective camera.")
        )
        .property("Width", &Tiny3D::Camera::mWidth, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The width of view in orthographic camera.")
        )
        .property("Height", &Tiny3D::Camera::mHeight, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The height of view in orthographic camera.")
        )
        .property("Near", &Tiny3D::Camera::mNear, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The near plane of view.")
        )
        .property("Far", &Tiny3D::Camera::mFar, registration::protected_access)
        (
            metadata(TXT_DESCRIPTION, "The far plane of view.")
        );
}

