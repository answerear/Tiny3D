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


#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "T3DD3D9HardwareBufferManager.h"
#include "T3DD3D9Error.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(D3D9Renderer);

    //--------------------------------------------------------------------------

    D3D9RendererPtr D3D9Renderer::create()
    {
        D3D9RendererPtr renderer = new D3D9Renderer();
        renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    D3D9Renderer::D3D9Renderer()
    {
        mName = Renderer::DIRECT3D9;
    }

    //--------------------------------------------------------------------------

    D3D9Renderer::~D3D9Renderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            // 创建 IDirect3D9 对象
            mD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
            if (nullptr == mD3D)
            {
                ret = T3D_ERR_D3D9_CREATE_FAILED;
                T3D_LOG_ERROR(LOG_TAG_D3D9RENDERER, "Create Direct3D9 \
                    interface failed !");
                break;
            }

            // 输出设备信息
            UINT uAdapter = 0;
            for (uAdapter = 0; uAdapter < mD3D->GetAdapterCount(); ++uAdapter)
            {
                D3DADAPTER_IDENTIFIER9 d3dai;
                D3DDISPLAYMODE d3ddm;
                mD3D->GetAdapterIdentifier(uAdapter, 0, &d3dai);
                mD3D->GetAdapterDisplayMode(uAdapter, &d3ddm);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            D3D_SAFE_RELEASE(mD3D);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr D3D9Renderer::createRenderWindow(const String &name,
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        TResult ret = T3D_OK;

        RenderWindowPtr window = nullptr;

        do
        {
            window = D3D9RenderWindow::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D9RENDERER, "Create render window \
                    failed !");
                break;
            }

            ret = window->create(param, paramEx);
            if (ret != T3D_OK)
            {
                window->release();
                window = nullptr;
                break;
            }

        } while (0);

        return window;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::beginRender(size_t count, Rect *rects,
        uint32_t clearFlags, const ColorRGB &color, Real z, uint32_t stencil)
    {
        TResult ret = T3D_OK;

        do 
        {
            Color4 clr;
            clr.from(color);
            HRESULT hr = S_OK;
            if (FAILED(hr = mD3DDevice->Clear(0, NULL, 
                D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clr.A8R8G8B8(), 1.0f, 0)))
            {

            }

            if (FAILED(hr = mD3DDevice->BeginScene()))
            {

            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::endRender()
    {
        TResult ret = T3D_OK;

        do 
        {
            HRESULT hr = S_OK;

            if (FAILED(hr = mD3DDevice->EndScene()))
            {

            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool D3D9Renderer::queryCapability(Capability cap) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &D3D9Renderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::perspective(Real left, Real right,
        Real top, Real bottom, Real nearDist, Real farDist)
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::orthographic(Real left, Real right,
        Real top, Real bottom, Real nearDist, Real farDist)
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::makeViewportMatrix(ViewportPtr viewport)
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::updateFrustum(const Matrix4 &m, FrustumBoundPtr bound)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setCullingMode(CullingMode mode)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setRenderMode(RenderMode mode)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setViewport(ViewportPtr viewport)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setMaterial(MaterialPtr material)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawVertexArray(VertexArrayObjectPtr vao)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawVertexList(PrimitiveType priType,
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawIndexList(PrimitiveType priType,
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo,
        HardwareIndexBufferPtr ibo)
    {
        TResult ret = T3D_OK;

        return ret;
    }
}

