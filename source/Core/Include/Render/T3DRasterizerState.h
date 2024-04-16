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


#ifndef __T3D_RASTERIZER_STATE_H__
#define __T3D_RASTERIZER_STATE_H__


#include "T3DConfig.h"
#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 光栅化状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API RasterizerDesc
    {
        TPROPERTY()
        PolygonMode FillMode = PolygonMode::kSolid;
        
        TPROPERTY()
        CullingMode CullMode = CullingMode::kBack;

        TPROPERTY()
#if (T3D_COORDINATION_RH)
        bool        FrontAnticlockwise = true;
#else
        bool        FrontAnticlockwise = false;
#endif
        
        TPROPERTY()
        Real        DepthBias = 0.0f;
        
        TPROPERTY()
        Real        DepthBiasClamp = 0.0f;
        
        TPROPERTY()
        Real        SlopeScaledDepthBias = 0.0f;
        
        TPROPERTY()
        bool        DepthClipEnable = true;
        
        TPROPERTY()
        bool        ScissorEnable = false;
        
        TPROPERTY()
        bool        MultisampleEnable = false;
        
        TPROPERTY()
        bool        AntialiasedLineEnable = false;

        TPROPERTY()
        bool        Conservative = false;
    };

    template class T3D_ENGINE_API RenderStateResource<RasterizerDesc, RHIRasterizerState>;
    
    class T3D_ENGINE_API RasterizerState : public RenderStateResource<RasterizerDesc, RHIRasterizerState>
    {
    public:
        static RasterizerStatePtr create(const RasterizerDesc &desc, uint32_t hash = 0);
        
        Type getType() const override;

    protected:
        RasterizerState(uint32_t hash, const RasterizerDesc &desc);

        ~RasterizerState() override = default;
        
        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_RASTERIZER_STATE_H__*/
