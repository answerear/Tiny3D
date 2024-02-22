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

#include "T3DReal.h"


namespace Tiny3D
{
    const float32_t TReal<float32_t>::ZERO = 0.0f;
    const float32_t TReal<float32_t>::ONE = 1.0f;
    const float32_t TReal<float32_t>::HALF = 0.5f;
    const float32_t TReal<float32_t>::MINUS_ONE = -1.0f;
    const float32_t TReal<float32_t>::INF = std::numeric_limits<float32_t>::infinity();
    const float32_t TReal<float32_t>::MINUS_INF = -std::numeric_limits<float32_t>::infinity();
    const float32_t TReal<float32_t>::EPSILON = 1e-5f;

    const float64_t TReal<float64_t>::ZERO = 0.0;
    const float64_t TReal<float64_t>::ONE = 1.0;
    const float64_t TReal<float64_t>::HALF = 0.5;
    const float64_t TReal<float64_t>::MINUS_ONE = -1.0;
    const float64_t TReal<float64_t>::INF = std::numeric_limits<float64_t>::infinity();
    const float64_t TReal<float64_t>::MINUS_INF = -std::numeric_limits<float64_t>::infinity();
    const float64_t TReal<float64_t>::EPSILON = 1e-5;

    const fix32_t TReal<fix32_t>::ZERO = fix32_t::ZERO;
    const fix32_t TReal<fix32_t>::ONE = fix32_t::ONE;
    const fix32_t TReal<fix32_t>::HALF = fix32_t::HALF;
    const fix32_t TReal<fix32_t>::MINUS_ONE = fix32_t::MINUS_ONE;
    const fix32_t TReal<fix32_t>::INF = fix32_t::INF;
    const fix32_t TReal<fix32_t>::MINUS_INF = fix32_t::MINUSINF;
    const fix32_t TReal<fix32_t>::EPSILON = 1e-5;

    const fix64_t TReal<fix64_t>::ZERO = fix32_t::ZERO;
    const fix64_t TReal<fix64_t>::ONE = fix32_t::ONE;
    const fix64_t TReal<fix64_t>::HALF = fix32_t::HALF;
    const fix64_t TReal<fix64_t>::MINUS_ONE = fix32_t::MINUS_ONE;
    const fix64_t TReal<fix64_t>::INF = fix32_t::INF;
    const fix64_t TReal<fix64_t>::MINUS_INF = fix32_t::MINUSINF;
    const fix64_t TReal<fix64_t>::EPSILON = 1e-5;
}
