/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4RenderState.h"


namespace Tiny3D
{
    NullGL4BlendStatePtr NullGL4BlendState::create() { return T3D_NEW NullGL4BlendState(); }
    NullGL4BlendState::~NullGL4BlendState() {}
    void *NullGL4BlendState::getNativeObject() const { return nullptr; }

    NullGL4RasterizerStatePtr NullGL4RasterizerState::create() { return T3D_NEW NullGL4RasterizerState(); }
    NullGL4RasterizerState::~NullGL4RasterizerState() {}
    void *NullGL4RasterizerState::getNativeObject() const { return nullptr; }

    NullGL4DepthStencilStatePtr NullGL4DepthStencilState::create() { return T3D_NEW NullGL4DepthStencilState(); }
    NullGL4DepthStencilState::~NullGL4DepthStencilState() {}
    void *NullGL4DepthStencilState::getNativeObject() const { return nullptr; }

    NullGL4SamplerStatePtr NullGL4SamplerState::create() { return T3D_NEW NullGL4SamplerState(); }
    NullGL4SamplerState::~NullGL4SamplerState() {}
    void *NullGL4SamplerState::getNativeObject() const { return nullptr; }
}
