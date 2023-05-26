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

#ifndef __T3D_D3D11_RENDERER_H__
#define __T3D_D3D11_RENDERER_H__


#include "T3DD3D11Prerequisites.h"


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
        
        virtual RHIRenderWindowPtr createRenderWindow(
           const String &name, const RenderWindowCreateParam &param) override;

    protected:
        D3D11Renderer();
        
        void cleanup();
    };

    #define D3D11_RENDERER      (D3D11Renderer::getInstance())
}


#endif    /*__T3D_D3D11_RENDERER_H__*/
