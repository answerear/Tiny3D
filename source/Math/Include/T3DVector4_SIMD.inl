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

// SIMD specializations for TVector4<float32_t>
// This file is conditionally included from T3DVector4.inl

#if defined(T3D_SIMD_SSE) || defined(T3D_SIMD_NEON)

namespace Tiny3D
{

//=============================================================================
// SSE Implementation
//=============================================================================
#if defined(T3D_SIMD_SSE)

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator +(const TVector4 &other) const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);
        __m128 r = _mm_add_ps(a, b);
        TVector4 result;
        _mm_storeu_ps(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator -(const TVector4 &other) const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);
        __m128 r = _mm_sub_ps(a, b);
        TVector4 result;
        _mm_storeu_ps(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator *(float32_t scalar) const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 s = _mm_set1_ps(scalar);
        __m128 r = _mm_mul_ps(a, s);
        TVector4 result;
        _mm_storeu_ps(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator -() const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 r = _mm_sub_ps(_mm_setzero_ps(), a);
        TVector4 result;
        _mm_storeu_ps(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator +=(const TVector4 &other)
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);
        _mm_storeu_ps(&_x, _mm_add_ps(a, b));
        return *this;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator -=(const TVector4 &other)
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);
        _mm_storeu_ps(&_x, _mm_sub_ps(a, b));
        return *this;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator *=(float32_t scalar)
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 s = _mm_set1_ps(scalar);
        _mm_storeu_ps(&_x, _mm_mul_ps(a, s));
        return *this;
    }

    template <>
    inline float32_t TVector4<float32_t>::dot(const TVector4 &other) const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);
        __m128 mul = _mm_mul_ps(a, b);
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TVector4<float32_t>::length2() const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 mul = _mm_mul_ps(a, a);
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TVector4<float32_t>::length() const
    {
        __m128 a = _mm_loadu_ps(&_x);
        __m128 mul = _mm_mul_ps(a, a);
        float sum = t3d_hsum_ps(mul);
        return TMath<float32_t>::sqrt(sum);
    }

    template <>
    inline float32_t TVector4<float32_t>::normalize()
    {
        float32_t len = length();

        if (len > TReal<float32_t>::ZERO)
        {
            __m128 a = _mm_loadu_ps(&_x);
            __m128 invLen = _mm_set1_ps(1.0f / len);
            _mm_storeu_ps(&_x, _mm_mul_ps(a, invLen));
        }
        else
        {
            len = TReal<float32_t>::ZERO;
            _mm_storeu_ps(&_x, _mm_setzero_ps());
        }

        return len;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::cross(const TVector4 &other) const
    {
        // cross for 4D: (y*z'-z*y', z*w'-w*z', w*x'-x*w', x*y'-y*x')
        __m128 a = _mm_loadu_ps(&_x);
        __m128 b = _mm_loadu_ps(&other._x);

        // a shuffled: (y, z, w, x)
        __m128 a1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 3, 2, 1));
        // b shuffled: (z, w, x, y)
        __m128 b1 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 0, 3, 2));
        // a shuffled: (z, w, x, y)
        __m128 a2 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
        // b shuffled: (y, z, w, x)
        __m128 b2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(0, 3, 2, 1));

        __m128 r = _mm_sub_ps(_mm_mul_ps(a1, b1), _mm_mul_ps(a2, b2));
        TVector4 result;
        _mm_storeu_ps(&result._x, r);
        return result;
    }

//=============================================================================
// NEON Implementation
//=============================================================================
#elif defined(T3D_SIMD_NEON)

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator +(const TVector4 &other) const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t b = vld1q_f32(&other._x);
        float32x4_t r = vaddq_f32(a, b);
        TVector4 result;
        vst1q_f32(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator -(const TVector4 &other) const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t b = vld1q_f32(&other._x);
        float32x4_t r = vsubq_f32(a, b);
        TVector4 result;
        vst1q_f32(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator *(float32_t scalar) const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t r = vmulq_n_f32(a, scalar);
        TVector4 result;
        vst1q_f32(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::operator -() const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t r = vnegq_f32(a);
        TVector4 result;
        vst1q_f32(&result._x, r);
        return result;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator +=(const TVector4 &other)
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t b = vld1q_f32(&other._x);
        vst1q_f32(&_x, vaddq_f32(a, b));
        return *this;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator -=(const TVector4 &other)
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t b = vld1q_f32(&other._x);
        vst1q_f32(&_x, vsubq_f32(a, b));
        return *this;
    }

    template <>
    inline TVector4<float32_t> &TVector4<float32_t>::operator *=(float32_t scalar)
    {
        float32x4_t a = vld1q_f32(&_x);
        vst1q_f32(&_x, vmulq_n_f32(a, scalar));
        return *this;
    }

    template <>
    inline float32_t TVector4<float32_t>::dot(const TVector4 &other) const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t b = vld1q_f32(&other._x);
        float32x4_t mul = vmulq_f32(a, b);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TVector4<float32_t>::length2() const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t mul = vmulq_f32(a, a);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TVector4<float32_t>::length() const
    {
        float32x4_t a = vld1q_f32(&_x);
        float32x4_t mul = vmulq_f32(a, a);
        float sum = t3d_hsum_f32(mul);
        return TMath<float32_t>::sqrt(sum);
    }

    template <>
    inline float32_t TVector4<float32_t>::normalize()
    {
        float32_t len = length();

        if (len > TReal<float32_t>::ZERO)
        {
            float32x4_t a = vld1q_f32(&_x);
            float32x4_t invLen = vdupq_n_f32(1.0f / len);
            vst1q_f32(&_x, vmulq_f32(a, invLen));
        }
        else
        {
            len = TReal<float32_t>::ZERO;
            vst1q_f32(&_x, vdupq_n_f32(0.0f));
        }

        return len;
    }

    template <>
    inline TVector4<float32_t> TVector4<float32_t>::cross(const TVector4 &other) const
    {
        // Fallback to scalar for 4D cross on NEON (less common operation)
        return TVector4(_y * other._z - _z * other._y,
            _z * other._w - _w * other._z,
            _w * other._x - _x * other._w,
            _x * other._y - _y * other._x);
    }

#endif // T3D_SIMD_SSE / T3D_SIMD_NEON

} // namespace Tiny3D

#endif // T3D_SIMD_SSE || T3D_SIMD_NEON
