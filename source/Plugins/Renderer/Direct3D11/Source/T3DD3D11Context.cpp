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


#include "T3DD3D11Context.h"
#include "T3DD3D11Window.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ContextPtr D3D11Context::create()
    {
        D3D11ContextPtr renderer = new D3D11Context();
        // renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    D3D11Context::D3D11Context()
        : mInstance(nullptr)
    {
        mName = RHIContext::DIRECT3D11;
    }

    //--------------------------------------------------------------------------

    D3D11Context::~D3D11Context()
    {
        destroy();
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            mRenderTarget = nullptr;
            mPrimaryWindow = nullptr;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::renderAllTargets()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = RHIContext::renderAllTargets();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    RHIRenderWindowPtr D3D11Context::createRenderWindow(const String &name,
        const RenderWindowCreateParam &param)
    {
        TResult ret = T3D_OK;

        RHIRenderWindowPtr window = nullptr;

        do
        {
            window = D3D11Window::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, 
                    "Create render window failed !");
                break;
            }

            ret = window->create(param);
            if (T3D_FAILED(ret))
            {
                // window->release();
                // window = nullptr;
                break;
            }

            if (mPrimaryWindow == nullptr)
            {
                mPrimaryWindow = window;
            }
            else
            {

            }
        } while (0);

        return window;
    }

    //--------------------------------------------------------------------------
}

