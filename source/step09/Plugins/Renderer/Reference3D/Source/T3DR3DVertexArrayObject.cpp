﻿/*******************************************************************************
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


#include "T3DR3DVertexArrayObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DVertexArrayObjectPtr R3DVertexArrayObject::create(bool useIndices)
    {
        R3DVertexArrayObjectPtr vao = new R3DVertexArrayObject(useIndices);
        vao->release();
        return vao;
    }

    //--------------------------------------------------------------------------

    R3DVertexArrayObject::R3DVertexArrayObject(bool useIndices)
        : mUseIndices(useIndices)
    {

    }

    //--------------------------------------------------------------------------

    R3DVertexArrayObject::~R3DVertexArrayObject()
    {

    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::beginBinding()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::endBinding()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::setPrimitiveType(
        Renderer::PrimitiveType priType)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Renderer::PrimitiveType R3DVertexArrayObject::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::setVertexDeclaration(
        VertexDeclarationPtr decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    VertexDeclarationPtr R3DVertexArrayObject::getVertexDeclaration() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::addVertexBuffer(HardwareVertexBufferPtr vbo)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    HardwareVertexBufferPtr R3DVertexArrayObject::getVertexBuffer(size_t idx) const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult R3DVertexArrayObject::setIndexBuffer(HardwareIndexBufferPtr ibo)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    HardwareIndexBufferPtr R3DVertexArrayObject::getIndexBuffer() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool R3DVertexArrayObject::isIndicesUsed() const
    {
        return mUseIndices;
    }
}