/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#include "Material/T3DShaderSamplerParam.h"
#include "Kernel/T3DAgent.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderSamplerParamPtr ShaderSamplerParam::create(const String &name, TEXTURE_TYPE texType)
    {
        return new ShaderSamplerParam(name, texType);
    }
    
    //--------------------------------------------------------------------------

    ShaderSamplerParam::ShaderSamplerParam(const String &name, TEXTURE_TYPE texType)
        : mName(name)
        , mTexType(texType)
    {
        ArchivePtr archive = T3D_AGENT.getProjectArchive();
        mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::~ShaderSamplerParam()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParamPtr ShaderSamplerParam::clone() const
    {
        return create(mName, mTexType);
    }

    //--------------------------------------------------------------------------

    bool ShaderSamplerParam::onPostLoad()
    {
        ArchivePtr archive = T3D_AGENT.getProjectArchive();
        mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
        return true;
    }

    //--------------------------------------------------------------------------
}


