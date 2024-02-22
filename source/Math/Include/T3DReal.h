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

#ifndef __T3D_REAL_H__
#define __T3D_REAL_H__


#include "T3DMathPrerequisites.h"
#include "T3DFix32.h"
#include "T3DFix64.h"

namespace Tiny3D
{
    template <typename T>
    class TReal
    {
    public:
        static const T ZERO;
        static const T ONE;
        static const T HALF;
        static const T MINUS_ONE;
        static const T INF;
        static const T MINUS_INF;
        static const T EPSILON;
    };

    template <typename T> const T TReal<T>::ZERO;
    template <typename T> const T TReal<T>::ONE;
    template <typename T> const T TReal<T>::HALF;
    template <typename T> const T TReal<T>::MINUS_ONE;
    template <typename T> const T TReal<T>::INF;
    template <typename T> const T TReal<T>::MINUS_INF;
    template <typename T> const T TReal<T>::EPSILON;

    template <>
    class T3D_MATH_API TReal<float32_t>
    {
    public:
        static const float32_t ZERO;
        static const float32_t ONE;
        static const float32_t HALF;
        static const float32_t MINUS_ONE;
        static const float32_t INF;
        static const float32_t MINUS_INF;
        static const float32_t EPSILON;
    };

    template <>
    class T3D_MATH_API TReal<float64_t>
    {
    public:
        static const float64_t ZERO;
        static const float64_t ONE;
        static const float64_t HALF;
        static const float64_t MINUS_ONE;
        static const float64_t INF;
        static const float64_t MINUS_INF;
        static const float64_t EPSILON;
    };

    template <>
    class T3D_MATH_API TReal<fix32_t>
    {
    public:
        static const fix32_t ZERO;
        static const fix32_t ONE;
        static const fix32_t HALF;
        static const fix32_t MINUS_ONE;
        static const fix32_t INF;
        static const fix32_t MINUS_INF;
        static const fix32_t EPSILON;
    };

    template <>
    class T3D_MATH_API TReal<fix64_t>
    {
    public:
        static const fix64_t ZERO;
        static const fix64_t ONE;
        static const fix64_t HALF;
        static const fix64_t MINUS_ONE;
        static const fix64_t INF;
        static const fix64_t MINUS_INF;
        static const fix64_t EPSILON;
    };
}

// #include "T3DReal.inl"

#endif    /*__T3D_REAL_H__*/
