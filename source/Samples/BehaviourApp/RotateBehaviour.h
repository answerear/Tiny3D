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

#ifndef __ROTATE_BEHAVIOUR_H__
#define __ROTATE_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"
#include "Component/T3DTransform3D.h"
#include "Kernel/T3DTime.h"


namespace Tiny3D
{
    /**
     * \brief Sample Behaviour that spins a GameObject around Y.
     * \remarks
     *   Engine is left-handed: when looking from +Y toward -Y, positive yaw is
     *   clockwise. setClockwise(true) uses +speed, false uses -speed.
     */
    TCLASS()
    class RotateBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~RotateBehaviour() override = default;

        void setClockwise(bool clockwise) { mClockwise = clockwise; }
        bool isClockwise() const { return mClockwise; }

        void setAngularSpeedDeg(Real degPerSec) { mAngularSpeedDeg = degPerSec; }
        Real getAngularSpeedDeg() const { return mAngularSpeedDeg; }

    protected:
        RotateBehaviour() = default;
        explicit RotateBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;

    private:
        Real mAngularSpeedDeg {90.0f};
        bool mClockwise {true};
        Transform3DPtr mTransform {nullptr};
    };
}


#endif  /*__ROTATE_BEHAVIOUR_H__*/
