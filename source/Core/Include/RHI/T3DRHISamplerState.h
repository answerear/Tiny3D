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


#ifndef __T3D_RHI_SAMPLER_STATE_H__
#define __T3D_RHI_SAMPLER_STATE_H__


#include "T3DPrerequisites.h"
#include "T3DRHIConstant.h"


namespace Tiny3D
{
    /**
     * \brief 纹理采样状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API SamplerState
    {
        uint8_t *getCRCData(uint32_t &dataSize) const
        {
            dataSize = sizeof(SamplerState) - sizeof(void*);
            return (uint8_t*)(&MinFilter);
        }

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

    /**
     * \brief 渲染硬件层的纹理采样状态
     */
    class T3D_ENGINE_API RHISamplerState : public Object
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }

        const SamplerState &getState() const
        {
            return mState;
        }

    protected:
        RHISamplerState(const SamplerState &state);

        uint32_t        mHash = 0;
        SamplerState    mState {};
    };
}


#endif  /*__T3D_RHI_SAMPLER_STATE_H__*/
