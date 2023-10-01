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


#ifndef __T3D_NULL_CONTEXT_H__
#define __T3D_NULL_CONTEXT_H__


#include "T3DNullPrerequisites.h"


namespace Tiny3D
{
    class NullContext : public RHIContext
    {
    public:
        static NullContextPtr create();

        virtual ~NullContext();
     
        TResult renderAllTargets() override;

        Matrix4 makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const override;

        Matrix4 makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist) override;

        Matrix4 makeViewportMatrix(Viewport *viewport) override;

        TResult clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil) override;

        RHIBlendStatePtr createBlendState(const BlendDesc &desc) override;

        RHIDepthStencilStatePtr createDepthStencilState(const DepthStencilDesc &desc) override;

        RHIRasterizerStatePtr createRasterizerState(const RasterizerDesc &desc) override;

        RHISamplerStatePtr createSamplerState(const SamplerDesc &desc) override;

        TResult setBlendState(RHIBlendStatePtr state) override;

        TResult setDepthStencilState(RHIDepthStencilStatePtr state) override;

        TResult setRasterizerState(RHIRasterizerStatePtr state) override;
        
        TResult setSamplerState(RHISamplerStatePtr state) override;

        TResult setViewport(Viewport *viewport) override;

        TResult setShader() override;

        TResult setTexture() override;

        TResult renderObject() override;
        
    protected:
        NullContext();

        TResult collectInformation();
    };
}


#endif  /*__T3D_NULL_CONTEXT_H__*/
