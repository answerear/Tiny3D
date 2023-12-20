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


#ifndef __T3D_SAMPLER_STATE_H__
#define __T3D_SAMPLER_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 纹理采样状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API SamplerDesc
    {
        TPROPERTY()
        FilterOptions       MinFilter;
        
        TPROPERTY()
        FilterOptions       MagFilter;
        
        TPROPERTY()
        FilterOptions       MipFilter;
        
        TPROPERTY()
        TextureAddressMode  AddressU;
        
        TPROPERTY()
        TextureAddressMode  AddressV;
        
        TPROPERTY()
        TextureAddressMode  AddressW;
        
        TPROPERTY()
        Real                MipLODBias;
        
        TPROPERTY()
        uint32_t            MaxAnisotropy;
        
        TPROPERTY()
        CompareFunction     CompareFunc;
        
        TPROPERTY()
        ColorRGBA           BorderColor;
        
        TPROPERTY()
        Real                MinLOD;
        
        TPROPERTY()
        Real                MaxLOD;
    };

    class T3D_ENGINE_API SamplerState : public RenderStateResource<SamplerDesc, RHISamplerState>
    {
    public:
        static SamplerStatePtr create(const SamplerDesc &desc, uint32_t hash = 0);
        
        Type getType() const override;
        
    protected:
        SamplerState(uint32_t hash, const SamplerDesc &desc);

        ~SamplerState() override = default;
        
        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_SAMPLER_STATE_H__*/
