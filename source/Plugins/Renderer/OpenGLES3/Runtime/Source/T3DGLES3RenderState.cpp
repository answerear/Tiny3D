/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3RenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // GLES3BlendState
    //--------------------------------------------------------------------------

    GLES3BlendStatePtr GLES3BlendState::create()
    {
        return T3D_NEW GLES3BlendState();
    }

    void *GLES3BlendState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------
    // GLES3DepthStencilState
    //--------------------------------------------------------------------------

    GLES3DepthStencilStatePtr GLES3DepthStencilState::create()
    {
        return T3D_NEW GLES3DepthStencilState();
    }

    void *GLES3DepthStencilState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------
    // GLES3RasterizerState
    //--------------------------------------------------------------------------

    GLES3RasterizerStatePtr GLES3RasterizerState::create()
    {
        return T3D_NEW GLES3RasterizerState();
    }

    void *GLES3RasterizerState::getNativeObject() const
    {
        return (void *)&data;
    }

    //--------------------------------------------------------------------------
    // GLES3SamplerState
    //--------------------------------------------------------------------------

    GLES3SamplerStatePtr GLES3SamplerState::create()
    {
        return T3D_NEW GLES3SamplerState();
    }

    GLES3SamplerState::~GLES3SamplerState()
    {
        GL_SAFE_DELETE_SAMPLER(GLSampler);
    }

    void *GLES3SamplerState::getNativeObject() const
    {
        return (void *)(intptr_t)GLSampler;
    }
}
