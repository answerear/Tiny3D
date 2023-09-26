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

#ifndef __T3D_COLOR_ARGB_H__
#define __T3D_COLOR_ARGB_H__


#include "T3DMathPrerequisites.h"
#include "T3DColorRGB.h"


namespace Tiny3D
{
    TSTRUCT("Description"="A color structure from the four float ARGB components (alpha, red, green, and blue) values.")
    struct T3D_MATH_API ColorARGB
    {
        TRTTI_FRIEND

    public:
        static const ColorARGB BLACK;
        static const ColorARGB WHITE;
        static const ColorARGB RED;
        static const ColorARGB GREEN;
        static const ColorARGB BLUE;
        static const ColorARGB YELLOW;

        ColorARGB()
            : mBlue(1.0f)
            , mGreen(1.0f)
            , mRed(1.0f)
            , mAlpha(1.0f)
        {
            
        }

        ColorARGB(float32_t r, float32_t g, float32_t b, float32_t a = 1.0f)
            : mBlue(b)
            , mGreen(g)
            , mRed(r)
            , mAlpha(a)
        {

        }

        ColorARGB(const ColorRGB &color)
            : mBlue(color.blue())
            , mGreen(color.green())
            , mRed(color.red())
            , mAlpha(1.0f)
        {

        }

        ColorARGB(const ColorARGB &other)
        {
            mAlpha = other.mAlpha; 
            mRed = other.mRed;
            mGreen = other.mGreen; 
            mBlue = other.mBlue;
        }

        ColorARGB &operator =(const ColorARGB &other)
        {
            mAlpha = other.mAlpha; 
            mRed = other.mRed;
            mGreen = other.mGreen; 
            mBlue = other.mBlue;
            return *this;
        }

        bool operator ==(const ColorARGB &other) const
        {
            return (mAlpha == other.mAlpha 
                && mRed == other.mRed
                && mGreen == other.mGreen 
                && mBlue == other.mBlue);
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

    private:
        TPROPERTY(RTTRFuncName="alpha", RTTRFuncType="setter", "Description"="alpha")
        void setAlpha(float32_t a) { mAlpha = a; }

        TPROPERTY(RTTRFuncName="red", RTTRFuncTYpe="setter", "Description"="red")
        void setRed(float32_t r) { mRed = r; }

        TPROPERTY(RTTRFuncName="green", RTTRFuncTYpe="setter", "Description"="green")
        void setGreen(float32_t g) { mGreen = g; }
        
        TPROPERTY(RTTRFuncName="blue", RTTRFuncTYpe="setter", "Description"="blue")
        void setBlue(float32_t b) { mBlue = b; }
        
        float32_t   mBlue;
        float32_t   mGreen;
        float32_t   mRed;
        float32_t   mAlpha;
    };
}


#endif  /*__T3D_COLOR_ARGB_H__*/
