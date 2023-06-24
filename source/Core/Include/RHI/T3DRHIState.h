/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_RHI_STATE_H__
#define __T3D_RHI_STATE_H__


#include "T3DPrerequisites.h"
#include "T3DRHIConstant.h"


namespace Tiny3D
{
    /**
     * \brief 颜色混合状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API BlendState
    {
        TRTTI_ENABLE()
        
    public:
        enum
        {
            kMaxRenderTarget = 8
        };

        uint8_t *getCRCData(uint32_t &dataSize) const
        {
            dataSize = sizeof(BlendState) - sizeof(void*);
            return (uint8_t*)(&AlphaToCoverageEnable);
        }

        TSTRUCT()
        struct T3D_ENGINE_API RTBlendState
        {
            TRTTI_ENABLE()
            
        public:
            TPROPERTY()
            bool            BlendEnable = false;
            TPROPERTY()
            BlendFactor     SrcBlend = BlendFactor::kOne;
            TPROPERTY()
            BlendFactor     DestBlend = BlendFactor::kZero;
            TPROPERTY()
            BlendOperation  BlendOp = BlendOperation::kAdd;
            TPROPERTY()
            BlendFactor     SrcBlendAlpha = BlendFactor::kOne;
            TPROPERTY()
            BlendFactor     DstBlendAlpha = BlendFactor::kZero;
            TPROPERTY()
            BlendOperation  BlendOpAlpha = BlendOperation::kAdd;
            TPROPERTY()
            uint8_t         ColorMask = kWriteMaskAll;
        };

        TPROPERTY()
        bool    AlphaToCoverageEnable = false;
        TPROPERTY()
        bool    IndependentBlendEnable = false;
        TPROPERTY()
        RTBlendState    RenderTargetStates[kMaxRenderTarget]{};
    };

    /**
     * \brief 渲染硬件层的颜色混合状态
     */
    class T3D_ENGINE_API RHIBlendState : public Object
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }
        
        const BlendState &getState() const
        {
            return mState;
        }
        
    protected:
        RHIBlendState(const BlendState &state);

        uint32_t    mHash = 0;
        BlendState  mState {};
    };
    
    /**
     * \brief 深度缓冲和模板缓冲状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API DepthStencilState
    {
        TRTTI_ENABLE()

    public:
        uint8_t *getCRCData(uint32_t &dataSize) const
        {
            dataSize = sizeof(DepthStencilState) - sizeof(void*);
            return (uint8_t*)(&DepthTestEnable);
        }
        
        TSTRUCT()
        struct T3D_ENGINE_API StencilOpDesc
        {
            TRTTI_ENABLE()

        public:
            CompareFunction StencilFunc;
            StencilOp       StencilFailOp;
            StencilOp       StencilDepthFailOp;
            StencilOp       StencilPassOp;
        };

        TPROPERTY()
        bool            DepthTestEnable;
        TPROPERTY()
        bool            DepthWriteEnable;
        TPROPERTY()
        CompareFunction DepthFunc;

        TPROPERTY()
        bool            StencilEnable;
        TPROPERTY()
        uint32_t        StencilRef;
        TPROPERTY()
        uint8_t         StencilReadMask;
        TPROPERTY()
        uint8_t         StencilWriteMask;

        // Front face
        TPROPERTY()
        StencilOpDesc   FrontFace;
        
        // Back face
        TPROPERTY()
        StencilOpDesc   BackFace;
    };

    /**
     * \brief 渲染硬件的深度缓冲和模板缓冲状态
     */
    class T3D_ENGINE_API RHIDepthStencilState : public Object
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }

        const DepthStencilState &getState() const
        {
            return mState;
        }

    protected:
        RHIDepthStencilState(const DepthStencilState &state);
        
        uint32_t            mHash = 0;
        DepthStencilState   mState {};
    };

    /**
     * \brief 光栅化状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API RasterizerState
    {
        TRTTI_ENABLE()

    public:
        uint8_t *getCRCData(uint32_t &dataSize) const
        {
            dataSize = sizeof(RasterizerState) - sizeof(void*);
            return (uint8_t*)(&FillMode);
        }

        TPROPERTY()
        PolygonMode FillMode;
        TPROPERTY()
        CullingMode CullMode;
        TPROPERTY()
        Real        DepthBias;
        TPROPERTY()
        Real        DepthBiasClamp;
        TPROPERTY()
        Real        SlopeScaledDepthBias;
        TPROPERTY()
        bool        DepthClipEnable;
        TPROPERTY()
        bool        ScissorEnable;
        TPROPERTY()
        bool        MultisampleEnable;
        TPROPERTY()
        bool        AntialiasedLineEnable;
    };

    /**
     * \brief 渲染硬件层的光栅化状态
     */
    class T3D_ENGINE_API RHIRasterizerState : public Object
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }

        const RasterizerState &getState() const
        {
            return mState;
        }

    protected:
        RHIRasterizerState(const RasterizerState &state);
        
        uint32_t        mHash = 0;
        RasterizerState mState {};
    };

    /**
     * \brief 纹理采样状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API SamplerState
    {
        TRTTI_ENABLE()

    public:
        uint8_t *getCRCData(uint32_t &dataSize) const
        {
            dataSize = sizeof(SamplerState) - sizeof(void*);
            return (uint8_t*)(&MinFilter);
        }

        TPROPERTY()
        FilterOptions       MinFilter;
        TPROPERTY()
        FilterOptions       MagFilter;
        TPROPERTY()
        FilterOptions       MipFilter;
        TPROPERTY()
        TextureAddressMode  AddressU;
        TPROPERTY()
        TextureAddressMode  AddressV;
        TPROPERTY()
        TextureAddressMode  AddressW;
        TPROPERTY()
        Real                MipLODBias;
        TPROPERTY()
        uint32_t            MaxAnisotropy;
        TPROPERTY()
        CompareFunction     CompareFunc;
        TPROPERTY()
        ColorRGBA           BorderColor;
        TPROPERTY()
        Real                MinLOD;
        TPROPERTY()
        Real                MaxLOD;
    };

    /**
     * \brief 渲染硬件层的纹理采样状态
     */
    class T3D_ENGINE_API RHISamplerState : public Object
    {
    public:
        uint32_t hash() const
        {
            return mHash;
        }

        const SamplerState &getState() const
        {
            return mState;
        }

    protected:
        RHISamplerState(const SamplerState &state);

        uint32_t        mHash = 0;
        SamplerState    mState {};
    };
}


#endif  /*__T3D_RHI_STATE_H__*/
