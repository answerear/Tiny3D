/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3Shader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // GLES3VertexShader
    //--------------------------------------------------------------------------

    GLES3VertexShaderPtr GLES3VertexShader::create()
    {
        return T3D_NEW GLES3VertexShader();
    }

    GLES3VertexShader::~GLES3VertexShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3VertexShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
    // GLES3PixelShader
    //--------------------------------------------------------------------------

    GLES3PixelShaderPtr GLES3PixelShader::create()
    {
        return T3D_NEW GLES3PixelShader();
    }

    GLES3PixelShader::~GLES3PixelShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3PixelShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
    // GLES3HullShader
    //--------------------------------------------------------------------------

    GLES3HullShaderPtr GLES3HullShader::create()
    {
        return T3D_NEW GLES3HullShader();
    }

    GLES3HullShader::~GLES3HullShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3HullShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
    // GLES3DomainShader
    //--------------------------------------------------------------------------

    GLES3DomainShaderPtr GLES3DomainShader::create()
    {
        return T3D_NEW GLES3DomainShader();
    }

    GLES3DomainShader::~GLES3DomainShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3DomainShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
    // GLES3GeometryShader
    //--------------------------------------------------------------------------

    GLES3GeometryShaderPtr GLES3GeometryShader::create()
    {
        return T3D_NEW GLES3GeometryShader();
    }

    GLES3GeometryShader::~GLES3GeometryShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3GeometryShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
    // GLES3ComputeShader
    //--------------------------------------------------------------------------

    GLES3ComputeShaderPtr GLES3ComputeShader::create()
    {
        return T3D_NEW GLES3ComputeShader();
    }

    GLES3ComputeShader::~GLES3ComputeShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GLES3ComputeShader::getNativeObject() const
    {
        return (void *)(intptr_t)GLShaderHandle;
    }
}
