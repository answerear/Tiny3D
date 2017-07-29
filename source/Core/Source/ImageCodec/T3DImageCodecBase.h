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

#ifndef __T3D_IMAGE_CODEC_BASE_H__
#define __T3D_IMAGE_CODEC_BASE_H__


#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    class ImageCodecBase : public Object
    {
    public:
        enum FileType
        {
            E_FT_UNKNOWN = -1,
            E_FT_BMP = 0,
            E_FT_ICO = 1,
            E_FT_JPEG = 2,
            E_FT_JNG = 3,
            E_FT_KOALA = 4,
            E_FT_LBM = 5,
            E_FT_IFF = E_FT_LBM,
            E_FT_MNG = 6,
            E_FT_PBM = 7,
            E_FT_PBMRAW = 8,
            E_FT_PCD = 9,
            E_FT_PCX = 10,
            E_FT_PGM = 11,
            E_FT_PGMRAW = 12,
            E_FT_PNG = 13,
            E_FT_PPM = 14,
            E_FT_PPMRAW = 15,
            E_FT_RAS = 16,
            E_FT_TARGA = 17,
            E_FT_TIFF = 18,
            E_FT_WBMP = 19,
            E_FT_PSD = 20,
            E_FT_CUT = 21,
            E_FT_XBM = 22,
            E_FT_XPM = 23,
            E_FT_DDS = 24,
            E_FT_GIF = 25,
            E_FT_HDR = 26,
            E_FT_FAXG3 = 27,
            E_FT_SGI = 28,
            E_FT_EXR = 29,
            E_FT_J2K = 30,
            E_FT_JP2 = 31,
            E_FT_PFM = 32,
            E_FT_PICT = 33,
            E_FT_RAW = 34,
            E_FT_WEBP = 35,
            E_FT_JXR = 36,
            E_FT_IMG = 37,
            MAX_TYPE_SUPPORTED,
        };


        virtual ~ImageCodecBase();

        virtual bool startup() = 0;
        virtual bool shutdown() = 0;

        virtual bool isSupportedType(uint8_t *data, size_t size, FileType &eFileType) const = 0;
        virtual FileType getFileType() const = 0;

        virtual bool encode(uint8_t *&data, size_t &size, const Image &image) = 0;
        virtual bool decode(uint8_t *data, size_t size, Image &image, FileType eFileType) = 0;

    protected:
        ImageCodecBase();

        void setImageData(Image &image, uint8_t *data, size_t size);
        void setImageDimension(Image &image, int32_t width, int32_t height, int32_t pitch);
        void setImageInfo(Image &image, int32_t bpp, bool hasAlpha, bool isPreMulti, PixelFormat eFormat);
    };

    T3D_DECLARE_SMART_PTR(ImageCodecBase);
}


#endif  /*__T3D_IMAGE_CODEC_BASE_H__*/
