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


#include "T3DD3D11Sampler.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const D3D11SamplerCreator::SAMPLER_TYPE = "Sampler";

    //--------------------------------------------------------------------------

    String D3D11SamplerCreator::getType() const
    {
        return SAMPLER_TYPE;
    }

    //--------------------------------------------------------------------------

    SamplerPtr D3D11SamplerCreator::createObject(int32_t argc, ...) const
    {
        va_list params;
        va_start(params, argc);
        String name = va_arg(params, char *);
        va_end(params);
        return D3D11Sampler::create(name);
    }

    //--------------------------------------------------------------------------

    D3D11SamplerPtr D3D11Sampler::create(const String &name)
    {
        D3D11SamplerPtr sampler = new D3D11Sampler(name);
        sampler->release();
        return sampler;
    }

    //--------------------------------------------------------------------------

    D3D11Sampler::D3D11Sampler(const String &name)
        : Sampler(name)
        , mD3DSampler(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    D3D11Sampler::~D3D11Sampler()
    {
        D3D_SAFE_RELEASE(mD3DSampler);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Sampler::load()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Sampler::unload()
    {
        D3D_SAFE_RELEASE(mD3DSampler);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr D3D11Sampler::clone() const
    {
        D3D11SamplerPtr sampler = D3D11Sampler::create(getName());

        TResult ret = cloneProperties(sampler);
        if (ret != T3D_OK)
        {
            sampler = nullptr;
        }

        return sampler;
    }
}
