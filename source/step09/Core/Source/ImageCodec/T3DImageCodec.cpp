/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include "ImageCodec/T3DImageCodec.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(ImageCodec);

    //--------------------------------------------------------------------------

    ImageCodecPtr ImageCodec::create()
    {
        ImageCodecPtr codec = new ImageCodec();
        codec->release();
        return codec;
    }

    //--------------------------------------------------------------------------

    ImageCodec::ImageCodec()
    {

    }

    ImageCodec::~ImageCodec()
    {

    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::addImageCodec(ImageCodecBasePtr codec)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult ImageCodec::removeImageCodec(ImageCodecBasePtr codec)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::encode(const String &name, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_PNG */)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult ImageCodec::encode(DataStream &stream, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_PNG */)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult ImageCodec::encode(uint8_t *&data, size_t &size, const Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_PNG */)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ImageCodec::decode(const String &name, Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult ImageCodec::decode(DataStream &stream, Image &image, 
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        TResult ret = T3D_OK;
        
        return ret;
    }

    TResult ImageCodec::decode(uint8_t *data, size_t size, Image &image,
        ImageCodecBase::FileType type /* = ImageCodecBase::E_FT_UNKNOWN */)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    ImageCodecBasePtr ImageCodec::getImageCodec(uint8_t *data, size_t size, 
        ImageCodecBase::FileType &type) const
    {
        ImageCodecBasePtr codec;

        if (type != ImageCodecBase::E_FT_UNKNOWN)
        {
            auto itr = mCodecMap.find(type);
            if (itr != mCodecMap.end())
            {
                codec = itr->second;
            }
        }

        return codec;
    }

    ImageCodecBasePtr ImageCodec::getImageCodec(
        ImageCodecBase::FileType type) const
    {
        ImageCodecBasePtr codec;
        auto itr = mCodecMap.find(type);
        if (itr != mCodecMap.end())
        {
            codec = itr->second;
        }
        return codec;
    }
}

