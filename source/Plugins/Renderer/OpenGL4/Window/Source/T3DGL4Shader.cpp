/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGL4Shader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4VertexShaderPtr GL4VertexShader::create()
    {
        return T3D_NEW GL4VertexShader();
    }

    GL4VertexShader::~GL4VertexShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4VertexShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------

    GL4PixelShaderPtr GL4PixelShader::create()
    {
        return T3D_NEW GL4PixelShader();
    }

    GL4PixelShader::~GL4PixelShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4PixelShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------

    GL4HullShaderPtr GL4HullShader::create()
    {
        return T3D_NEW GL4HullShader();
    }

    GL4HullShader::~GL4HullShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4HullShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------

    GL4DomainShaderPtr GL4DomainShader::create()
    {
        return T3D_NEW GL4DomainShader();
    }

    GL4DomainShader::~GL4DomainShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4DomainShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------

    GL4GeometryShaderPtr GL4GeometryShader::create()
    {
        return T3D_NEW GL4GeometryShader();
    }

    GL4GeometryShader::~GL4GeometryShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4GeometryShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------

    GL4ComputeShaderPtr GL4ComputeShader::create()
    {
        return T3D_NEW GL4ComputeShader();
    }

    GL4ComputeShader::~GL4ComputeShader()
    {
        GL_SAFE_DELETE_SHADER(GLShaderHandle);
    }

    void *GL4ComputeShader::getNativeObject() const
    {
        return (void *)(uintptr_t)GLShaderHandle;
    }

    //--------------------------------------------------------------------------
}
