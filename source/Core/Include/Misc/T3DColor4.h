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

#ifndef __T3D_COLOR4_H__
#define __T3D_COLOR4_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DCommon.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Color4
    {
    public:
        Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
            : mAlpha(a)
            , mRed(r)
            , mGreen(g)
            , mBlue(b)
        {

        }

        Color4(const Color4 &other)
        {
            mAlpha = other.mAlpha; mRed = other.mRed;
            mGreen = other.mGreen; mBlue = other.mBlue;
        }

        Color4 &operator =(const Color4 &other)
        {
            mAlpha = other.mAlpha; mRed = other.mRed;
            mGreen = other.mGreen; mBlue = other.mBlue;
            return *this;
        }

        bool operator ==(const Color4 &other) const
        {
            return (mAlpha == other.mAlpha && mRed == other.mRed
                && mGreen == other.mGreen && mBlue == other.mBlue);
        }

        uint8_t alpha() const   { return mAlpha; }
        uint8_t &alpha()        { return mAlpha; }

        uint8_t red() const     { return mRed; }
        uint8_t &red()          { return mRed; }

        uint8_t green() const   { return mGreen; }
        uint8_t &green()        { return mGreen; }

        uint8_t blue() const    { return mBlue; }
        uint8_t &blue()         { return mBlue; }

        uint32_t A8R8G8B8() const
        {
            return ((mAlpha << 24) & 0xFF000000) | ((mRed << 16) & 0x00FF0000)
                | ((mGreen << 8) & 0x0000FF00) | (mBlue & 0x000000FF);
        }

        uint16_t A1R5G5B5() const
        {
            uint8_t alpha = (mAlpha > 0 ? 1 : 0);
            return ((alpha << 15) & 0x8000) | ((mRed << 10) & 0x7C00)
                | ((mGreen << 5) & 0x03E0) | (mBlue & 0x001F);
        }

        uint16_t R5G6B5() const
        {
            return ((mRed << 11) & 0xF800) | ((mGreen << 5) & 0x07E0) 
                | (mBlue & 0x001F);
        }

        uint16_t A4R4G4B4() const
        {
            uint8_t alpha = (mAlpha >> 1);
            return ((alpha << 12) & 0xF000) | ((mRed << 8) & 0x0F00) 
                | ((mGreen << 4) & 0x00F0) | (mBlue & 0x000F);
        }

        static void convert_B8G8R8A8toA8R8G8B8(void *src, void *dst, size_t count);

        static void convert(void *srcPixel, PixelFormat srcFmt, void *dstPixel, PixelFormat dstFmt);

        static const Color4 BLACK;
        static const Color4 WHITE;
        static const Color4 RED;
        static const Color4 GREEN;
        static const Color4 BLUE;

    private:
        uint8_t     mBlue;
        uint8_t     mGreen;
        uint8_t     mRed;
        uint8_t     mAlpha;
    };
}


#endif  /*__T3D_COLOR4_H__*/
