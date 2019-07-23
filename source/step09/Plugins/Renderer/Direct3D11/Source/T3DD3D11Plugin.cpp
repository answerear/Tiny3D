﻿/*******************************************************************************
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


#include "T3DD3D11Plugin.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11Plugin::D3D11Plugin()
        : mName("D3D11Renderer")
        , mRenderer(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    D3D11Plugin::~D3D11Plugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &D3D11Plugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Plugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = D3D11Renderer::create();
            if (mRenderer != nullptr)
            {
                ret = T3D_AGENT.addRenderer(mRenderer);
                if (ret != T3D_OK)
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Plugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Plugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Plugin::uninstall()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = T3D_AGENT.removeRenderer(mRenderer);
            if (ret != T3D_OK)
            {
                break;
            }

            mRenderer = nullptr;
        } while (0);

        return ret;
    }
}
