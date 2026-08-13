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

#ifndef __CUBE_CONTROLLER_BEHAVIOUR_H__
#define __CUBE_CONTROLLER_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DCamera.h"
#include "Kernel/T3DTime.h"
#include "Input/T3DInput.h"


namespace Tiny3D
{
    /**
     * \brief 立方体控制器：WASD 在平面上移动、右键旋转世界、滚轮缩放相机
     */
    TCLASS()
    class CubeControllerBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~CubeControllerBehaviour() override = default;

        void setCamera(Transform3D *cameraXform, Camera *camera);

    protected:
        CubeControllerBehaviour() = default;
        explicit CubeControllerBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;

    private:
        void applyJump();
        void captureStartPose();
        void resetToStart();
        void rotateWorld(Real dx, Real dy);
        void dollyCamera(Real delta);
        void placeCameraBoom();
        void getPlaneMoveAxes(Vector3 &forward, Vector3 &right) const;
        void moveToPlanePoint(const Vector2 &screenPos);
        bool screenPointToRay(const Vector2 &screenPos, Ray &outRay) const;
        bool intersectGroundPlane(const Ray &ray, Vector3 &outHit) const;
        void updateJump(Real dtSec);
        void handleKeyboardMouse(Real dtSec);
        void handleTouch(Real dtSec);
        Real touchDistance() const;

    private:
        Transform3DPtr  mTransform {nullptr};
        Transform3DPtr  mWorldXform {nullptr};
        Transform3DPtr  mCameraXform {nullptr};
        Camera         *mCamera {nullptr};

        Real    mMoveSpeed {6.0f};
        Real    mRotateSpeed {0.2f};
        Real    mJumpSpeed {6.0f};
        Real    mGravity {18.0f};
        Real    mGroundY {0.5f};
        Real    mVelocityY {0.0f};
        bool    mGrounded {true};

        Real    mWorldPitchDeg {0.0f};
        Real    mWorldYawDeg {0.0f};
        Real    mMaxWorldPitchDeg {80.0f};

        Real    mPitchDeg {20.0f};
        Real    mBoomDistance {8.0f};
        Real    mMinBoom {3.0f};
        Real    mMaxBoom {18.0f};

        Real    mTapMoveThreshold {20.0f};
        uint64_t mDoubleTapWindowMs {300};
        uint64_t mPendingTapTime {0};
        bool    mPendingTap {false};
        bool    mIgnoreNextTapEnd {false};
        Vector2 mPendingTapPos {0.0f, 0.0f};
        Vector2 mFingerDownPos {0.0f, 0.0f};
        bool    mDragging {false};
        Real    mLastPinchDist {0.0f};
        bool    mPinching {false};

        Vector3     mInitialCubePos {0.0f, 0.5f, 0.0f};
        Quaternion  mInitialCubeOrient {};
        Vector3     mInitialWorldPos {};
        Quaternion  mInitialWorldOrient {};
        Real        mInitialWorldPitchDeg {0.0f};
        Real        mInitialWorldYawDeg {0.0f};
        Real        mInitialBoomDistance {8.0f};
        Real        mInitialCamPitchDeg {20.0f};
    };
}


#endif  /*__CUBE_CONTROLLER_BEHAVIOUR_H__*/
