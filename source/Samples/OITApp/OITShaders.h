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

#ifndef __OIT_SHADERS_H__
#define __OIT_SHADERS_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    enum class OITShader : uint32_t
    {
        /// Bar interior lit by the pendant lamps; vertices without a normal are drawn unlit
        kSceneVS = 0,
        kScenePS,
        /// Stained glass: SV_Target0 is the light the glass scatters, SV_Target1 the per channel transmittance
        kWindowPS,
        /// Stained glass without dual source blending: transmittance only, multiplied into the target
        kWindowFallbackPS,
        /// Display case glass, faded with the blend constant
        kCaseGlassPS,
    };

    /// Name of the constant buffer every shader reads; GL matches uniform blocks by this name
    extern const char *const kOITSceneParamsName;

    /// Number of point lights in OITSceneParams
    constexpr uint32_t kOITLightCount = 4;

    /**
     * \brief Compile and reflect one of the sample's shaders for the active renderer (D3D11 HLSL / GL 4.3 GLSL)
     * \param [in] id : which shader
     * \param [out] dummyShader : owner of the dummy pass the variant hangs on, keep it alive with the variant
     * \return nullptr when the renderer has no source or compilation fails
     */
    ShaderVariantPtr createOITShader(OITShader id, ShaderPtr &dummyShader);
}


#endif  /*__OIT_SHADERS_H__*/
