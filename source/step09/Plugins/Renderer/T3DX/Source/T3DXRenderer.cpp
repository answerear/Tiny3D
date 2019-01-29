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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3DXRenderer::T3DXRenderer()
        : Renderer()
    {
        mName = Renderer::T3DXRENDERER;
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
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult T3DXRenderer::render()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr T3DXRenderer::createRenderWindow(
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        RenderWindowPtr window = nullptr;
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
