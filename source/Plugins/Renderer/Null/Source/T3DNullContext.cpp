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


#include "T3DNullContext.h"
#include "T3DNullWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullContextPtr NullContext::create()
    {
        NullContextPtr renderer = new NullContext();
        // renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    NullContext::NullContext()
    {
        
    }

    //--------------------------------------------------------------------------

    NullContext::~NullContext()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult NullContext::renderAllTargets()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = RHIContext::renderAllTargets();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    Matrix4 NullContext::makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const
    {
        return Matrix4::IDENTITY;
    }

    Matrix4 NullContext::makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist)
    {
        return Matrix4::IDENTITY;
    }

    Matrix4 NullContext::makeViewportMatrix(Viewport *viewport)
    {
        return Matrix4::IDENTITY;
    }

    TResult NullContext::clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    RHIBlendStatePtr NullContext::createBlendState(const BlendDesc &desc)
    {
        return nullptr;
    }

    RHIDepthStencilStatePtr NullContext::createDepthStencilState(const DepthStencilDesc &desc)
    {
        return nullptr;
    }

    RHIRasterizerStatePtr NullContext::createRasterizerState(const RasterizerDesc &desc)
    {
        return nullptr;
    }

    RHISamplerStatePtr NullContext::createSamplerState(const SamplerDesc &desc)
    {
        return nullptr;
    }

    TResult NullContext::setBlendState(RHIBlendStatePtr state)
    {
        return T3D_OK;
    }

    TResult NullContext::setDepthStencilState(RHIDepthStencilStatePtr state)
    {
        return T3D_OK;
    }

    TResult NullContext::setRasterizerState(RHIRasterizerStatePtr state)
    {
        return T3D_OK;
    }
        
    TResult NullContext::setSamplerState(RHISamplerStatePtr state)
    {
        return T3D_OK;
    }

    TResult NullContext::setViewport(Viewport *viewport)
    {
        return T3D_OK;
    }

    TResult NullContext::setShader()
    {
        return T3D_OK;
    }

    TResult NullContext::setTexture()
    {
        return T3D_OK;
    }

    TResult NullContext::renderObject()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
}

