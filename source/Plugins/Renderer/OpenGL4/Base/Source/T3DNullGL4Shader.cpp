/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4Shader.h"


namespace Tiny3D
{
    NullGL4VertexShaderPtr NullGL4VertexShader::create() { return T3D_NEW NullGL4VertexShader(); }
    NullGL4VertexShader::~NullGL4VertexShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4VertexShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    NullGL4HullShaderPtr NullGL4HullShader::create() { return T3D_NEW NullGL4HullShader(); }
    NullGL4HullShader::~NullGL4HullShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4HullShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    NullGL4DomainShaderPtr NullGL4DomainShader::create() { return T3D_NEW NullGL4DomainShader(); }
    NullGL4DomainShader::~NullGL4DomainShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4DomainShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    NullGL4GeometryShaderPtr NullGL4GeometryShader::create() { return T3D_NEW NullGL4GeometryShader(); }
    NullGL4GeometryShader::~NullGL4GeometryShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4GeometryShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    NullGL4PixelShaderPtr NullGL4PixelShader::create() { return T3D_NEW NullGL4PixelShader(); }
    NullGL4PixelShader::~NullGL4PixelShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4PixelShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    NullGL4ComputeShaderPtr NullGL4ComputeShader::create() { return T3D_NEW NullGL4ComputeShader(); }
    NullGL4ComputeShader::~NullGL4ComputeShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *NullGL4ComputeShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }
}
