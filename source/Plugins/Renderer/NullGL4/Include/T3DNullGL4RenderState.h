/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_NULLGL4_RENDER_STATE_H__
#define __T3D_NULLGL4_RENDER_STATE_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4BlendState : public RHIBlendState
    {
    public:
        static NullGL4BlendStatePtr create();
        ~NullGL4BlendState() override;
        void *getNativeObject() const override;
    protected:
        NullGL4BlendState() = default;
    };

    class NullGL4RasterizerState : public RHIRasterizerState
    {
    public:
        static NullGL4RasterizerStatePtr create();
        ~NullGL4RasterizerState() override;
        void *getNativeObject() const override;
    protected:
        NullGL4RasterizerState() = default;
    };

    class NullGL4DepthStencilState : public RHIDepthStencilState
    {
    public:
        static NullGL4DepthStencilStatePtr create();
        ~NullGL4DepthStencilState() override;
        void *getNativeObject() const override;
    protected:
        NullGL4DepthStencilState() = default;
    };

    class NullGL4SamplerState : public RHISamplerState
    {
    public:
        static NullGL4SamplerStatePtr create();
        ~NullGL4SamplerState() override;
        void *getNativeObject() const override;
    protected:
        NullGL4SamplerState() = default;
    };
}


#endif    /*__T3D_NULLGL4_RENDER_STATE_H__*/
