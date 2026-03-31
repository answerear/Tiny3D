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

// SIMD specializations for TQuaternion<float32_t>
// This file is conditionally included from T3DQuaternion.inl
//
// Memory layout: _w, _x, _y, _z  (w is first in memory)

#if defined(T3D_SIMD_SSE) || defined(T3D_SIMD_NEON)

namespace Tiny3D
{

//=============================================================================
// SSE Implementation
//=============================================================================
#if defined(T3D_SIMD_SSE)

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator +(
        const TQuaternion &other) const
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 b = _mm_loadu_ps(&other._w);
        TQuaternion result;
        _mm_storeu_ps(&result._w, _mm_add_ps(a, b));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator -(
        const TQuaternion &other) const
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 b = _mm_loadu_ps(&other._w);
        TQuaternion result;
        _mm_storeu_ps(&result._w, _mm_sub_ps(a, b));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator *(float32_t scalar) const
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 s = _mm_set1_ps(scalar);
        TQuaternion result;
        _mm_storeu_ps(&result._w, _mm_mul_ps(a, s));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator -() const
    {
        __m128 a = _mm_loadu_ps(&_w);
        TQuaternion result;
        _mm_storeu_ps(&result._w, _mm_sub_ps(_mm_setzero_ps(), a));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator +=(const TQuaternion &other)
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 b = _mm_loadu_ps(&other._w);
        _mm_storeu_ps(&_w, _mm_add_ps(a, b));
        return *this;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator -=(const TQuaternion &other)
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 b = _mm_loadu_ps(&other._w);
        _mm_storeu_ps(&_w, _mm_sub_ps(a, b));
        return *this;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator *=(float32_t scalar)
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 s = _mm_set1_ps(scalar);
        _mm_storeu_ps(&_w, _mm_mul_ps(a, s));
        return *this;
    }

    template <>
    inline float32_t TQuaternion<float32_t>::dot(const TQuaternion &other) const
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 b = _mm_loadu_ps(&other._w);
        __m128 mul = _mm_mul_ps(a, b);
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TQuaternion<float32_t>::norm() const
    {
        __m128 a = _mm_loadu_ps(&_w);
        __m128 mul = _mm_mul_ps(a, a);
        return t3d_hsum_ps(mul);
    }

    template <>
    inline float32_t TQuaternion<float32_t>::normalize()
    {
        float32_t len = norm();
        float32_t factor = 1.0f / TMath<float32_t>::sqrt(len);
        __m128 a = _mm_loadu_ps(&_w);
        __m128 f = _mm_set1_ps(factor);
        _mm_storeu_ps(&_w, _mm_mul_ps(a, f));
        return len;
    }

    // Quaternion multiplication (Hamilton product)
    // Memory layout: [w, x, y, z]
    // result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    // result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y
    // result.y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z
    // result.z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x
    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator *(
        const TQuaternion &other) const
    {
        // Use scalar implementation - quaternion multiplication has complex
        // sign patterns that don't map well to simple SIMD without SSE4.1 blendv.
        // The scalar version is already well-optimized by the compiler.
        return TQuaternion(
            _w * other._w - _x * other._x - _y * other._y - _z * other._z,
            _w * other._x + _x * other._w + _y * other._z - _z * other._y,
            _w * other._y + _y * other._w + _z * other._x - _x * other._z,
            _w * other._z + _z * other._w + _x * other._y - _y * other._x
        );
    }

//=============================================================================
// NEON Implementation
//=============================================================================
#elif defined(T3D_SIMD_NEON)

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator +(
        const TQuaternion &other) const
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t b = vld1q_f32(&other._w);
        TQuaternion result;
        vst1q_f32(&result._w, vaddq_f32(a, b));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator -(
        const TQuaternion &other) const
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t b = vld1q_f32(&other._w);
        TQuaternion result;
        vst1q_f32(&result._w, vsubq_f32(a, b));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator *(float32_t scalar) const
    {
        float32x4_t a = vld1q_f32(&_w);
        TQuaternion result;
        vst1q_f32(&result._w, vmulq_n_f32(a, scalar));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator -() const
    {
        float32x4_t a = vld1q_f32(&_w);
        TQuaternion result;
        vst1q_f32(&result._w, vnegq_f32(a));
        return result;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator +=(const TQuaternion &other)
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t b = vld1q_f32(&other._w);
        vst1q_f32(&_w, vaddq_f32(a, b));
        return *this;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator -=(const TQuaternion &other)
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t b = vld1q_f32(&other._w);
        vst1q_f32(&_w, vsubq_f32(a, b));
        return *this;
    }

    template <>
    inline TQuaternion<float32_t> &TQuaternion<float32_t>::operator *=(float32_t scalar)
    {
        float32x4_t a = vld1q_f32(&_w);
        vst1q_f32(&_w, vmulq_n_f32(a, scalar));
        return *this;
    }

    template <>
    inline float32_t TQuaternion<float32_t>::dot(const TQuaternion &other) const
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t b = vld1q_f32(&other._w);
        float32x4_t mul = vmulq_f32(a, b);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TQuaternion<float32_t>::norm() const
    {
        float32x4_t a = vld1q_f32(&_w);
        float32x4_t mul = vmulq_f32(a, a);
        return t3d_hsum_f32(mul);
    }

    template <>
    inline float32_t TQuaternion<float32_t>::normalize()
    {
        float32_t len = norm();
        float32_t factor = 1.0f / TMath<float32_t>::sqrt(len);
        float32x4_t a = vld1q_f32(&_w);
        vst1q_f32(&_w, vmulq_n_f32(a, factor));
        return len;
    }

    // Quaternion multiplication: scalar fallback (same reasoning as SSE)
    template <>
    inline TQuaternion<float32_t> TQuaternion<float32_t>::operator *(
        const TQuaternion &other) const
    {
        return TQuaternion(
            _w * other._w - _x * other._x - _y * other._y - _z * other._z,
            _w * other._x + _x * other._w + _y * other._z - _z * other._y,
            _w * other._y + _y * other._w + _z * other._x - _x * other._z,
            _w * other._z + _z * other._w + _x * other._y - _y * other._x
        );
    }

#endif // T3D_SIMD_SSE / T3D_SIMD_NEON

} // namespace Tiny3D

#endif // T3D_SIMD_SSE || T3D_SIMD_NEON
