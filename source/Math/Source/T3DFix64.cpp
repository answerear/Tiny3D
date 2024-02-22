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


#include "T3DFix64.h"

namespace Tiny3D
{
    const int32_t fix64::INTEGER_BITS = 40;
    const int32_t fix64::DECIMAL_BITS = 24;

    const int64_t fix64::MAX_INT_VALUE = 549755813887LL; // 549755813887LL
    const int64_t fix64::MIN_INT_VALUE = -549755813888LL; // -549755813888LL

    const float32_t fix64::MAX_FLOAT_VALUE = (fix64::MAX_INT_VALUE * 1.0f); // 549755813887.0f
    const float32_t fix64::MIN_FLOAT_VALUE = (fix64::MIN_INT_VALUE * 1.0f); // -549755813888.0f

    const fix64 fix64::ZERO = fix64(0);
    const fix64 fix64::HALF = fix64(0.5f);
    const fix64 fix64::ONE = fix64(1);
    const fix64 fix64::MINUS_ONE = fix64(-1);
    const fix64 fix64::INF = fix64(0x7FFFFFFFFFFFFFFFLL, 0);
    const fix64 fix64::MINUSINF = fix64(0x8000000000000000LL, 0);
}


