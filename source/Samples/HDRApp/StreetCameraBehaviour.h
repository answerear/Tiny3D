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

#ifndef __STREET_CAMERA_BEHAVIOUR_H__
#define __STREET_CAMERA_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"
#include "Component/T3DCamera.h"


namespace Tiny3D
{
    /**
     * \brief Walks the camera along a closed loop of eye / target waypoints
     * \remarks Catmull-Rom through both eye and target, constant time per segment, so it never stops at a waypoint.
     */
    TCLASS()
    class StreetCameraBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~StreetCameraBehaviour() override = default;

        TPROPERTY(RTTRFuncName="SegmentSeconds", RTTRFuncType="getter")
        Real getSegmentSeconds() const { return mSegmentSeconds; }

        TPROPERTY(RTTRFuncName="SegmentSeconds", RTTRFuncType="setter")
        void setSegmentSeconds(Real seconds) { mSegmentSeconds = seconds; }

        void addWaypoint(const Vector3 &eye, const Vector3 &target);

    protected:
        StreetCameraBehaviour() = default;
        explicit StreetCameraBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;

    private:
        void apply();

        struct Waypoint
        {
            Vector3 eye;
            Vector3 target;
        };

        TArray<Waypoint> mWaypoints;
        Real mSegmentSeconds {5.0f};
        Real mElapsed {0.0f};
        CameraPtr mCamera {nullptr};
    };
}


#endif  /*__STREET_CAMERA_BEHAVIOUR_H__*/
