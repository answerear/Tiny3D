/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_NULLGL4_SHADER_H__
#define __T3D_NULLGL4_SHADER_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4Shader : public RHIShader
    {
    public:
        GLuint GLShaderHandle {0};

    protected:
        NullGL4Shader() = default;
    };

    class NullGL4VertexShader : public NullGL4Shader
    {
    public:
        static NullGL4VertexShaderPtr create();
        ~NullGL4VertexShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4VertexShader() = default;
    };

    class NullGL4HullShader : public NullGL4Shader
    {
    public:
        static NullGL4HullShaderPtr create();
        ~NullGL4HullShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4HullShader() = default;
    };

    class NullGL4DomainShader : public NullGL4Shader
    {
    public:
        static NullGL4DomainShaderPtr create();
        ~NullGL4DomainShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4DomainShader() = default;
    };

    class NullGL4GeometryShader : public NullGL4Shader
    {
    public:
        static NullGL4GeometryShaderPtr create();
        ~NullGL4GeometryShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4GeometryShader() = default;
    };

    class NullGL4PixelShader : public NullGL4Shader
    {
    public:
        static NullGL4PixelShaderPtr create();
        ~NullGL4PixelShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4PixelShader() = default;
    };

    class NullGL4ComputeShader : public NullGL4Shader
    {
    public:
        static NullGL4ComputeShaderPtr create();
        ~NullGL4ComputeShader() override;
        void *getNativeObject() const override;
    protected:
        NullGL4ComputeShader() = default;
    };
}


#endif  /*__T3D_NULLGL4_SHADER_H__*/
