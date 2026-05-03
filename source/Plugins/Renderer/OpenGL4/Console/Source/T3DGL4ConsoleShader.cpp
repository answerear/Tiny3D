/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleShader.h"


namespace Tiny3D
{
    GL4ConsoleVertexShaderPtr GL4ConsoleVertexShader::create() { return T3D_NEW GL4ConsoleVertexShader(); }
    GL4ConsoleVertexShader::~GL4ConsoleVertexShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsoleVertexShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    GL4ConsoleHullShaderPtr GL4ConsoleHullShader::create() { return T3D_NEW GL4ConsoleHullShader(); }
    GL4ConsoleHullShader::~GL4ConsoleHullShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsoleHullShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    GL4ConsoleDomainShaderPtr GL4ConsoleDomainShader::create() { return T3D_NEW GL4ConsoleDomainShader(); }
    GL4ConsoleDomainShader::~GL4ConsoleDomainShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsoleDomainShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    GL4ConsoleGeometryShaderPtr GL4ConsoleGeometryShader::create() { return T3D_NEW GL4ConsoleGeometryShader(); }
    GL4ConsoleGeometryShader::~GL4ConsoleGeometryShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsoleGeometryShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    GL4ConsolePixelShaderPtr GL4ConsolePixelShader::create() { return T3D_NEW GL4ConsolePixelShader(); }
    GL4ConsolePixelShader::~GL4ConsolePixelShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsolePixelShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }

    GL4ConsoleComputeShaderPtr GL4ConsoleComputeShader::create() { return T3D_NEW GL4ConsoleComputeShader(); }
    GL4ConsoleComputeShader::~GL4ConsoleComputeShader() { GL_SAFE_DELETE_SHADER(GLShaderHandle); }
    void *GL4ConsoleComputeShader::getNativeObject() const { return (void *)(uintptr_t)GLShaderHandle; }
}
