/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "CubeControllerBehaviour.h"

#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderWindow.h"
#include "T3DConfig.h"

#include <rttr/registration>


RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace Tiny3D;

    registration::class_<CubeControllerBehaviour>("CubeControllerBehaviour")
        .constructor<>()
        (
            policy::ctor::as_raw_ptr
        )
        .constructor<const Tiny3D::UUID &>()
        (
            policy::ctor::as_raw_ptr
        );

    type::register_wrapper_converter_for_base_classes<SmartPtr<CubeControllerBehaviour>>();

    registration::class_<SmartPtr<CubeControllerBehaviour>>("Tiny3D::SmartPtr<CubeControllerBehaviour>")
        .constructor<Object*>()
        (
            policy::ctor::as_object
        )
        .constructor<Object&>()
        (
            policy::ctor::as_object
        );
}


namespace Tiny3D
{
    CubeControllerBehaviour::CubeControllerBehaviour(const UUID &uuid)
        : Behaviour(uuid)
    {
    }

    void CubeControllerBehaviour::setCamera(Transform3D *cameraXform, Camera *camera)
    {
        mCameraXform = cameraXform;
        mCamera = camera;
        placeCameraBoom();
    }

    void CubeControllerBehaviour::onAwake()
    {
        mTransform = getGameObject()->getComponent<Transform3D>();
        if (mTransform != nullptr)
        {
            mWorldXform = static_cast<Transform3D *>(mTransform->getParent());
        }
        captureStartPose();
    }

    void CubeControllerBehaviour::captureStartPose()
    {
        if (mTransform != nullptr)
        {
            mInitialCubePos = mTransform->getPosition();
            mInitialCubeOrient = mTransform->getOrientation();
        }
        if (mWorldXform != nullptr)
        {
            mInitialWorldPos = mWorldXform->getPosition();
            mInitialWorldOrient = mWorldXform->getOrientation();
        }
        mInitialWorldPitchDeg = mWorldPitchDeg;
        mInitialWorldYawDeg = mWorldYawDeg;
        mInitialBoomDistance = mBoomDistance;
        mInitialCamPitchDeg = mPitchDeg;
    }

    void CubeControllerBehaviour::resetToStart()
    {
        if (mWorldXform != nullptr)
        {
            mWorldXform->setPosition(mInitialWorldPos);
            mWorldXform->setOrientation(mInitialWorldOrient);
        }
        mWorldPitchDeg = mInitialWorldPitchDeg;
        mWorldYawDeg = mInitialWorldYawDeg;

        if (mTransform != nullptr)
        {
            mTransform->setPosition(mInitialCubePos);
            mTransform->setOrientation(mInitialCubeOrient);
        }
        mVelocityY = 0.0f;
        mGrounded = true;

        mBoomDistance = mInitialBoomDistance;
        mPitchDeg = mInitialCamPitchDeg;
        mPendingTap = false;
        mIgnoreNextTapEnd = false;
        mDragging = false;
        mPinching = false;
    }

    void CubeControllerBehaviour::onUpdate()
    {
        if (mTransform == nullptr || Input::getInstancePtr() == nullptr)
        {
            return;
        }

        const Real dtSec = static_cast<Real>(Time::deltaTime()) / static_cast<Real>(1000.0f);

        if (T3D_INPUT.getKeyDown(APP_SCANCODE_R))
        {
            resetToStart();
        }
        else if (T3D_INPUT.getTouchCount() > 0)
        {
            handleTouch(dtSec);
        }
        else
        {
            handleKeyboardMouse(dtSec);
        }

        if (mPendingTap && (Time::unscaledTime() - mPendingTapTime) >= mDoubleTapWindowMs)
        {
            mPendingTap = false;
            moveToPlanePoint(mPendingTapPos);
        }

        updateJump(dtSec);
        placeCameraBoom();
    }

    void CubeControllerBehaviour::applyJump()
    {
        if (mGrounded)
        {
            mVelocityY = mJumpSpeed;
            mGrounded = false;
        }
    }

    void CubeControllerBehaviour::getPlaneMoveAxes(Vector3 &forward, Vector3 &right) const
    {
        Vector3 camForward = Vector3::UNIT_Z;
        Vector3 camRight = Vector3::UNIT_X;
        if (mCameraXform != nullptr)
        {
            const Quaternion &camRot = mCameraXform->getLocalToWorldTransform().getOrientation();
            camForward = camRot.zAxis();
            camRight = camRot.xAxis();
#if T3D_COORDINATION_RH
            camForward = -camForward;
#endif
        }

        Quaternion worldInv = Quaternion::IDENTITY;
        if (mWorldXform != nullptr)
        {
            worldInv = mWorldXform->getLocalToWorldTransform().getOrientation().inverse();
        }

        forward = worldInv * camForward;
        forward.y() = 0.0f;
        if (forward.normalize() <= 0.0f)
        {
            forward = Vector3::UNIT_Z;
        }

        right = worldInv * camRight;
        right.y() = 0.0f;
        if (right.normalize() <= 0.0f)
        {
            right = Vector3::UNIT_X;
        }
    }

