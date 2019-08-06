/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DD3D11GPUProgram.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ShaderPtr D3D11Shader::create(const String &name)
    {
        D3D11ShaderPtr shader = new D3D11Shader(name);
        shader->release();
        return shader;
    }

    //--------------------------------------------------------------------------

    D3D11Shader::D3D11Shader(const String &name)
        : Shader(name)
        , mHasCompiled(false)
    {

    }

    //--------------------------------------------------------------------------

    D3D11Shader::~D3D11Shader()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D11Shader::compile()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    bool D3D11Shader::hasCompiled() const
    {
        return mHasCompiled;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Shader::load()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Shader::unload()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr D3D11Shader::clone() const
    {
        ShaderPtr shader = create(getName());
        return shader;
    }

    //--------------------------------------------------------------------------

    D3D11GPUProgramPtr D3D11GPUProgram::create(const String &name)
    {
        D3D11GPUProgramPtr program = new D3D11GPUProgram(name);
        program->release();
        return program;
    }

    //--------------------------------------------------------------------------

    D3D11GPUProgram::D3D11GPUProgram(const String &name)
        : GPUProgram(name)
        , mHasLinked(false)
    {

    }

    //--------------------------------------------------------------------------

    D3D11GPUProgram::~D3D11GPUProgram()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D11GPUProgram::link(ShaderPtr vertexShader, ShaderPtr pixelShader)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    bool D3D11GPUProgram::hasLinked() const
    {
        return mHasLinked;
    }

    //--------------------------------------------------------------------------

    TResult D3D11GPUProgram::load()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11GPUProgram::unload()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr D3D11GPUProgram::clone() const
    {
        D3D11GPUProgramPtr program = create(getName());
        return program;
    }
}