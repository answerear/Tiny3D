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


#include "T3DD3D11Context.h"
#include "T3DD3D11Window.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ContextPtr D3D11Context::create()
    {
        D3D11ContextPtr renderer = new D3D11Context();
        // renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    D3D11Context::D3D11Context()
        : mInstance(nullptr)
    {
        
    }

    //--------------------------------------------------------------------------

    D3D11Context::~D3D11Context()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::renderAllTargets()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = RHIContext::renderAllTargets();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D11Context::makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const
    {
        return Matrix4::IDENTITY;
    }

    Matrix4 D3D11Context::makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist)
    {
        return Matrix4::IDENTITY;
    }

    Matrix4 D3D11Context::makeViewportMatrix(Viewport *viewport)
    {
        return Matrix4::IDENTITY;
    }

    TResult D3D11Context::clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    RHIBlendStatePtr D3D11Context::createBlendState(const BlendState &state)
    {
        return nullptr;
    }

    RHIDepthStencilStatePtr D3D11Context::createDepthStencilState(const DepthStencilState &state)
    {
        return nullptr;
    }

    RHIRasterizerStatePtr D3D11Context::createRasterizerState(const RasterizerState &state)
    {
        return nullptr;
    }

    RHISamplerStatePtr D3D11Context::createSamplerState(const SamplerState &state)
    {
        return nullptr;
    }

    TResult D3D11Context::setBlendState(RHIBlendStatePtr state)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setDepthStencilState(RHIDepthStencilStatePtr state)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setRasterizerState(RHIRasterizerStatePtr state)
    {
        return T3D_OK;
    }
        
    TResult D3D11Context::setSamplerState(RHISamplerStatePtr state)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setViewport(Viewport *viewport)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setShader()
    {
        return T3D_OK;
    }

    TResult D3D11Context::setTexture()
    {
        return T3D_OK;
    }

    TResult D3D11Context::renderObject()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
}

