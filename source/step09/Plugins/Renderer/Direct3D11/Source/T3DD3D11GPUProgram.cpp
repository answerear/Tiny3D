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
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
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
        , mVertexShader(nullptr)
        , mPixelShader(nullptr)
        , mHasLinked(false)
    {

    }

    //--------------------------------------------------------------------------

    D3D11GPUProgram::~D3D11GPUProgram()
    {
        mVertexShader = nullptr;
        mPixelShader = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D11GPUProgram::link(ShaderPtr vertexShader, ShaderPtr pixelShader)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (!vertexShader->hasCompiled() || !pixelShader->hasCompiled())
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Shader has not compiled !");
                ret = T3D_ERR_D3D11_SHADER_NOT_COMPILED;
                break;
            }

            mVertexShader = vertexShader;
            mPixelShader = pixelShader;
            mHasLinked = true;
        } while (0);

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
        program->mVertexShader = mVertexShader;
        program->mPixelShader = mPixelShader;
        return program;
    }
}