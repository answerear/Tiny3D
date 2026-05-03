/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4CONSOLE_SHADER_H__
#define __T3D_GL4CONSOLE_SHADER_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsoleShader : public RHIShader
    {
    public:
        GLuint GLShaderHandle {0};

    protected:
        GL4ConsoleShader() = default;
    };

    class GL4ConsoleVertexShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsoleVertexShaderPtr create();
        ~GL4ConsoleVertexShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleVertexShader() = default;
    };

    class GL4ConsoleHullShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsoleHullShaderPtr create();
        ~GL4ConsoleHullShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleHullShader() = default;
    };

    class GL4ConsoleDomainShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsoleDomainShaderPtr create();
        ~GL4ConsoleDomainShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleDomainShader() = default;
    };

    class GL4ConsoleGeometryShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsoleGeometryShaderPtr create();
        ~GL4ConsoleGeometryShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleGeometryShader() = default;
    };

    class GL4ConsolePixelShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsolePixelShaderPtr create();
        ~GL4ConsolePixelShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsolePixelShader() = default;
    };

    class GL4ConsoleComputeShader : public GL4ConsoleShader
    {
    public:
        static GL4ConsoleComputeShaderPtr create();
        ~GL4ConsoleComputeShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ConsoleComputeShader() = default;
    };
}


#endif  /*__T3D_GL4CONSOLE_SHADER_H__*/
