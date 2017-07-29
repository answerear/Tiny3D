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

#ifndef __T3D_IMAGE_H__
#define __T3D_IMAGE_H__



#include "Misc/T3DObject.h"
#include "Misc/T3DCommon.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Image : public Object
    {
        friend class ImageCodecBase;

    public:
        static const char * const FILETYPE_PNG;
        static const char * const FILETYPE_JPG;
        static const char * const FILETYPE_BMP;
        static const char * const FILETYPE_TGA;
        static const char * const FILETYPE_DDS;
        
        explicit Image();
        Image(int32_t width, int32_t height);
        Image(const Image &other);
        virtual ~Image();

        Image &operator =(const Image &other);

        bool operator ==(const Image &other) const;
        bool operator !=(const Image &other) const;

        bool load(const String &name);
        bool save(const String &name, const String &fileType = FILETYPE_PNG) const;

        bool flip();
        bool mirror();

        bool fill(const Color4 &rkColor);

        bool copyToScaling(void *dstData, int32_t dstWidth, int32_t dstHeight, PixelFormat dstFormat, int32_t dstPitch, bool needFlip = false) const;

        int32_t compare(const Image &other, bool compareAlpha = true) const;

        uint8_t *getData();
        const uint8_t *getData() const;

        size_t getSize() const;

        int32_t getWidth() const;
        int32_t getHeight() const;
        int32_t getPitch() const;
        int32_t getBPP() const;

        PixelFormat getFormat() const;

        bool hasAlpha() const;
        bool isPremultipliedAlpha() const;

    protected:
        void copy(const Image &other);

    private:
        int32_t     mWidth;
        int32_t     mHeight;
        int32_t     mBPP;
        int32_t     mPitch;

        PixelFormat mFormat;

        bool        mHasAlpha;
        bool        mIsPreMulti;

        uint8_t     *mData;
        size_t      mDataSize;
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/

