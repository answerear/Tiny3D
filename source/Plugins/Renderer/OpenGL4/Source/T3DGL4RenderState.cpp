/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGL4RenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4BlendStatePtr GL4BlendState::create()
    {
        return T3D_NEW GL4BlendState();
    }

    void *GL4BlendState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------

    GL4DepthStencilStatePtr GL4DepthStencilState::create()
    {
        return T3D_NEW GL4DepthStencilState();
    }

    void *GL4DepthStencilState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------

    GL4RasterizerStatePtr GL4RasterizerState::create()
    {
        return T3D_NEW GL4RasterizerState();
    }

    void *GL4RasterizerState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------

    GL4SamplerStatePtr GL4SamplerState::create()
    {
        return T3D_NEW GL4SamplerState();
    }

    GL4SamplerState::~GL4SamplerState()
    {
        GL_SAFE_DELETE_SAMPLER(GLSampler);
    }

    void *GL4SamplerState::getNativeObject() const
    {
        return (void *)(uintptr_t)GLSampler;
    }

    //--------------------------------------------------------------------------
}
