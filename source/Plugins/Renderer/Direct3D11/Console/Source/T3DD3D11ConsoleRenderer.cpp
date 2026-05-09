/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "T3DD3D11ConsoleRenderer.h"
#include "T3DD3D11ConsoleContext.h"
#include "T3DD3D11ConsoleWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11RendererConsolePtr D3D11RendererConsole::create()
    {
        return T3D_NEW D3D11RendererConsole();
    }
    
    //--------------------------------------------------------------------------

    D3D11RendererConsole::D3D11RendererConsole()
    {
        mName = DIRECT3D11_CONSOLE;
    }
    
    //--------------------------------------------------------------------------

    D3D11RendererConsole::~D3D11RendererConsole()
    {
        cleanup();
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11RendererConsole::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            mContext = D3D11ConsoleContext::create();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11RendererConsole::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            // Do NOT cleanup() here. Context/Device destruction is deferred
            // to ~D3D11RendererConsole() when mRenderers map is cleared.
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void D3D11RendererConsole::getEditorInfo(void *info, RenderWindow *window)
    {
        
    }

    //--------------------------------------------------------------------------

    void D3D11RendererConsole::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}


