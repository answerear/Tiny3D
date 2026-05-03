/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4CONSOLE_RENDER_STATE_H__
#define __T3D_GL4CONSOLE_RENDER_STATE_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsoleBlendState : public RHIBlendState
    {
    public:
        static GL4ConsoleBlendStatePtr create();
        ~GL4ConsoleBlendState() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleBlendState() = default;
    };

    class GL4ConsoleRasterizerState : public RHIRasterizerState
    {
    public:
        static GL4ConsoleRasterizerStatePtr create();
        ~GL4ConsoleRasterizerState() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleRasterizerState() = default;
    };

    class GL4ConsoleDepthStencilState : public RHIDepthStencilState
    {
    public:
        static GL4ConsoleDepthStencilStatePtr create();
        ~GL4ConsoleDepthStencilState() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleDepthStencilState() = default;
    };

    class GL4ConsoleSamplerState : public RHISamplerState
    {
    public:
        static GL4ConsoleSamplerStatePtr create();
        ~GL4ConsoleSamplerState() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleSamplerState() = default;
    };
}


#endif    /*__T3D_GL4CONSOLE_RENDER_STATE_H__*/
