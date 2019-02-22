/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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


#include "T3DR3DRenderer.h"
#include "T3DR3DRenderWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DRendererPtr R3DRenderer::create()
    {
        R3DRendererPtr renderer = new R3DRenderer();
        renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    R3DRenderer::R3DRenderer()
        : Renderer()
        , mRenderWindow(nullptr)
    {
        mName = Renderer::REFERENCE3D;
    }

    R3DRenderer::~R3DRenderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::init()
    {
        return T3D_OK;
    }

    TResult R3DRenderer::destroy()
    {
        mRenderWindow = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr R3DRenderer::createRenderWindow(
        const String &name,
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        TResult ret = T3D_OK;

        RenderWindowPtr window = nullptr;

        do 
        {
            window = R3DRenderWindow::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_R3DRENDERER, "Create render window \
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

    TResult R3DRenderer::beginRender()
    {
        return T3D_OK;
    }

    TResult R3DRenderer::endRender()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool R3DRenderer::queryCapability(Capability cap) const
    {
        return true;
    }

    TResult R3DRenderer::enableCapability(Capability cap, bool enabled)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        return T3D_OK;
    }

    const Matrix4 &R3DRenderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 R3DRenderer::perspective(Real fovY, Real aspect, Real zNear,
        Real zFar)
    {
        Matrix4 m;
        return m;
    }

    //--------------------------------------------------------------------------

    Matrix4 R3DRenderer::orthographic(Real left, Real right, Real top,
        Real bottom, Real zNear, Real zFar)
    {
        Matrix4 m;
        return m;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setCullingMode(CullingMode mode)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setRenderMode(RenderMode mode)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setViewport(ViewportPtr viewport)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawVertexArray(VertexArrayPtr vao)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawVertexList(PrimitiveType priType,
        HardwareVertexBufferPtr vbo, size_t startIdx, size_t priCount)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawIndexList(PrimitiveType priType,
        HardwareVertexBufferPtr vbo, HardwareIndexBufferPtr ibo,
        size_t startIdx, size_t priCount)
    {
        return T3D_OK;
    }
}
