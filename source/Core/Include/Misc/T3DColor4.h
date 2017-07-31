/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
        static const Color4 BLACK;
        static const Color4 WHITE;
        static const Color4 RED;
        static const Color4 GREEN;
        static const Color4 BLUE;

        static const uint16_t RGB555_RED_MASK;
        static const uint16_t RGB555_GREEN_MASK;
        static const uint16_t RGB555_BLUE_MASK;
        static const uint16_t RGB555_ALPHA_MASK;

        static const uint16_t RGB565_RED_MASK;
        static const uint16_t RGB565_GREEN_MASK;
        static const uint16_t RGB565_BLUE_MASK;

        static const uint16_t RGB444_RED_MASK;
        static const uint16_t RGB444_GREEN_MASK;
        static const uint16_t RGB444_BLUE_MASK;
        static const uint16_t RGB444_ALPHA_MASK;

        static const uint32_t RGB_RED_MASK;
        static const uint32_t RGB_GREEN_MASK;
        static const uint32_t RGB_BLUE_MASK;
        static const uint32_t RGB_ALPHA_MASK;

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
            return ((mAlpha << 24) & RGB_ALPHA_MASK) | ((mRed << 16) & RGB_RED_MASK)
                | ((mGreen << 8) & RGB_GREEN_MASK) | (mBlue & RGB_BLUE_MASK);
        }

        uint16_t A1R5G5B5() const
        {
            uint8_t alpha = (mAlpha > 0 ? 1 : 0);
            return ((alpha << 15) & RGB555_ALPHA_MASK) | ((mRed << 10) & RGB555_RED_MASK)
                | ((mGreen << 5) & RGB555_GREEN_MASK) | (mBlue & RGB555_BLUE_MASK);
        }

        uint16_t R5G6B5() const
        {
            return ((mRed << 11) & RGB565_RED_MASK) | ((mGreen << 5) & RGB565_GREEN_MASK)
                | (mBlue & RGB565_BLUE_MASK);
        }

        uint16_t A4R4G4B4() const
        {
            uint8_t alpha = (mAlpha >> 1);
            return ((alpha << 12) & RGB444_ALPHA_MASK) | ((mRed << 8) & RGB444_RED_MASK) 
                | ((mGreen << 4) & RGB444_GREEN_MASK) | (mBlue & RGB444_BLUE_MASK);
        }

        static void convert_B8G8R8A8toA8R8G8B8(void *src, void *dst, size_t count);

        static void convert(void *srcPixel, PixelFormat srcFmt, void *dstPixel, PixelFormat dstFmt);

    private:
        uint8_t     mBlue;
        uint8_t     mGreen;
        uint8_t     mRed;
        uint8_t     mAlpha;
    };
}


#endif  /*__T3D_COLOR4_H__*/
