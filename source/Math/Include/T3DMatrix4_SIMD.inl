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

// SIMD specializations for TMatrix4<float32_t>
// This file is conditionally included from T3DMatrix4.inl

#if defined(T3D_SIMD_SSE) || defined(T3D_SIMD_NEON)

namespace Tiny3D
{

//=============================================================================
// SSE Implementation
//=============================================================================
#if defined(T3D_SIMD_SSE)

    // Helper: multiply a row vector by matrix (row * mat)
    // row = [a, b, c, d], mat rows = r0, r1, r2, r3
    // result = a*r0 + b*r1 + c*r2 + d*r3
    static inline __m128 t3d_mat4_mul_row_sse(__m128 row, const float *mat)
    {
        __m128 r0 = _mm_load_ps(mat);
        __m128 r1 = _mm_load_ps(mat + 4);
        __m128 r2 = _mm_load_ps(mat + 8);
        __m128 r3 = _mm_load_ps(mat + 12);

        __m128 x = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 y = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 z = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 w = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3));

        return _mm_add_ps(
            _mm_add_ps(_mm_mul_ps(x, r0), _mm_mul_ps(y, r1)),
            _mm_add_ps(_mm_mul_ps(z, r2), _mm_mul_ps(w, r3)));
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator *(const TMatrix4 &other) const
    {
        TMatrix4 result(true);

        __m128 a0 = _mm_load_ps(mTuples);
        __m128 a1 = _mm_load_ps(mTuples + 4);
        __m128 a2 = _mm_load_ps(mTuples + 8);
        __m128 a3 = _mm_load_ps(mTuples + 12);

        _mm_store_ps(result.mTuples,      t3d_mat4_mul_row_sse(a0, other.mTuples));
        _mm_store_ps(result.mTuples + 4,  t3d_mat4_mul_row_sse(a1, other.mTuples));
        _mm_store_ps(result.mTuples + 8,  t3d_mat4_mul_row_sse(a2, other.mTuples));
        _mm_store_ps(result.mTuples + 12, t3d_mat4_mul_row_sse(a3, other.mTuples));

        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator +(const TMatrix4 &other) const
    {
        TMatrix4 result(true);
        _mm_store_ps(result.mTuples,      _mm_add_ps(_mm_load_ps(mTuples),      _mm_load_ps(other.mTuples)));
        _mm_store_ps(result.mTuples + 4,  _mm_add_ps(_mm_load_ps(mTuples + 4),  _mm_load_ps(other.mTuples + 4)));
        _mm_store_ps(result.mTuples + 8,  _mm_add_ps(_mm_load_ps(mTuples + 8),  _mm_load_ps(other.mTuples + 8)));
        _mm_store_ps(result.mTuples + 12, _mm_add_ps(_mm_load_ps(mTuples + 12), _mm_load_ps(other.mTuples + 12)));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator -(const TMatrix4 &other) const
    {
        TMatrix4 result(true);
        _mm_store_ps(result.mTuples,      _mm_sub_ps(_mm_load_ps(mTuples),      _mm_load_ps(other.mTuples)));
        _mm_store_ps(result.mTuples + 4,  _mm_sub_ps(_mm_load_ps(mTuples + 4),  _mm_load_ps(other.mTuples + 4)));
        _mm_store_ps(result.mTuples + 8,  _mm_sub_ps(_mm_load_ps(mTuples + 8),  _mm_load_ps(other.mTuples + 8)));
        _mm_store_ps(result.mTuples + 12, _mm_sub_ps(_mm_load_ps(mTuples + 12), _mm_load_ps(other.mTuples + 12)));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator *(float32_t scalar) const
    {
        __m128 s = _mm_set1_ps(scalar);
        TMatrix4 result(true);
        _mm_store_ps(result.mTuples,      _mm_mul_ps(_mm_load_ps(mTuples),      s));
        _mm_store_ps(result.mTuples + 4,  _mm_mul_ps(_mm_load_ps(mTuples + 4),  s));
        _mm_store_ps(result.mTuples + 8,  _mm_mul_ps(_mm_load_ps(mTuples + 8),  s));
        _mm_store_ps(result.mTuples + 12, _mm_mul_ps(_mm_load_ps(mTuples + 12), s));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator +=(const TMatrix4 &other)
    {
        _mm_store_ps(mTuples,      _mm_add_ps(_mm_load_ps(mTuples),      _mm_load_ps(other.mTuples)));
        _mm_store_ps(mTuples + 4,  _mm_add_ps(_mm_load_ps(mTuples + 4),  _mm_load_ps(other.mTuples + 4)));
        _mm_store_ps(mTuples + 8,  _mm_add_ps(_mm_load_ps(mTuples + 8),  _mm_load_ps(other.mTuples + 8)));
        _mm_store_ps(mTuples + 12, _mm_add_ps(_mm_load_ps(mTuples + 12), _mm_load_ps(other.mTuples + 12)));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator -=(const TMatrix4 &other)
    {
        _mm_store_ps(mTuples,      _mm_sub_ps(_mm_load_ps(mTuples),      _mm_load_ps(other.mTuples)));
        _mm_store_ps(mTuples + 4,  _mm_sub_ps(_mm_load_ps(mTuples + 4),  _mm_load_ps(other.mTuples + 4)));
        _mm_store_ps(mTuples + 8,  _mm_sub_ps(_mm_load_ps(mTuples + 8),  _mm_load_ps(other.mTuples + 8)));
        _mm_store_ps(mTuples + 12, _mm_sub_ps(_mm_load_ps(mTuples + 12), _mm_load_ps(other.mTuples + 12)));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator *=(float32_t scalar)
    {
        __m128 s = _mm_set1_ps(scalar);
        _mm_store_ps(mTuples,      _mm_mul_ps(_mm_load_ps(mTuples),      s));
        _mm_store_ps(mTuples + 4,  _mm_mul_ps(_mm_load_ps(mTuples + 4),  s));
        _mm_store_ps(mTuples + 8,  _mm_mul_ps(_mm_load_ps(mTuples + 8),  s));
        _mm_store_ps(mTuples + 12, _mm_mul_ps(_mm_load_ps(mTuples + 12), s));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::transpose() const
    {
        __m128 r0 = _mm_load_ps(mTuples);
        __m128 r1 = _mm_load_ps(mTuples + 4);
        __m128 r2 = _mm_load_ps(mTuples + 8);
        __m128 r3 = _mm_load_ps(mTuples + 12);

        _MM_TRANSPOSE4_PS(r0, r1, r2, r3);

        TMatrix4 result(true);
        _mm_store_ps(result.mTuples,      r0);
        _mm_store_ps(result.mTuples + 4,  r1);
        _mm_store_ps(result.mTuples + 8,  r2);
        _mm_store_ps(result.mTuples + 12, r3);
        return result;
    }

    template <>
    inline TVector4<float32_t> TMatrix4<float32_t>::operator *(const TVector4<float32_t> &rkV) const
    {
        // M * V (row-major): each result component is dot(row_i, V)
        __m128 v = _mm_load_ps((const float*)rkV);
        __m128 r0 = _mm_load_ps(mTuples);
        __m128 r1 = _mm_load_ps(mTuples + 4);
        __m128 r2 = _mm_load_ps(mTuples + 8);
        __m128 r3 = _mm_load_ps(mTuples + 12);

        __m128 m0 = _mm_mul_ps(r0, v);
        __m128 m1 = _mm_mul_ps(r1, v);
        __m128 m2 = _mm_mul_ps(r2, v);
        __m128 m3 = _mm_mul_ps(r3, v);

        TVector4<float32_t> result;
        result.x() = t3d_hsum_ps(m0);
        result.y() = t3d_hsum_ps(m1);
        result.z() = t3d_hsum_ps(m2);
        result.w() = t3d_hsum_ps(m3);
        return result;
    }

//=============================================================================
// NEON Implementation
//=============================================================================
#elif defined(T3D_SIMD_NEON)

    // Helper: multiply a row vector by matrix
    static inline float32x4_t t3d_mat4_mul_row_neon(float32x4_t row, const float *mat)
    {
        float32x4_t r0 = vld1q_f32(mat);
        float32x4_t r1 = vld1q_f32(mat + 4);
        float32x4_t r2 = vld1q_f32(mat + 8);
        float32x4_t r3 = vld1q_f32(mat + 12);

#if defined(__aarch64__)
        float32x4_t result = vmulq_laneq_f32(r0, row, 0);
        result = vmlaq_laneq_f32(result, r1, row, 1);
        result = vmlaq_laneq_f32(result, r2, row, 2);
        result = vmlaq_laneq_f32(result, r3, row, 3);
#else
        float32x2_t row_low = vget_low_f32(row);
        float32x2_t row_high = vget_high_f32(row);
        float32x4_t result = vmulq_lane_f32(r0, row_low, 0);
        result = vmlaq_lane_f32(result, r1, row_low, 1);
        result = vmlaq_lane_f32(result, r2, row_high, 0);
        result = vmlaq_lane_f32(result, r3, row_high, 1);
#endif
        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator *(const TMatrix4 &other) const
    {
        TMatrix4 result(true);

        float32x4_t a0 = vld1q_f32(mTuples);
        float32x4_t a1 = vld1q_f32(mTuples + 4);
        float32x4_t a2 = vld1q_f32(mTuples + 8);
        float32x4_t a3 = vld1q_f32(mTuples + 12);

        vst1q_f32(result.mTuples,      t3d_mat4_mul_row_neon(a0, other.mTuples));
        vst1q_f32(result.mTuples + 4,  t3d_mat4_mul_row_neon(a1, other.mTuples));
        vst1q_f32(result.mTuples + 8,  t3d_mat4_mul_row_neon(a2, other.mTuples));
        vst1q_f32(result.mTuples + 12, t3d_mat4_mul_row_neon(a3, other.mTuples));

        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator +(const TMatrix4 &other) const
    {
        TMatrix4 result(true);
        vst1q_f32(result.mTuples,      vaddq_f32(vld1q_f32(mTuples),      vld1q_f32(other.mTuples)));
        vst1q_f32(result.mTuples + 4,  vaddq_f32(vld1q_f32(mTuples + 4),  vld1q_f32(other.mTuples + 4)));
        vst1q_f32(result.mTuples + 8,  vaddq_f32(vld1q_f32(mTuples + 8),  vld1q_f32(other.mTuples + 8)));
        vst1q_f32(result.mTuples + 12, vaddq_f32(vld1q_f32(mTuples + 12), vld1q_f32(other.mTuples + 12)));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator -(const TMatrix4 &other) const
    {
        TMatrix4 result(true);
        vst1q_f32(result.mTuples,      vsubq_f32(vld1q_f32(mTuples),      vld1q_f32(other.mTuples)));
        vst1q_f32(result.mTuples + 4,  vsubq_f32(vld1q_f32(mTuples + 4),  vld1q_f32(other.mTuples + 4)));
        vst1q_f32(result.mTuples + 8,  vsubq_f32(vld1q_f32(mTuples + 8),  vld1q_f32(other.mTuples + 8)));
        vst1q_f32(result.mTuples + 12, vsubq_f32(vld1q_f32(mTuples + 12), vld1q_f32(other.mTuples + 12)));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::operator *(float32_t scalar) const
    {
        float32x4_t s = vdupq_n_f32(scalar);
        TMatrix4 result(true);
        vst1q_f32(result.mTuples,      vmulq_f32(vld1q_f32(mTuples),      s));
        vst1q_f32(result.mTuples + 4,  vmulq_f32(vld1q_f32(mTuples + 4),  s));
        vst1q_f32(result.mTuples + 8,  vmulq_f32(vld1q_f32(mTuples + 8),  s));
        vst1q_f32(result.mTuples + 12, vmulq_f32(vld1q_f32(mTuples + 12), s));
        return result;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator +=(const TMatrix4 &other)
    {
        vst1q_f32(mTuples,      vaddq_f32(vld1q_f32(mTuples),      vld1q_f32(other.mTuples)));
        vst1q_f32(mTuples + 4,  vaddq_f32(vld1q_f32(mTuples + 4),  vld1q_f32(other.mTuples + 4)));
        vst1q_f32(mTuples + 8,  vaddq_f32(vld1q_f32(mTuples + 8),  vld1q_f32(other.mTuples + 8)));
        vst1q_f32(mTuples + 12, vaddq_f32(vld1q_f32(mTuples + 12), vld1q_f32(other.mTuples + 12)));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator -=(const TMatrix4 &other)
    {
        vst1q_f32(mTuples,      vsubq_f32(vld1q_f32(mTuples),      vld1q_f32(other.mTuples)));
        vst1q_f32(mTuples + 4,  vsubq_f32(vld1q_f32(mTuples + 4),  vld1q_f32(other.mTuples + 4)));
        vst1q_f32(mTuples + 8,  vsubq_f32(vld1q_f32(mTuples + 8),  vld1q_f32(other.mTuples + 8)));
        vst1q_f32(mTuples + 12, vsubq_f32(vld1q_f32(mTuples + 12), vld1q_f32(other.mTuples + 12)));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> &TMatrix4<float32_t>::operator *=(float32_t scalar)
    {
        float32x4_t s = vdupq_n_f32(scalar);
        vst1q_f32(mTuples,      vmulq_f32(vld1q_f32(mTuples),      s));
        vst1q_f32(mTuples + 4,  vmulq_f32(vld1q_f32(mTuples + 4),  s));
        vst1q_f32(mTuples + 8,  vmulq_f32(vld1q_f32(mTuples + 8),  s));
        vst1q_f32(mTuples + 12, vmulq_f32(vld1q_f32(mTuples + 12), s));
        return *this;
    }

    template <>
    inline TMatrix4<float32_t> TMatrix4<float32_t>::transpose() const
    {
        float32x4x2_t r01 = vtrnq_f32(vld1q_f32(mTuples), vld1q_f32(mTuples + 4));
        float32x4x2_t r23 = vtrnq_f32(vld1q_f32(mTuples + 8), vld1q_f32(mTuples + 12));

        TMatrix4 result(true);
        vst1q_f32(result.mTuples,      vcombine_f32(vget_low_f32(r01.val[0]),  vget_low_f32(r23.val[0])));
        vst1q_f32(result.mTuples + 4,  vcombine_f32(vget_low_f32(r01.val[1]),  vget_low_f32(r23.val[1])));
        vst1q_f32(result.mTuples + 8,  vcombine_f32(vget_high_f32(r01.val[0]), vget_high_f32(r23.val[0])));
        vst1q_f32(result.mTuples + 12, vcombine_f32(vget_high_f32(r01.val[1]), vget_high_f32(r23.val[1])));
        return result;
    }

    template <>
    inline TVector4<float32_t> TMatrix4<float32_t>::operator *(const TVector4<float32_t> &rkV) const
    {
        float32x4_t v = vld1q_f32((const float*)rkV);

        TVector4<float32_t> result;
        result.x() = t3d_hsum_f32(vmulq_f32(vld1q_f32(mTuples), v));
        result.y() = t3d_hsum_f32(vmulq_f32(vld1q_f32(mTuples + 4), v));
        result.z() = t3d_hsum_f32(vmulq_f32(vld1q_f32(mTuples + 8), v));
        result.w() = t3d_hsum_f32(vmulq_f32(vld1q_f32(mTuples + 12), v));
        return result;
    }

#endif // T3D_SIMD_SSE / T3D_SIMD_NEON

} // namespace Tiny3D

#endif // T3D_SIMD_SSE || T3D_SIMD_NEON
