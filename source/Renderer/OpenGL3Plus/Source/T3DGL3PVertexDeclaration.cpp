/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#include "T3DGL3PVertexDeclaration.h"


namespace Tiny3D
{
    GL3PVertexDeclaration::GL3PVertexDeclaration()
        : VertexDeclaration()
    {

    }

    GL3PVertexDeclaration::~GL3PVertexDeclaration()
    {
    }

    const VertexElement &GL3PVertexDeclaration::addElement(size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        return VertexDeclaration::addElement(stream, offset, type, semantic);
    }

    const VertexElement &GL3PVertexDeclaration::insertElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        return VertexDeclaration::insertElement(pos, stream, offset, type, semantic);
    }

    bool GL3PVertexDeclaration::addElement(const VertexElement &vertexElement)
    {
        mIsDirty = true;
        return VertexDeclaration::addElement(vertexElement);
    }

    bool GL3PVertexDeclaration::insertElement(size_t pos, const VertexElement &vertexElement)
    {
        mIsDirty = true;
        return VertexDeclaration::insertElement(pos, vertexElement);
    }


    void GL3PVertexDeclaration::removeElement(size_t pos)
    {
        mIsDirty = true;
        VertexDeclaration::removeElement(pos);
    }

    void GL3PVertexDeclaration::removeElement(VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        VertexDeclaration::removeElement(semantic);
    }

    void GL3PVertexDeclaration::removeAllElements()
    {
        mIsDirty = true;
        VertexDeclaration::removeAllElements();
    }

    void GL3PVertexDeclaration::updateElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        VertexDeclaration::updateElement(pos, stream, offset, type, semantic);
    }

    VertexDeclarationPtr GL3PVertexDeclaration::clone() const
    {
        VertexDeclarationPtr ptr = VertexDeclaration::clone();

        return ptr;
    }
}
