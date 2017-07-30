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

#include "T3DD3D9VertexDeclaration.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    D3D9VertexDeclaration::D3D9VertexDeclaration()
        : VertexDeclaration()
        , mD3DVertexDecl(nullptr)
    {
        
    }

    D3D9VertexDeclaration::~D3D9VertexDeclaration()
    {
        D3D_SAFE_RELEASE(mD3DVertexDecl);
    }

    const VertexElement &D3D9VertexDeclaration::addElement(size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        return VertexDeclaration::addElement(stream, offset, type, semantic);
    }

    const VertexElement &D3D9VertexDeclaration::insertElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        return VertexDeclaration::insertElement(pos, stream, offset, type, semantic);
    }

    bool D3D9VertexDeclaration::addElement(const VertexElement &vertexElement)
    {
        mIsDirty = true;
        return VertexDeclaration::addElement(vertexElement);
    }

    bool D3D9VertexDeclaration::insertElement(size_t pos, const VertexElement &vertexElement)
    {
        mIsDirty = true;
        return VertexDeclaration::insertElement(pos, vertexElement);
    }


    void D3D9VertexDeclaration::removeElement(size_t pos)
    {
        mIsDirty = true;
        VertexDeclaration::removeElement(pos);
    }

    void D3D9VertexDeclaration::removeElement(VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        VertexDeclaration::removeElement(semantic);
    }

    void D3D9VertexDeclaration::removeAllElements()
    {
        mIsDirty = true;
        VertexDeclaration::removeAllElements();
    }

    void D3D9VertexDeclaration::updateElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        mIsDirty = true;
        VertexDeclaration::updateElement(pos, stream, offset, type, semantic);
    }

    VertexDeclarationPtr D3D9VertexDeclaration::clone() const
    {
        VertexDeclarationPtr ptr = VertexDeclaration::clone();
        D3D9VertexDeclaration *decl = (D3D9VertexDeclaration *)((VertexDeclaration *)ptr);
        decl->mD3DVertexDecl = nullptr;
        decl->mIsDirty = true;

        return ptr;
    }

    LPDIRECT3DVERTEXDECLARATION9 D3D9VertexDeclaration::getD3D9VertexDeclaration()
    {
        if (mIsDirty)
        {
            D3D_SAFE_RELEASE(mD3DVertexDecl);

            D3DVERTEXELEMENT9 *d3dve = new D3DVERTEXELEMENT9[mVertexElements.size()+1];

            size_t i = 0;
            auto itr = mVertexElements.begin();
            while (itr != mVertexElements.end())
            {
                d3dve[i].Usage = D3D9Mappings::get(itr->getSemantic());
                d3dve[i].Type = D3D9Mappings::get(itr->getType());
                d3dve[i].Offset = itr->getOffset();
                d3dve[i].Stream = itr->getStream();
                d3dve[i].UsageIndex = 0;
                d3dve[i].Method = D3DDECLMETHOD_DEFAULT;
                ++itr;
                ++i;
            }

            d3dve[i].Usage = 0;
            d3dve[i].Type = D3DDECLTYPE_UNUSED;
            d3dve[i].Offset = 0;
            d3dve[i].Stream = 0xff;
            d3dve[i].UsageIndex = 0;
            d3dve[i].Method = 0;

            LPDIRECT3DDEVICE9 pD3DDevice = D3D9_RENDERER.getD3DDevice();
            HRESULT hr = pD3DDevice->CreateVertexDeclaration(d3dve, &mD3DVertexDecl);
            if (FAILED(hr))
            {

            }

            delete []d3dve;
            d3dve = nullptr;

            mIsDirty = false;
        }

        return mD3DVertexDecl;
    }
}
