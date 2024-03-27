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


#ifndef __T3D_SAMPLER_STATE_H__
#define __T3D_SAMPLER_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 纹理采样状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API SamplerDesc
    {
        TPROPERTY()
        FilterOptions       MinFilter {FilterOptions::kLinear};
        
        TPROPERTY()
        FilterOptions       MagFilter {FilterOptions::kLinear};
        
        TPROPERTY()
        FilterOptions       MipFilter {FilterOptions::kLinear};
        
        TPROPERTY()
        TextureAddressMode  AddressU {TextureAddressMode::kWrap};
        
        TPROPERTY()
        TextureAddressMode  AddressV {TextureAddressMode::kWrap};
        
        TPROPERTY()
        TextureAddressMode  AddressW {TextureAddressMode::kWrap};
        
        TPROPERTY()
        Real                MipLODBias {0.0f};
        
        TPROPERTY()
        uint32_t            MaxAnisotropy {1};
        
        TPROPERTY()
        CompareFunction     CompareFunc {CompareFunction::kAlwaysFail};
        
        TPROPERTY()
        ColorRGBA           BorderColor {ColorRGBA::WHITE};
        
        TPROPERTY()
        Real                MinLOD {0.0f};
        
        TPROPERTY()
        Real                MaxLOD {FLT_MAX};
    };

    template class T3D_ENGINE_API RenderStateResource<SamplerDesc, RHISamplerState>;
    
    class T3D_ENGINE_API SamplerState : public RenderStateResource<SamplerDesc, RHISamplerState>
    {
    public:
        static SamplerStatePtr create(const SamplerDesc &desc, uint32_t hash = 0);
        
        Type getType() const override;
        
    protected:
        SamplerState(uint32_t hash, const SamplerDesc &desc);

        ~SamplerState() override = default;
        
        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_SAMPLER_STATE_H__*/
