﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    const char * const Renderer::DIRECT3D9 = "Direct3D9";
    const char * const Renderer::DIRECT3D11 = "Direct3D11";
    const char * const Renderer::OPENGL3 = "OpenGL 3.x";
    const char * const Renderer::OPENGLES2 = "OpenGL ES 2";
    const char * const Renderer::OPENGLES3 = "OpenGL ES 3";
    const char * const Renderer::VULKAN = "Vulkan";
    const char * const Renderer::METAL = "Metal";

    //--------------------------------------------------------------------------

    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult Renderer::render()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Renderer::attachRenderTarget(RenderTargetPtr target)
    {
        TPair<RenderTargetListItr, bool> ret =
            mRenderTargets.insert(RenderTargetListValue(target->getName(),
                target));

        if (ret.second)
        {
            return T3D_OK;
        }

        return T3D_ERR_DUPLICATED_ITEM;
    }

    TResult Renderer::detachRenderTarget(const String &name)
    {
        auto itr = mRenderTargets.find(name);
        if (itr == mRenderTargets.end())
        {
            return T3D_ERR_NOT_FOUND;
        }

        mRenderTargets.erase(itr);

        return T3D_OK;
    }

    RenderTargetPtr Renderer::getRenderTarget(const String &name)
    {
        RenderTargetListItr itr = mRenderTargets.find(name);

        if (itr != mRenderTargets.end())
        {
            return itr->second;
        }

        return nullptr;
    }
}
