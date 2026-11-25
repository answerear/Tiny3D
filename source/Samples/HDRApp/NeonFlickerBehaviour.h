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

#ifndef __NEON_FLICKER_BEHAVIOUR_H__
#define __NEON_FLICKER_BEHAVIOUR_H__


#include "Component/T3DBehaviour.h"


namespace Tiny3D
{
    /**
     * \brief Makes a neon sign stutter now and then: its emission and its point light drop together
     */
    TCLASS()
    class NeonFlickerBehaviour : public Behaviour
    {
        TRTTI_ENABLE(Behaviour)
        TRTTI_FRIEND

    public:
        ~NeonFlickerBehaviour() override = default;

        void setTargets(Material *material, const ColorRGBA &emission, PointLight *light, Real lightIntensity);

    protected:
        NeonFlickerBehaviour() = default;
        explicit NeonFlickerBehaviour(const UUID &uuid);

        void onUpdate() override;

    private:
        MaterialPtr mMaterial {nullptr};
        PointLightPtr mLight {nullptr};
        ColorRGBA mEmission {};
        Real mLightIntensity {1.0f};
        Real mElapsed {0.0f};
        Real mLastLevel {-1.0f};
    };
}


#endif  /*__NEON_FLICKER_BEHAVIOUR_H__*/
