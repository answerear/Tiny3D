/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_SHADER_H__
#define __T3D_GL4_SHADER_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    class GL4Shader : public RHIShader
    {
    public:
        GLuint GLShaderHandle {0};

    protected:
        GL4Shader() = default;
    };

    class GL4VertexShader : public GL4Shader
    {
    public:
        static GL4VertexShaderPtr create();
        ~GL4VertexShader() override;
        void *getNativeObject() const override;
    protected:
        GL4VertexShader() = default;
    };

    class GL4PixelShader : public GL4Shader
    {
    public:
        static GL4PixelShaderPtr create();
        ~GL4PixelShader() override;
        void *getNativeObject() const override;
    protected:
        GL4PixelShader() = default;
    };

    class GL4HullShader : public GL4Shader
    {
    public:
        static GL4HullShaderPtr create();
        ~GL4HullShader() override;
        void *getNativeObject() const override;
    protected:
        GL4HullShader() = default;
    };

    class GL4DomainShader : public GL4Shader
    {
    public:
        static GL4DomainShaderPtr create();
        ~GL4DomainShader() override;
        void *getNativeObject() const override;
    protected:
        GL4DomainShader() = default;
    };

    class GL4GeometryShader : public GL4Shader
    {
    public:
        static GL4GeometryShaderPtr create();
        ~GL4GeometryShader() override;
        void *getNativeObject() const override;
    protected:
        GL4GeometryShader() = default;
    };

    class GL4ComputeShader : public GL4Shader
    {
    public:
        static GL4ComputeShaderPtr create();
        ~GL4ComputeShader() override;
        void *getNativeObject() const override;
    protected:
        GL4ComputeShader() = default;
    };
}


#endif  /*__T3D_GL4_SHADER_H__*/
