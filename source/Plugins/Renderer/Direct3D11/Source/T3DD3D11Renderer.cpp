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


#include "T3DD3D11Renderer.h"
#include "T3DD3D11Context.h"
#include "T3DD3D11RenderWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11RendererPtr D3D11Renderer::create()
    {
        return new D3D11Renderer();
    }
    
    //--------------------------------------------------------------------------

    D3D11Renderer::D3D11Renderer()
    {
        mName = DIRECT3D11;
    }
    
    //--------------------------------------------------------------------------

    D3D11Renderer::~D3D11Renderer()
    {
        cleanup();
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Renderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            mContext = D3D11Context::create();
            // ret = mContext->init();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            cleanup();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void D3D11Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}


