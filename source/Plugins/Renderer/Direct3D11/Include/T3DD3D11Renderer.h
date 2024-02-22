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

#ifndef __T3D_D3D11_RENDERER_H__
#define __T3D_D3D11_RENDERER_H__


#include "T3DD3D11Prerequisites.h"
#include "T3DD3D11Context.h"

namespace Tiny3D
{
    class D3D11Renderer
        : public RHIRenderer
        , public Singleton<D3D11Renderer>
    {
    public:
        static D3D11RendererPtr create();

        virtual ~D3D11Renderer() override;
        
        virtual TResult init() override;
        
        virtual TResult destroy() override;
        
    protected:
        D3D11Renderer();
        
        void cleanup();
    };

    #define D3D11_RENDERER      (D3D11Renderer::getInstance())
    #define D3D11_CONTEXT       (smart_pointer_cast<D3D11Context>(D3D11_RENDERER.getContext()))
}


#endif    /*__T3D_D3D11_RENDERER_H__*/
