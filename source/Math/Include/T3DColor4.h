/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_COLOR4_H__
#define __T3D_COLOR4_H__


#include "T3DMathPrerequisites.h"
#include "T3DColorRGB.h"
#include "T3DColorARGB.h"


namespace Tiny3D
{
    TSTRUCT("Description"="A color structure from the four 8-bit ARGB components (alpha, red, green, and blue) values.")
    struct T3D_MATH_API Color4
    {
        TRTTI_FRIEND

    public:
        static const Color4 BLACK;
        static const Color4 WHITE;
        static const Color4 RED;
        static const Color4 GREEN;
        static const Color4 BLUE;
        static const Color4 YELLOW;

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

        Color4()
            : mBlue(0xFF)
            , mGreen(0xFF)
            , mRed(0xFF)
            , mAlpha(0xFF)
        {
            
        }

        Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
            : mBlue(b)
            , mGreen(g)
            , mRed(r)
            , mAlpha(a)
        {

        }

        Color4(const ColorRGB &color)
            : mBlue((uint8_t)(color.blue() * 255.0f))
            , mGreen((uint8_t)(color.green() * 255.0f))
            , mRed((uint8_t)(color.red() * 255.0f))
            , mAlpha(255)
        {

        }

        Color4(const ColorARGB &color)
            : mBlue((uint8_t)(color.blue() * 255.0f))
            , mGreen((uint8_t)(color.green() * 255.0f))
            , mRed((uint8_t)(color.red() * 255.0f))
            , mAlpha((uint8_t)(color.alpha() * 255.0f))
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

        TFUNCTION("Description"="Convert from RGB with 3 float")
        void fromColor3f(ColorRGB color)
        {
            mAlpha = 0xFF;
            mRed = (uint8_t)(color.red() * 255.0f);
            mGreen = (uint8_t)(color.green() * 255.0f);
            mBlue = (uint8_t)(color.blue() * 255.0f);
        }

        TFUNCTION("Description"="Convert from ARGB with 4 float")
        void fromColor4f(ColorARGB color)
        {
            mAlpha = (uint8_t)(color.alpha() * 255.0f);
            mRed = (uint8_t)(color.red() * 255.0f);
            mGreen = (uint8_t)(color.green() * 255.0f);
            mBlue = (uint8_t)(color.blue() * 255.0f);
        }

        TFUNCTION("Description"="Convert to RGB with 3 float")
        ColorRGB toColor3f() const
        {
            return ColorRGB(
                (float32_t)mBlue / 255.0f,
                (float32_t)mGreen / 255.0f,
                (float32_t)mRed / 255.0f
            );
        }

        TFUNCTION("Description"="Convert to ARGB with 4 float")
        ColorARGB toColor4f() const
        {
            return ColorARGB(
                (float32_t)mBlue / 255.0f,
                (float32_t)mGreen / 255.0f,
                (float32_t)mRed / 255.0f,
                (float32_t)mAlpha / 255.0f
            );
        }

        TPROPERTY(RTTRFuncName="alpha", RTTRFuncType="getter", "Description"="The alpha component")
        uint8_t alpha() const   { return mAlpha; }
        uint8_t &alpha()        { return mAlpha; }

        TPROPERTY(RTTRFuncName="red", RTTRFuncType="getter", "Description"="The red component")
        uint8_t red() const     { return mRed; }
        uint8_t &red()          { return mRed; }

        TPROPERTY(RTTRFuncName="green", RTTRFuncType="getter", "Description"="The green component")
        uint8_t green() const   { return mGreen; }
        uint8_t &green()        { return mGreen; }

        TPROPERTY(RTTRFuncName="blue", RTTRFuncType="getter", "Description"="The blue component")
        uint8_t blue() const    { return mBlue; }
        uint8_t &blue()         { return mBlue; }

        TFUNCTION("Description"="Convert to ARGB32")
        uint32_t A8R8G8B8() const
        {
            return ((mAlpha << 24) & RGB_ALPHA_MASK) 
                | ((mRed << 16) & RGB_RED_MASK)
                | ((mGreen << 8) & RGB_GREEN_MASK) 
                | (mBlue & RGB_BLUE_MASK);
        }

        TFUNCTION("Description"="Convert to ARGB1555")
        uint16_t A1R5G5B5() const
        {
            uint8_t alpha = (mAlpha > 0 ? 1 : 0);
            return ((alpha << 15) & RGB555_ALPHA_MASK) 
                | ((mRed << 10) & RGB555_RED_MASK)
                | ((mGreen << 5) & RGB555_GREEN_MASK) 
                | (mBlue & RGB555_BLUE_MASK);
        }

        TFUNCTION("Description"="Convert to RGB565")
        uint16_t R5G6B5() const
        {
            return ((mRed << 11) & RGB565_RED_MASK) 
                | ((mGreen << 5) & RGB565_GREEN_MASK)
                | (mBlue & RGB565_BLUE_MASK);
        }

        TFUNCTION("Description"="Convert to ARGB4444")
        uint16_t A4R4G4B4() const
        {
            uint8_t alpha = (mAlpha >> 1);
            return ((alpha << 12) & RGB444_ALPHA_MASK) 
                | ((mRed << 8) & RGB444_RED_MASK) 
                | ((mGreen << 4) & RGB444_GREEN_MASK) 
                | (mBlue & RGB444_BLUE_MASK);
        }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "(" << mAlpha << ", " << mRed << ", " << mGreen << ", " << mBlue << ")";
            return ss.str();
        }

    private:
        TPROPERTY(RTTRFuncName="alpha", RTTRFuncType="setter", "Description"="alpha")
        void setAlpha(uint8_t a) { mAlpha = a; }

        TPROPERTY(RTTRFuncName="red", RTTRFuncTYpe="setter", "Description"="red")
        void setRed(uint8_t r) { mRed = r; }

        TPROPERTY(RTTRFuncName="green", RTTRFuncTYpe="setter", "Description"="green")
        void setGreen(uint8_t g) { mGreen = g; }
        
        TPROPERTY(RTTRFuncName="blue", RTTRFuncTYpe="setter", "Description"="blue")
        void setBlue(uint8_t b) { mBlue = b; }
        
        uint8_t     mBlue;
        uint8_t     mGreen;
        uint8_t     mRed;
        uint8_t     mAlpha;
    };
}


#endif  /*__T3D_COLOR4_H__*/
