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


#include "StreetCameraBehaviour.h"
#include "Kernel/T3DTime.h"


namespace Tiny3D
{
    namespace
    {
        Vector3 catmullRom(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, Real t)
        {
            const Real t2 = t * t;
            const Real t3 = t2 * t;
            return (p1 * 2.0f
                + (p2 - p0) * t
                + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2
                + (p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
        }
    }

    //--------------------------------------------------------------------------

    StreetCameraBehaviour::StreetCameraBehaviour(const UUID &uuid)
        : Behaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    void StreetCameraBehaviour::addWaypoint(const Vector3 &eye, const Vector3 &target)
    {
        mWaypoints.push_back({eye, target});
    }

    //--------------------------------------------------------------------------

    void StreetCameraBehaviour::onAwake()
    {
        mCamera = getGameObject()->getComponent<Camera>();
        apply();
    }

    //--------------------------------------------------------------------------

    void StreetCameraBehaviour::onUpdate()
    {
        // Time::deltaTime() is milliseconds
        mElapsed += static_cast<Real>(Time::deltaTime()) / 1000.0f;
        apply();
    }

    //--------------------------------------------------------------------------

    void StreetCameraBehaviour::apply()
    {
        const size_t count = mWaypoints.size();
        if (mCamera == nullptr || count < 2 || mSegmentSeconds <= REAL_ZERO)
        {
            return;
        }

        const Real loopSeconds = mSegmentSeconds * static_cast<Real>(count);
        mElapsed = std::fmod(mElapsed, loopSeconds);
        const Real segment = mElapsed / mSegmentSeconds;
        const size_t i1 = static_cast<size_t>(segment) % count;
        const Real t = segment - std::floor(segment);
        const size_t i0 = (i1 + count - 1) % count;
        const size_t i2 = (i1 + 1) % count;
        const size_t i3 = (i1 + 2) % count;

        const Vector3 eye = catmullRom(mWaypoints[i0].eye, mWaypoints[i1].eye, mWaypoints[i2].eye, mWaypoints[i3].eye, t);
        const Vector3 target = catmullRom(mWaypoints[i0].target, mWaypoints[i1].target, mWaypoints[i2].target, mWaypoints[i3].target, t);
        mCamera->lookAt(eye, target, Vector3::UP);
    }
}
