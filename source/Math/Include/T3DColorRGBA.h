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

#ifndef __T3D_COLOR_RGBA_H__
#define __T3D_COLOR_RGBA_H__


#include "T3DMathPrerequisites.h"
#include "T3DColorRGB.h"
#include "T3DColorARGB.h"


namespace Tiny3D
{
    TSTRUCT("Description"="A color structure from the four float RGBA components (red, green, blue, and alpha) values.")
    struct T3D_MATH_API ColorRGBA
    {
        TRTTI_FRIEND

    public:
        static const ColorRGBA BLACK;
        static const ColorRGBA WHITE;
        static const ColorRGBA RED;
        static const ColorRGBA GREEN;
        static const ColorRGBA BLUE;
        static const ColorRGBA YELLOW;

        ColorRGBA()
            : mRed(1.0f)
            , mGreen(1.0f)
            , mBlue(1.0f)
            , mAlpha(1.0f)
        {
            
        }
        
        ColorRGBA(float32_t r, float32_t g, float32_t b, float32_t a = 1.0f)
            : mRed(r)
            , mGreen(g)
            , mBlue(b)
            , mAlpha(a)
        {

        }

        ColorRGBA(const ColorRGB &color)
            : mRed(color.red())
            , mGreen(color.green())
            , mBlue(color.blue())
            , mAlpha(1.0f)
        {

        }

        ColorRGBA(const ColorARGB &color)
            : mRed(color.red())
            , mGreen(color.green())
            , mBlue(color.blue())
            , mAlpha(1.0f)
        {

        }

        ColorRGBA(const ColorRGBA &other)
        {
            mAlpha = other.mAlpha; 
            mRed = other.mRed;
            mGreen = other.mGreen; 
            mBlue = other.mBlue;
        }

        TFUNCTION("Description"="Convert to ARGB color format.")
        ColorARGB toARGB() const
        {
            return ColorARGB(mRed, mGreen, mBlue, mAlpha);
        }

        ColorRGBA &operator =(const ColorRGBA &other)
        {
            mAlpha = other.mAlpha; 
            mRed = other.mRed;
            mGreen = other.mGreen; 
            mBlue = other.mBlue;
            return *this;
        }

        bool operator ==(const ColorRGBA &other) const
        {
            return (mAlpha == other.mAlpha 
                && mRed == other.mRed
                && mGreen == other.mGreen 
                && mBlue == other.mBlue);
        }

        bool operator !=(const ColorRGBA &other) const
        {
            return !operator ==(other);
        }

        TPROPERTY(RTTRFuncName="alpha", RTTRFuncType="getter", "Description"="The alpha component")
        float32_t alpha() const   { return mAlpha; }
        float32_t &alpha()        { return mAlpha; }

        TPROPERTY(RTTRFuncName="red", RTTRFuncType="getter", "Description"="The red component")
        float32_t red() const     { return mRed; }
        float32_t &red()          { return mRed; }

        TPROPERTY(RTTRFuncName="green", RTTRFuncType="getter", "Description"="The green component")
        float32_t green() const   { return mGreen; }
        float32_t &green()        { return mGreen; }

        TPROPERTY(RTTRFuncName="blue", RTTRFuncType="getter", "Description"="The blue component")
        float32_t blue() const    { return mBlue; }
        float32_t &blue()         { return mBlue; }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "(" << mAlpha << ", " << mRed << ", " << mGreen << ", " << mBlue << ")";
            return ss.str();
        }
        
    private:
        TPROPERTY(RTTRFuncName="alpha", RTTRFuncType="setter", "Description"="The alpha component")
        void setAlpha(float32_t a) { mAlpha = a; }

        TPROPERTY(RTTRFuncName="red", RTTRFuncTYpe="setter", "Description"="The red component")
        void setRed(float32_t r) { mRed = r; }

        TPROPERTY(RTTRFuncName="green", RTTRFuncTYpe="setter", "Description"="The green component")
        void setGreen(float32_t g) { mGreen = g; }
        
        TPROPERTY(RTTRFuncName="blue", RTTRFuncTYpe="setter", "Description"="The blue component")
        void setBlue(float32_t b) { mBlue = b; }
        
        float32_t   mRed;
        float32_t   mGreen;
        float32_t   mBlue;
        float32_t   mAlpha;
    };
}


#endif  /*__T3D_COLOR_RGBA_H__*/
