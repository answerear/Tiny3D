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


#include "Light/T3DAmbientLight.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AmbientLightPtr AmbientLight::create()
    {
        return new AmbientLight(UUID::generate());
    }
    
    //--------------------------------------------------------------------------

    AmbientLight::AmbientLight(const UUID &uuid)
        : IndirectLight(uuid)
    {
        
    }

    //--------------------------------------------------------------------------

    ComponentPtr AmbientLight::clone() const
    {
        AmbientLightPtr light = create();

        if (light != nullptr)
        {
            TResult ret = light->cloneProperties(this);
            
            if (T3D_FAILED(ret))
            {
                light = nullptr;
            }
        }
        
        return light;
    }
    
    //--------------------------------------------------------------------------

    TResult AmbientLight::cloneProperties(const Component *const src)
    {
        TResult ret = Light::cloneProperties(src);
        
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        const AmbientLight *srcLight = static_cast<const AmbientLight *>(src);
        mIntensity = srcLight->getIntensity();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
