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

#ifndef __T3D_SIMD_CONFIG_H__
#define __T3D_SIMD_CONFIG_H__

#define T3D_SIMD_DISABLED

//-----------------------------------------------------------------------------
// SIMD platform detection
//-----------------------------------------------------------------------------

// Allow user to force disable SIMD via T3D_SIMD_DISABLED
#if !defined(T3D_SIMD_DISABLED)

    //-------------------------------------------------------------------------
    // SSE detection (x86 / x64)
    //-------------------------------------------------------------------------
    #if defined(_MSC_VER)
        // MSVC: SSE2 is always available on x64; on x86 check _M_IX86_FP
        #if defined(_M_X64) || defined(_M_AMD64)
            #define T3D_SIMD_SSE
        #elif defined(_M_IX86_FP) && (_M_IX86_FP >= 2)
            #define T3D_SIMD_SSE
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        // GCC / Clang: check predefined macros
        #if defined(__SSE2__)
            #define T3D_SIMD_SSE
        #endif
    #endif

    //-------------------------------------------------------------------------
    // NEON detection (ARM)
    //-------------------------------------------------------------------------
    #if !defined(T3D_SIMD_SSE)
        #if defined(__ARM_NEON) || defined(__ARM_NEON__)
            #define T3D_SIMD_NEON
        #elif defined(_M_ARM64) || defined(_M_ARM)
            // MSVC ARM targets
            #define T3D_SIMD_NEON
        #endif
    #endif

#endif // !T3D_SIMD_DISABLED


//-----------------------------------------------------------------------------
// Fallback: no SIMD
//-----------------------------------------------------------------------------
#if !defined(T3D_SIMD_SSE) && !defined(T3D_SIMD_NEON)
    #define T3D_SIMD_NONE
#endif


//-----------------------------------------------------------------------------
// Include platform SIMD intrinsic headers
//-----------------------------------------------------------------------------
#if defined(T3D_SIMD_SSE)
    #include <immintrin.h>
#elif defined(T3D_SIMD_NEON)
    #include <arm_neon.h>
#endif


//-----------------------------------------------------------------------------
// Helper: load 3-component vector into 128-bit register (4th = 0)
//         store 128-bit register back to 3-component vector
// These are used by TVector3 SIMD specializations.
//-----------------------------------------------------------------------------
#if defined(T3D_SIMD_SSE)

    // Load 3 floats from ptr into __m128, 4th element = 0
    inline __m128 t3d_load_vec3(const float *ptr)
    {
        // ptr[0], ptr[1], ptr[2], 0.0f
        __m128 xy = _mm_loadl_pi(_mm_setzero_ps(),
                                  reinterpret_cast<const __m64*>(ptr));
        __m128 z  = _mm_load_ss(ptr + 2);
        return _mm_movelh_ps(xy, z);
    }

    // Store lower 3 floats from __m128 to ptr
    inline void t3d_store_vec3(float *ptr, __m128 v)
    {
        _mm_storel_pi(reinterpret_cast<__m64*>(ptr), v);
        _mm_store_ss(ptr + 2, _mm_movehl_ps(v, v));
    }

    // Horizontal sum of all 4 floats in __m128 -> scalar
    inline float t3d_hsum_ps(__m128 v)
    {
        // SSE3 hadd approach, but we keep SSE2 compatible
        __m128 shuf = _mm_movehdup_ps(v);   // (v1, v1, v3, v3) - SSE3
        __m128 sums = _mm_add_ps(v, shuf);  // (v0+v1, -, v2+v3, -)
        shuf = _mm_movehl_ps(shuf, sums);   // (v2+v3, -, -, -)
        sums = _mm_add_ss(sums, shuf);      // (v0+v1+v2+v3, -, -, -)
        return _mm_cvtss_f32(sums);
    }

#elif defined(T3D_SIMD_NEON)

    // Load 3 floats from ptr into float32x4_t, 4th element = 0
    inline float32x4_t t3d_load_vec3(const float *ptr)
    {
        float32x2_t xy = vld1_f32(ptr);
        float32x2_t z0 = vld1_lane_f32(ptr + 2, vdup_n_f32(0.0f), 0);
        return vcombine_f32(xy, z0);
    }

    // Store lower 3 floats from float32x4_t to ptr
    inline void t3d_store_vec3(float *ptr, float32x4_t v)
    {
        vst1_f32(ptr, vget_low_f32(v));
        vst1q_lane_f32(ptr + 2, v, 2);
    }

    // Horizontal sum of all 4 floats in float32x4_t -> scalar
    inline float t3d_hsum_f32(float32x4_t v)
    {
        float32x2_t sum = vadd_f32(vget_low_f32(v), vget_high_f32(v));
        sum = vpadd_f32(sum, sum);
        return vget_lane_f32(sum, 0);
    }

#endif


#endif  /*__T3D_SIMD_CONFIG_H__*/
