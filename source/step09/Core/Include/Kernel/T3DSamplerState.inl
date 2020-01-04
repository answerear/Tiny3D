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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    inline const UVWAddressMode &SamplerState::getAddressMode() const
    {
        return mAddressMode;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setAddressMode(const UVWAddressMode &uvw)
    {
        mAddressMode = uvw;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setAddressMode(TextureAddressMode u,
        TextureAddressMode v, TextureAddressMode w)
    {
        mAddressMode.u = u;
        mAddressMode.v = v;
        mAddressMode.w = w;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setAddressMode(TextureAddressMode mode)
    {
        mAddressMode.u = mAddressMode.v = mAddressMode.w = mode;
    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &SamplerState::getBorderColor() const
    {
        return mBorderColor;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setBorderColor(const ColorRGBA &color)
    {
        mBorderColor = color;
    }

    //--------------------------------------------------------------------------

    inline FilterOptions SamplerState::getFilter(FilterType type)
    {
        FilterOptions opt = FilterOptions::NONE;
        if (type == FilterType::MIN)
            opt = mMinFilter;
        else if (type == FilterType::MAG)
            opt = mMagFilter;
        else if (type == FilterType::MIP)
            opt = mMinFilter;
        return opt;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setFilter(FilterOptions opt, FilterType type)
    {
        if (type == FilterType::MIN)
            mMinFilter = opt;
        else if (type == FilterType::MAG)
            mMagFilter = opt;
        else if (type == FilterType::MIP)
            mMipFilter = opt;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setFilter(FilterOptions minFilter,
        FilterOptions magFilter, FilterOptions mipFilter)
    {
        mMinFilter = minFilter;
        mMagFilter = magFilter;
        mMipFilter = mipFilter;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setFilter(FilterType type)
    {
        if (type == FilterType::MIN)
            mMinFilter = FilterOptions::ANISOTROPIC;
        else if (type == FilterType::MAG)
            mMagFilter = FilterOptions::ANISOTROPIC;
        else if (type == FilterType::MIP)
            mMipFilter = FilterOptions::ANISOTROPIC;
    }

    //--------------------------------------------------------------------------

    inline CompareFunction SamplerState::getCompareFunction() const
    {
        return mCompareFunc;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setCompareFunction(CompareFunction func)
    {
        mCompareFunc = func;
    }

    //--------------------------------------------------------------------------

    inline uint32_t SamplerState::getAnisotropy() const
    {
        return mAnisotropy;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setAnisotropy(uint32_t aniso)
    {
        mAnisotropy = aniso;
    }

    //--------------------------------------------------------------------------

    inline Real SamplerState::getMipmapBias() const
    {
        return mMipmapBias;
    }

    //--------------------------------------------------------------------------

    inline void SamplerState::setMipmapBias(Real bias)
    {
        mMipmapBias = bias;
    }
}

