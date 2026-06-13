/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_SHADER_H__
#define __T3D_GLES3_SHADER_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    class GLES3Shader : public RHIShader
    {
    public:
        GLuint GLShaderHandle {0};

    protected:
        GLES3Shader() = default;
    };

    class GLES3VertexShader : public GLES3Shader
    {
    public:
        static GLES3VertexShaderPtr create();
        ~GLES3VertexShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3VertexShader() = default;
    };

    class GLES3PixelShader : public GLES3Shader
    {
    public:
        static GLES3PixelShaderPtr create();
        ~GLES3PixelShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3PixelShader() = default;
    };

    class GLES3HullShader : public GLES3Shader
    {
    public:
        static GLES3HullShaderPtr create();
        ~GLES3HullShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3HullShader() = default;
    };

    class GLES3DomainShader : public GLES3Shader
    {
    public:
        static GLES3DomainShaderPtr create();
        ~GLES3DomainShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3DomainShader() = default;
    };

    class GLES3GeometryShader : public GLES3Shader
    {
    public:
        static GLES3GeometryShaderPtr create();
        ~GLES3GeometryShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3GeometryShader() = default;
    };

    class GLES3ComputeShader : public GLES3Shader
    {
    public:
        static GLES3ComputeShaderPtr create();
        ~GLES3ComputeShader() override;
        void *getNativeObject() const override;
    protected:
        GLES3ComputeShader() = default;
    };
}


#endif  /*__T3D_GLES3_SHADER_H__*/