    void CubeControllerBehaviour::rotateWorld(Real dx, Real dy)
    {
        if (mWorldXform == nullptr)
        {
            return;
        }

        mWorldYawDeg -= dx * mRotateSpeed;
        mWorldPitchDeg -= dy * mRotateSpeed;
        if (mWorldPitchDeg > mMaxWorldPitchDeg)
        {
            mWorldPitchDeg = mMaxWorldPitchDeg;
        }
        else if (mWorldPitchDeg < -mMaxWorldPitchDeg)
        {
            mWorldPitchDeg = -mMaxWorldPitchDeg;
        }

        mWorldXform->rotate(Degree(mWorldPitchDeg), Degree(mWorldYawDeg), Degree(0.0f));
    }

    void CubeControllerBehaviour::dollyCamera(Real delta)
    {
        if (Math::abs(delta) <= 0.0f)
        {
            return;
        }

        mBoomDistance -= delta;
        if (mBoomDistance < mMinBoom)
        {
            mBoomDistance = mMinBoom;
        }
        else if (mBoomDistance > mMaxBoom)
        {
            mBoomDistance = mMaxBoom;
        }
    }

    void CubeControllerBehaviour::placeCameraBoom()
    {
        if (mCameraXform == nullptr)
        {
            return;
        }

        Vector3 target = Vector3::ZERO;
        if (mTransform != nullptr)
        {
            target = mTransform->getLocalToWorldTransform().getTranslation();
        }

        const Degree pitch(mPitchDeg);
        Vector3 offset(0.0f, Math::sin(pitch) * mBoomDistance,
            -Math::cos(pitch) * mBoomDistance);
        const Vector3 eye = target + offset;

        mCameraXform->setPosition(eye);

        Matrix3 matR;
#if T3D_COORDINATION_RH
        matR.lookAt_RH(eye, target, Vector3::UNIT_Y);
#else
        matR.lookAt_LH(eye, target, Vector3::UNIT_Y);
#endif
        mCameraXform->setOrientation(Quaternion(matR));
    }

    bool CubeControllerBehaviour::screenPointToRay(const Vector2 &screenPos, Ray &outRay) const
    {
        if (mCamera == nullptr || mCameraXform == nullptr || Agent::getInstancePtr() == nullptr)
        {
            return false;
        }

        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        if (rw == nullptr)
        {
            return false;
        }

        const Real width = static_cast<Real>(rw->getDescriptor().Width);
        const Real height = static_cast<Real>(rw->getDescriptor().Height);
        if (width <= 0.0f || height <= 0.0f)
        {
            return false;
        }

        const Real ndcX = (screenPos.x() / width) * 2.0f - 1.0f;
        const Real ndcY = 1.0f - (screenPos.y() / height) * 2.0f;
        const Real tanHalfFov = Math::tan(mCamera->getFovY() * Real(0.5));
        const Real aspect = mCamera->getAspectRatio();

#if T3D_COORDINATION_RH
        Vector3 dirCam(ndcX * aspect * tanHalfFov, ndcY * tanHalfFov, -1.0f);
#else
        Vector3 dirCam(ndcX * aspect * tanHalfFov, ndcY * tanHalfFov, 1.0f);
#endif
        if (dirCam.normalize() <= 0.0f)
        {
            return false;
        }

        const Transform &world = mCameraXform->getLocalToWorldTransform();
        Vector3 origin = world.getTranslation();
        Vector3 dirWorld = world.getOrientation() * dirCam;
        if (dirWorld.normalize() <= 0.0f)
        {
            return false;
        }

        outRay.setOrigin(origin);
        outRay.setDirection(dirWorld * mCamera->getFarPlaneDistance());
        return true;
    }

    bool CubeControllerBehaviour::intersectGroundPlane(const Ray &ray, Vector3 &outHit) const
    {
        Vector3 origin = Vector3::ZERO;
        Vector3 normal = Vector3::UNIT_Y;
        if (mWorldXform != nullptr)
        {
            const Transform &world = mWorldXform->getLocalToWorldTransform();
            origin = world.getTranslation();
            normal = world.getOrientation() * Vector3::UNIT_Y;
            if (normal.normalize() <= 0.0f)
            {
                normal = Vector3::UNIT_Y;
            }
        }

        const Real denom = ray.getDirection().dot(normal);
        if (Math::abs(denom) < Real(1e-6))
        {
            return false;
        }

        const Real t = (origin - ray.getOrigin()).dot(normal) / denom;
        if (t < 0.0f || t > 1.0f)
        {
            return false;
        }

        outHit = ray.getOrigin() + ray.getDirection() * t;
        return true;
    }

