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

#ifndef __HDR_SHADER_SOURCES_H__
#define __HDR_SHADER_SOURCES_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    enum class HDRShader : uint32_t
    {
        kForwardVS = 0,
        kForwardPS,
        kShadowVS,
        kEmissiveVS,
        kEmissivePS,
        kPostVS,
        kBloomPrefilterPS,
        kBloomDownsamplePS,
        kBloomUpsamplePS,
        kTonemapPS,
    };

    /**
     * \brief Pick the embedded shader for the active renderer (HLSL / GLSL 4.5 / SPIR-V)
     * \return false when the platform or renderer has no embedded variant
     */
    bool getHDRShaderCode(HDRShader id, const char *&code, size_t &size);

    /**
     * \brief Create a shader variant with an empty keyword for the active renderer
     * \return nullptr when there is no embedded variant
     */
    ShaderVariantPtr createHDRShaderVariant(HDRShader id, SHADER_STAGE stage);
}


#endif  /*__HDR_SHADER_SOURCES_H__*/
