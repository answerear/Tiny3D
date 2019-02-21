/*******************************************************************************
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


#include "T3DXRenderer.h"
#include "T3DXRenderWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3DXRendererPtr T3DXRenderer::create()
    {
        T3DXRendererPtr renderer = new T3DXRenderer();
        renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    T3DXRenderer::T3DXRenderer()
        : Renderer()
        , mRenderWindow(nullptr)
    {
        mName = Renderer::REFERENCE3D;
    }

    T3DXRenderer::~T3DXRenderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::init()
    {
        return T3D_OK;
    }

    TResult T3DXRenderer::destroy()
    {
        mRenderWindow = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr T3DXRenderer::createRenderWindow(
        const String &name,
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        TResult ret = T3D_OK;

        RenderWindowPtr window = nullptr;

        do 
        {
            window = T3DXRenderWindow::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_T3DXRENDERER, "Create render window \
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

            mRenderWindow = window;
        } while (0);

        return window;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::beginRender()
    {
        return T3D_OK;
    }

    TResult T3DXRenderer::endRender()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool T3DXRenderer::queryCapability(Capability cap) const
    {
        return true;
    }

    TResult T3DXRenderer::enableCapability(Capability cap, bool enabled)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        return T3D_OK;
    }

    const Matrix4 &T3DXRenderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 T3DXRenderer::perspective(Real fovY, Real aspect, Real zNear,
        Real zFar)
    {
        Matrix4 m;
        return m;
    }

    //--------------------------------------------------------------------------

    Matrix4 T3DXRenderer::orthographic(Real left, Real right, Real top,
        Real bottom, Real zNear, Real zFar)
    {
        Matrix4 m;
        return m;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::setCullingMode(CullingMode mode)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::setRenderMode(RenderMode mode)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::setViewport(ViewportPtr viewport)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::drawVertexArray(VertexArrayPtr vao)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::drawVertexList(PrimitiveType priType,
        HardwareVertexBufferPtr vbo, size_t startIdx, size_t priCount)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::drawIndexList(PrimitiveType priType,
        HardwareVertexBufferPtr vbo, HardwareIndexBufferPtr ibo,
        size_t startIdx, size_t priCount)
    {
        return T3D_OK;
    }
}
