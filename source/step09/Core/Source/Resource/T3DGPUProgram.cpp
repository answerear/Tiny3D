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


#include "Resource/T3DGPUProgram.h"
#include "Resource/T3DGPUProgramManager.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderParamPtr ShaderParam::create()
    {
        ShaderParamPtr param = new ShaderParam();
        param->release();
        return param;
    }

    //--------------------------------------------------------------------------

    ShaderParam::ShaderParam()
    {

    }

    //--------------------------------------------------------------------------

    ShaderParam::~ShaderParam()
    {

    }

    //--------------------------------------------------------------------------

    Shader::Shader(const String &name)
        : Resource(name)
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Shader::getType() const
    {
        return E_RT_SHADER;
    }

    //--------------------------------------------------------------------------

    GPUProgram::GPUProgram(const String &name)
        : Resource(name)
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type GPUProgram::getType() const
    {
        return E_RT_GPUPROGRAM;
    }

    //--------------------------------------------------------------------------

    TResult GPUProgram::addShader(
        const String &name, Shader::ShaderType type, ShaderPtr &shader)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 创建对应平台的 shader 对象
            shader = T3D_SHADER_MGR.loadShader(type, name);
            if (shader == nullptr)
            {
                ret = T3D_ERR_RES_CREATE_SHADER;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Create vertex shader for %s failed !", name.c_str());
                break;
            }

            auto r = mShaders.insert(ShadersValue(type, shader));
            if (!r.second)
            {
                ret = T3D_ERR_GPU_DUPLICATED_STAGE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Duplicated stage of shader !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GPUProgram::removeShader(Shader::ShaderType type)
    {
        TResult ret = T3D_OK;

        do 
        {
            auto itr = mShaders.find(type);
            if (itr == mShaders.end())
            {
                ret = T3D_ERR_GPU_NONEXISTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "The type [%d] of shader is nonexistent !", type);
                break;
            }

            mShaders.erase(itr);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GPUProgram::removeShader(const String &name)
    {
        TResult ret = T3D_OK;

        do 
        {
            bool found = false;
            auto itr = mShaders.begin();
            while (itr != mShaders.end())
            {
                ShaderPtr shader = itr->second;
                if (shader->getName() == name)
                {
                    mShaders.erase(itr);
                    found = true;
                    break;
                }

                ++itr;
            }

            if (!found)
            {
                ret = T3D_ERR_GPU_NONEXISTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "The name [%s] of shader is nonexistent !", name.c_str());
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    ShaderPtr GPUProgram::getVertexShader() const
    {
        return mShaders.at(Shader::E_ST_VERTEX_SHADER);
    }

    //--------------------------------------------------------------------------

    ShaderPtr GPUProgram::getPixelShader() const
    {
        return mShaders.at(Shader::E_ST_PIXEL_SHADER);
    }

    //--------------------------------------------------------------------------

    TResult GPUProgram::cloneProperties(GPUProgramPtr newObj) const
    {
        TResult ret = T3D_OK;

        do 
        {
            newObj->mShaders = mShaders;
        } while (0);

        return ret;
    }
}

