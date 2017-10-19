/*******************************************************************************
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
 ******************************************************************************/

#ifndef __T3D_D3D9_VERTEX_DECLARATION_H__
#define __T3D_D3D9_VERTEX_DECLARATION_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
    class D3D9VertexDeclaration : public VertexDeclaration
    {
    public:
        D3D9VertexDeclaration();
        virtual ~D3D9VertexDeclaration();

        virtual const VertexElement &addElement(size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;
        virtual const VertexElement &insertElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;
        virtual bool addElement(const VertexElement &vertexElement) override;
        virtual bool insertElement(size_t pos, const VertexElement &vertexElement) override;
        virtual void removeElement(VertexElement::Semantic semantic) override;
        virtual void removeElement(size_t pos) override;
        virtual void removeAllElements() override;

        virtual void updateElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic) override;

        virtual VertexDeclarationPtr clone() const override;

        LPDIRECT3DVERTEXDECLARATION9 getD3D9VertexDeclaration();

    protected:
        LPDIRECT3DVERTEXDECLARATION9    mD3DVertexDecl;
        bool        mIsDirty;
    };
}


#endif  /*__T3D_D3D9_VERTEX_DECLARATION_H__*/
