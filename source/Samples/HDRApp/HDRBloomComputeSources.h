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

#ifndef __HDR_BLOOM_COMPUTE_SOURCES_H__
#define __HDR_BLOOM_COMPUTE_SOURCES_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    enum class HDRBloomKernel : uint32_t
    {
        /// t0 : finer mip (SRV), u0 : next mip (UAV), 2x2 box
        kDownsample = 0,
        /// t0 : this level of the down chain, t1 : coarser level, u0 : this level of the up chain
        kUpsample,
    };

    /**
     * \brief Compile a bloom compute kernel for the active renderer (D3D11 HLSL / GL 4.3 GLSL)
     * \param [in] kernel : which kernel
     * \param [in] format : pixel format of the UAV, GLSL needs it as the image format qualifier
     * \param [out] dummyShader : owner of the dummy pass the variant hangs on, keep it alive with the variant
     * \return nullptr when the renderer has no source or compilation fails
     */
    ShaderVariantPtr createHDRBloomKernel(HDRBloomKernel kernel, PixelFormat format, ShaderPtr &dummyShader);
}


#endif  /*__HDR_BLOOM_COMPUTE_SOURCES_H__*/