    void CubeControllerBehaviour::moveToPlanePoint(const Vector2 &screenPos)
    {
        Ray ray;
        if (!screenPointToRay(screenPos, ray))
        {
            return;
        }

        Vector3 hit;
        if (!intersectGroundPlane(ray, hit))
        {
            return;
        }

        Vector3 local = hit;
        if (mWorldXform != nullptr)
        {
            const Matrix4 &worldMat = mWorldXform->getLocalToWorldTransform().getAffineMatrix();
            local = worldMat.inverse() * hit;
        }

        Vector3 pos = mTransform->getPosition();
        pos.x() = local.x();
        pos.z() = local.z();
        mTransform->setPosition(pos);
    }

    void CubeControllerBehaviour::updateJump(Real dtSec)
    {
        Vector3 pos = mTransform->getPosition();
        if (!mGrounded)
        {
            mVelocityY -= mGravity * dtSec;
            pos.y() += mVelocityY * dtSec;
            if (pos.y() <= mGroundY)
            {
                pos.y() = mGroundY;
                mVelocityY = 0.0f;
                mGrounded = true;
            }
            mTransform->setPosition(pos);
        }
        else if (pos.y() < mGroundY)
        {
            pos.y() = mGroundY;
            mTransform->setPosition(pos);
        }
    }

    void CubeControllerBehaviour::handleKeyboardMouse(Real dtSec)
    {
        if (T3D_INPUT.getMouseButton(MouseButton::Right))
        {
            const Vector2 delta = T3D_INPUT.getMouseDelta();
            rotateWorld(delta.x(), delta.y());
        }

        Vector3 forward;
        Vector3 right;
        getPlaneMoveAxes(forward, right);

        Vector3 move = Vector3::ZERO;
        if (T3D_INPUT.getKey(APP_SCANCODE_W))
        {
            move += forward;
        }
        if (T3D_INPUT.getKey(APP_SCANCODE_S))
        {
            move -= forward;
        }
        if (T3D_INPUT.getKey(APP_SCANCODE_D))
        {
            move += right;
        }
        if (T3D_INPUT.getKey(APP_SCANCODE_A))
        {
            move -= right;
        }
        if (move.normalize() > 0.0f)
        {
            mTransform->translate(move * (mMoveSpeed * dtSec));
        }

        if (T3D_INPUT.getKeyDown(APP_SCANCODE_SPACE))
        {
            applyJump();
        }

        const Vector2 scroll = T3D_INPUT.getMouseScrollDelta();
        dollyCamera(scroll.y() * 0.4f);
    }

    Real CubeControllerBehaviour::touchDistance() const
    {
        if (T3D_INPUT.getTouchCount() < 2)
        {
            return 0.0f;
        }
        const Touch a = T3D_INPUT.getTouch(0);
        const Touch b = T3D_INPUT.getTouch(1);
        const Vector2 d = a.position - b.position;
        return Math::sqrt(d.x() * d.x() + d.y() * d.y());
    }

    void CubeControllerBehaviour::handleTouch(Real dtSec)
    {
        (void)dtSec;
        const uint32_t count = T3D_INPUT.getTouchCount();
        if (count >= 2)
        {
            mPendingTap = false;
            mDragging = false;
            mIgnoreNextTapEnd = false;
            const Real dist = touchDistance();
            if (!mPinching)
            {
                mPinching = true;
                mLastPinchDist = dist;
            }
            else if (mLastPinchDist > 1.0f)
            {
                dollyCamera((dist - mLastPinchDist) * 0.02f);
                mLastPinchDist = dist;
            }
            return;
        }

        mPinching = false;
        mLastPinchDist = 0.0f;

        if (count != 1)
        {
            return;
        }

        const Touch touch = T3D_INPUT.getTouch(0);
        if (touch.phase == TouchPhase::Began)
        {
            const uint64_t now = Time::unscaledTime();
            mFingerDownPos = touch.position;
            mDragging = false;
            if (mPendingTap && (now - mPendingTapTime) <= mDoubleTapWindowMs)
            {
                mPendingTap = false;
                mIgnoreNextTapEnd = true;
                applyJump();
            }
        }
        else if (touch.phase == TouchPhase::Moved)
        {
            const Vector2 moved = touch.position - mFingerDownPos;
            const Real mag = Math::sqrt(moved.x() * moved.x() + moved.y() * moved.y());
            if (mDragging || mag > mTapMoveThreshold)
            {
                mDragging = true;
                mPendingTap = false;
                rotateWorld(touch.delta.x(), touch.delta.y());
            }
        }
        else if (touch.phase == TouchPhase::Ended)
        {
            if (mIgnoreNextTapEnd)
            {
                mIgnoreNextTapEnd = false;
            }
            else if (!mDragging)
            {
                mPendingTap = true;
                mPendingTapPos = touch.position;
                mPendingTapTime = Time::unscaledTime();
            }
            mDragging = false;
        }
        else if (touch.phase == TouchPhase::Canceled)
        {
            mPendingTap = false;
            mDragging = false;
            mIgnoreNextTapEnd = false;
        }
    }
}
