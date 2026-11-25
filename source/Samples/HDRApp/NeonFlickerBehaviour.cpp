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


#include "NeonFlickerBehaviour.h"
#include "Light/T3DPointLight.h"
#include "Kernel/T3DTime.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    namespace
    {
        // Brightness per 60 ms step inside one 5.4 s cycle; the stutter sits at the end
        const Real kPattern[] =
        {
            1.0f, 0.1f, 1.0f, 1.0f, 0.05f, 0.05f, 0.6f, 0.1f, 1.0f, 0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        };
        const Real kStepSeconds = 0.06f;
        const Real kCycleSeconds = 5.4f;
    }

    //--------------------------------------------------------------------------

    NeonFlickerBehaviour::NeonFlickerBehaviour(const UUID &uuid)
        : Behaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    void NeonFlickerBehaviour::setTargets(Material *material, const ColorRGBA &emission, PointLight *light, Real lightIntensity)
    {
        mMaterial = material;
        mEmission = emission;
        mLight = light;
        mLightIntensity = lightIntensity;
        mLastLevel = -1.0f;
    }

    //--------------------------------------------------------------------------

    void NeonFlickerBehaviour::onUpdate()
    {
        mElapsed = std::fmod(mElapsed + static_cast<Real>(Time::deltaTime()) / 1000.0f, kCycleSeconds);

        const size_t steps = sizeof(kPattern) / sizeof(kPattern[0]);
        const Real stutterStart = kCycleSeconds - kStepSeconds * static_cast<Real>(steps);
        Real level = 1.0f;
        if (mElapsed >= stutterStart)
        {
            const size_t step = static_cast<size_t>((mElapsed - stutterStart) / kStepSeconds);
            level = kPattern[step < steps ? step : steps - 1];
        }

        if (level == mLastLevel)
        {
            return;
        }
        mLastLevel = level;

        if (mMaterial != nullptr)
        {
            ColorRGBA emission = mEmission;
            emission.alpha() = mEmission.alpha() * level;
            mMaterial->setColor("hdr_EmissionColor", emission);
        }
        if (mLight != nullptr)
        {
            mLight->setDiffuseIntensity(mLightIntensity * level);
            mLight->setSpecularIntensity(mLightIntensity * level);
        }
    }
}
