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


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"
#include "Kernel/T3DCommon.h"


namespace Tiny3D
{
    /**
     * @brief 采样状态
     */
    class T3D_ENGINE_API SamplerState : public Object
    {
    public:
        static SamplerStatePtr create();

        virtual ~SamplerState();

        const UVWAddressMode &getAddressMode() const;

        void setAddressMode(const UVWAddressMode &uvw);

        void setAddressMode(TextureAddressMode u, TextureAddressMode v,
            TextureAddressMode w);

        void setAddressMode(TextureAddressMode mode);

        const ColorRGBA &getBorderColor() const;

        void setBorderColor(const ColorRGBA &color);

        FilterOptions getFilter(FilterType type);

        void setFilter(FilterOptions opt, FilterType type);

        void setFilter(FilterOptions minFilter, FilterOptions magFilter,
            FilterOptions mipFilter);

        void setFilter(FilterType type);

        CompareFunction getCompareFunction() const;

        void setCompareFunction(CompareFunction func);

        uint32_t getAnisotropy() const;

        void setAnisotropy(uint32_t aniso);

        Real getMipmapBias() const;

        void setMipmapBias(Real bias);

    protected:
        SamplerState();

    protected:
        UVWAddressMode  mAddressMode;
        ColorRGBA       mBorderColor;
        FilterOptions   mMinFilter;
        FilterOptions   mMagFilter;
        FilterOptions   mMipFilter;
        CompareFunction mCompareFunc;
        uint32_t        mAnisotropy;
        Real            mMipmapBias;
    };
}


#include "T3DSamplerState.inl"


#endif  /*__T3D_SAMPLER_STATE_H__*/
