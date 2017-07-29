/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "ImageCodec/T3DImageCodecBase.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
//     const char * const ImageCodecBase::FORMAT_BMP = "bmp";
//     const char * const ImageCodecBase::FORMAT_PNG = "png";
//     const char * const ImageCodecBase::FORMAT_JPG = "jpg";
//     const char * const ImageCodecBase::FORMAT_TGA = "tga";
//     const char * const ImageCodecBase::FORMAT_DDS = "dds";

    ImageCodecBase::ImageCodecBase()
    {

    }

    ImageCodecBase::~ImageCodecBase()
    {

    }

//     bool ImageCodecBase::isSupportedType(const String &name)
//     {
//         FileType type = getFileType(name);
//         return (type != E_FT_UNKNOWN && type < MAX_TYPE_SUPPORTED);
//     }
// 
//     ImageCodecBase::FileType ImageCodecBase::getFileType(const String &name)
//     {
//         FileType type = E_FT_UNKNOWN;
//         String::size_type pos = name.rfind('.');
//         if (pos != String::npos)
//         {
//             String ext = name.substr(pos+1);
// 
//             if (stricmp(ext.c_str(), FORMAT_BMP) == 0)
//             {
//                 // BMP
//                 type = E_TYPE_BMP;
//             }
//             else if (stricmp(ext.c_str(), FORMAT_PNG) == 0)
//             {
//                 // PNG
//                 type = E_TYPE_PNG;
//             }
//             else if (stricmp(ext.c_str(), FORMAT_JPG) == 0)
//             {
//                 // JPG
//                 type = E_TYPE_JPG;
//             }
//             else if (stricmp(ext.c_str(), FORMAT_TGA) == 0)
//             {
//                 // TGA
//                 type = E_TYPE_TGA;
//             }
//             else if (stricmp(ext.c_str(), FORMAT_DDS) == 0)
//             {
//                 // DDS
//                 type = E_TYPE_DDS;
//             }
//         }
// 
//         return type;
//     }

    void ImageCodecBase::setImageData(Image &image, uint8_t *data, size_t size)
    {
        image.mData = data;
        image.mDataSize = size;
    }

    void ImageCodecBase::setImageDimension(Image &image, int32_t width, int32_t height, int32_t pitch)
    {
        image.mWidth = width;
        image.mHeight = height;
        image.mPitch = pitch;
    }

    void ImageCodecBase::setImageInfo(Image &image, int32_t bpp, bool hasAlpha, bool isPreMulti, PixelFormat eFormat)
    {
        image.mBPP = bpp;
        image.mHasAlpha = hasAlpha;
        image.mIsPreMulti = isPreMulti;
        image.mFormat = eFormat;
    }
}
