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

#ifndef __SOFT_PARTICLE_SHADERS_H__
#define __SOFT_PARTICLE_SHADERS_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    enum class SPShader : uint32_t
    {
        /// Opaque camp lit by the flickering fire and a dim moon
        kSceneVS = 0,
        kScenePS,
        /// Camera facing particles, faded by the distance to the opaque depth when soft
        kParticleVS,
        kParticlePS,
    };

    /// Name of the constant buffer the scene shaders read; GL matches uniform blocks by this name
    extern const char *const kSPSceneParamsName;
    /// Name of the constant buffer the particle shaders read
    extern const char *const kSPParticleParamsName;

    /**
     * \brief Compile and reflect one of the sample's shaders for the active renderer (D3D11 HLSL / GL 4.3 GLSL)
     * \param [in] id : which shader
     * \param [out] dummyShader : owner of the dummy pass the variant hangs on, keep it alive with the variant
     * \return nullptr when the renderer has no source or compilation fails
     */
    ShaderVariantPtr createSPShader(SPShader id, ShaderPtr &dummyShader);
}


#endif  /*__SOFT_PARTICLE_SHADERS_H__*/
