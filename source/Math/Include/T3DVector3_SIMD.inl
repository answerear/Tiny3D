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

// SIMD specializations for TVector3<float32_t>
// This file is conditionally included from T3DVector3.inl

#if defined(T3D_SIMD_SSE) || defined(T3D_SIMD_NEON)

namespace Tiny3D
{

//=============================================================================
// SSE Implementation
//=============================================================================
#if defined(T3D_SIMD_SSE)

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator +(const TVector3 &other) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        __m128 r = _mm_add_ps(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator -(const TVector3 &other) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        __m128 r = _mm_sub_ps(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator *(float32_t scalar) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 s = _mm_set1_ps(scalar);
        __m128 r = _mm_mul_ps(a, s);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator *(const TVector3 &other) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        __m128 r = _mm_mul_ps(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator -() const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 r = _mm_sub_ps(_mm_setzero_ps(), a);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator +=(const TVector3 &other)
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        t3d_store_vec3(&_x, _mm_add_ps(a, b));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator -=(const TVector3 &other)
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        t3d_store_vec3(&_x, _mm_sub_ps(a, b));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator *=(float32_t scalar)
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 s = _mm_set1_ps(scalar);
        t3d_store_vec3(&_x, _mm_mul_ps(a, s));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator *=(const TVector3 &rkV)
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&rkV._x);
        t3d_store_vec3(&_x, _mm_mul_ps(a, b));
        return *this;
    }

    template <>
    inline float32_t TVector3<float32_t>::dot(const TVector3 &other) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);
        __m128 mul = _mm_mul_ps(a, b);
        // Sum only xyz (w is 0*0=0, so hsum is fine)
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TVector3<float32_t>::length2() const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 mul = _mm_mul_ps(a, a);
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TVector3<float32_t>::length() const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 mul = _mm_mul_ps(a, a);
        float sum = t3d_hsum_ps(mul);
        return TMath<float32_t>::sqrt(sum);
    }

    template <>
    inline float32_t TVector3<float32_t>::normalize()
    {
        float32_t len = length();

        if (len > TReal<float32_t>::ZERO)
        {
            __m128 a = t3d_load_vec3(&_x);
            __m128 invLen = _mm_set1_ps(1.0f / len);
            t3d_store_vec3(&_x, _mm_mul_ps(a, invLen));
        }
        else
        {
            len = TReal<float32_t>::ZERO;
            _x = 0.0f;
            _y = 0.0f;
            _z = 0.0f;
        }

        return len;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::cross(const TVector3 &other) const
    {
        __m128 a = t3d_load_vec3(&_x);
        __m128 b = t3d_load_vec3(&other._x);

        // cross = (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
        __m128 a_yzx = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 b_yzx = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));

        __m128 c = _mm_sub_ps(_mm_mul_ps(a, b_yzx), _mm_mul_ps(a_yzx, b));
        c = _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));

        TVector3 result;
        t3d_store_vec3(&result._x, c);
        return result;
    }

//=============================================================================
// NEON Implementation
//=============================================================================
#elif defined(T3D_SIMD_NEON)

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator +(const TVector3 &other) const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        float32x4_t r = vaddq_f32(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator -(const TVector3 &other) const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        float32x4_t r = vsubq_f32(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator *(float32_t scalar) const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t r = vmulq_n_f32(a, scalar);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator *(const TVector3 &other) const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        float32x4_t r = vmulq_f32(a, b);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::operator -() const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t r = vnegq_f32(a);
        TVector3 result;
        t3d_store_vec3(&result._x, r);
        return result;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator +=(const TVector3 &other)
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        t3d_store_vec3(&_x, vaddq_f32(a, b));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator -=(const TVector3 &other)
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        t3d_store_vec3(&_x, vsubq_f32(a, b));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator *=(float32_t scalar)
    {
        float32x4_t a = t3d_load_vec3(&_x);
        t3d_store_vec3(&_x, vmulq_n_f32(a, scalar));
        return *this;
    }

    template <>
    inline TVector3<float32_t> &TVector3<float32_t>::operator *=(const TVector3 &rkV)
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&rkV._x);
        t3d_store_vec3(&_x, vmulq_f32(a, b));
        return *this;
    }

    template <>
    inline float32_t TVector3<float32_t>::dot(const TVector3 &other) const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t b = t3d_load_vec3(&other._x);
        float32x4_t mul = vmulq_f32(a, b);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TVector3<float32_t>::length2() const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t mul = vmulq_f32(a, a);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TVector3<float32_t>::length() const
    {
        float32x4_t a = t3d_load_vec3(&_x);
        float32x4_t mul = vmulq_f32(a, a);
        float sum = t3d_hsum_f32(mul);
        return TMath<float32_t>::sqrt(sum);
    }

    template <>
    inline float32_t TVector3<float32_t>::normalize()
    {
        float32_t len = length();

        if (len > TReal<float32_t>::ZERO)
        {
            float32x4_t a = t3d_load_vec3(&_x);
            float32x4_t invLen = vdupq_n_f32(1.0f / len);
            t3d_store_vec3(&_x, vmulq_f32(a, invLen));
        }
        else
        {
            len = TReal<float32_t>::ZERO;
            _x = 0.0f;
            _y = 0.0f;
            _z = 0.0f;
        }

        return len;
    }

    template <>
    inline TVector3<float32_t> TVector3<float32_t>::cross(const TVector3 &other) const
    {
        // Scalar fallback for cross product on NEON (shuffle is less efficient)
        return TVector3(_y * other._z - _z * other._y,
            _z * other._x - _x * other._z,
            _x * other._y - _y * other._x);
    }

#endif // T3D_SIMD_SSE / T3D_SIMD_NEON

} // namespace Tiny3D

#endif // T3D_SIMD_SSE || T3D_SIMD_NEON
