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


#include "Resource/T3DShaderManager.h"
#include "Resource/T3DShader.h"
#include "Kernel/T3DAgent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderManagerPtr ShaderManager::create()
    {
        return new ShaderManager();
    }

    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(ArchivePtr archive, const String &name)
    {
        return smart_pointer_cast<Shader>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(const String &name)
    {
        ArchivePtr archive = T3D_AGENT.getProjectArchive();
        return smart_pointer_cast<Shader>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(const String &name, CompletedCallback callback)
    {
        ShaderPtr shader;

        return shader;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ShaderManager::create(const String &name, int32_t argc, va_list args)
    {
        return Shader::create(name);
    }

    //--------------------------------------------------------------------------
}

