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


#include "Resource/T3DGPUProgramManager.h"
#include "Resource/T3DGPUProgramCreator.h"
#include "Resource/T3DGPUProgram.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(ShaderManager);

    //--------------------------------------------------------------------------

    ShaderManagerPtr ShaderManager::create()
    {
        ShaderManagerPtr mgr = new ShaderManager();
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    ShaderManager::ShaderManager()
        : ResourceManager()
        , mCreator(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    ShaderManager::~ShaderManager()
    {

    }

    //--------------------------------------------------------------------------

    void ShaderManager::setShaderCreator(ShaderCreator *creator)
    {
        mCreator = creator;
    }

    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(const String &name)
    {
        return smart_pointer_cast<Shader>(ResourceManager::load(name, 0));
    }

    //--------------------------------------------------------------------------

    void ShaderManager::unloadShader(ShaderPtr &shader)
    {
        unload((ResourcePtr &)shader);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ShaderManager::create(const String &name, int32_t argc,
        va_list args)
    {
        if (mCreator == nullptr)
        {
            return nullptr;
        }

        ShaderPtr shader = mCreator->createObject(1, name.c_str());
        return shader;
    }


    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(GPUProgramManager);

    //--------------------------------------------------------------------------

    GPUProgramManagerPtr GPUProgramManager::create()
    {
        GPUProgramManagerPtr mgr = new GPUProgramManager();
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    GPUProgramManager::GPUProgramManager()
        : ResourceManager()
        , mCreator(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    GPUProgramManager::~GPUProgramManager()
    {

    }

    //--------------------------------------------------------------------------

    void GPUProgramManager::setGPUProgramCreator(GPUProgramCreator *creator)
    {
        mCreator = creator;
    }

    //--------------------------------------------------------------------------

    GPUProgramPtr GPUProgramManager::loadGPUProgram(const String &name)
    {
        return smart_pointer_cast<GPUProgram>(ResourceManager::load(name, 0));
    }

    //--------------------------------------------------------------------------

    void GPUProgramManager::unloadGPUProgram(GPUProgramPtr &program)
    {
        unload((ResourcePtr &)program);
    }

    //--------------------------------------------------------------------------

    ResourcePtr GPUProgramManager::create(const String &name, int32_t argc,
        va_list args)
    {
        if (mCreator == nullptr)
        {
            return nullptr;
        }

        GPUProgramPtr program = mCreator->createObject(1, name.c_str());
        return program;
    }
}

