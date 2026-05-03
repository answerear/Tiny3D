/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleRenderState.h"


namespace Tiny3D
{
    GL4ConsoleBlendStatePtr GL4ConsoleBlendState::create() { return T3D_NEW GL4ConsoleBlendState(); }
    GL4ConsoleBlendState::~GL4ConsoleBlendState() {}
    void *GL4ConsoleBlendState::getNativeObject() const { return nullptr; }

    GL4ConsoleRasterizerStatePtr GL4ConsoleRasterizerState::create() { return T3D_NEW GL4ConsoleRasterizerState(); }
    GL4ConsoleRasterizerState::~GL4ConsoleRasterizerState() {}
    void *GL4ConsoleRasterizerState::getNativeObject() const { return nullptr; }

    GL4ConsoleDepthStencilStatePtr GL4ConsoleDepthStencilState::create() { return T3D_NEW GL4ConsoleDepthStencilState(); }
    GL4ConsoleDepthStencilState::~GL4ConsoleDepthStencilState() {}
    void *GL4ConsoleDepthStencilState::getNativeObject() const { return nullptr; }

    GL4ConsoleSamplerStatePtr GL4ConsoleSamplerState::create() { return T3D_NEW GL4ConsoleSamplerState(); }
    GL4ConsoleSamplerState::~GL4ConsoleSamplerState() {}
    void *GL4ConsoleSamplerState::getNativeObject() const { return nullptr; }
}
