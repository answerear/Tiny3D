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


#include "T3DFix32.h"


namespace Tiny3D
{
    const int32_t fix32::INTEGER_BITS = 20;
    const int32_t fix32::DECIMAL_BITS = 12;

    const int32_t fix32::MAX_INT_VALUE = 524287; // 524287
    const int32_t fix32::MIN_INT_VALUE = -524288; // -524288

    const float32_t fix32::MAX_FLOAT_VALUE = (fix32::MAX_INT_VALUE * 1.0f); // 524287.0f
    const float32_t fix32::MIN_FLOAT_VALUE = (fix32::MIN_INT_VALUE * 1.0f); // -524288.0f

    const fix32 fix32::ZERO(0);
    const fix32 fix32::HALF(0.5f);
    const fix32 fix32::ONE(1);
    const fix32 fix32::MINUS_ONE(-1);
    const fix32 fix32::INF(0x7FFFFFFF, 0);
    const fix32 fix32::MINUSINF(0x80000000, 0);
}
