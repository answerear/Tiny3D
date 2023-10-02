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


#include "Render/T3DVertexDeclaration.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VertexDeclarationPtr VertexDeclaration::create()
    {
        return new VertexDeclaration();
    }

    //--------------------------------------------------------------------------

    VertexDeclaration::VertexDeclaration()
    {

    }

    //--------------------------------------------------------------------------

    VertexDeclaration::~VertexDeclaration()
    {

    }

    //--------------------------------------------------------------------------

    RenderResource::Type VertexDeclaration::getType() const 
    { 
        return Type::kVertexDeclaration;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::getAttribute(size_t index) const
    {
        T3D_ASSERT(index < mVertexAttributes.size(), "VertexDeclaration the index of attribute is out of bound !");

        VertexAttriListConstItr itr = mVertexAttributes.begin();

        size_t i = 0;
        while (itr != mVertexAttributes.end())
        {
            if (i == index)
                break;
            ++itr;
            ++i;
        }

        return *itr;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::addAttribute(size_t stream, 
        size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic, size_t semanticIndex)
    {
        VertexAttribute element(stream, offset, type, semantic, semanticIndex);

        mVertexAttributes.push_back(element);
        return mVertexAttributes.back();
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::insertAttribute(size_t pos, 
        size_t stream, size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic, size_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addAttribute(stream, offset, type, semantic, semanticIndex);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.insert(itr, VertexAttribute(stream, offset, type, semantic, semanticIndex));
        return *itr;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::addAttribute(
        const VertexAttribute &vertexAttribute)
    {
        mVertexAttributes.push_back(vertexAttribute);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::insertAttribute(size_t pos, 
        const VertexAttribute &vertexAttribute)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addAttribute(vertexAttribute);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.insert(itr, vertexAttribute);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAttribute(size_t pos)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER,
                "Remove attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexAttributes.erase(itr);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAttribute(VertexAttribute::Semantic semantic, size_t sematicIndex)
    {
        TResult ret = T3D_ERR_NOT_FOUND;
        VertexAttriListItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == sematicIndex)
            {
                mVertexAttributes.erase(itr);
                ret = T3D_OK;
                break;
            }
            ++itr;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAllAttributes()
    {
        mVertexAttributes.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::updateAttribute(size_t pos, size_t stream, 
        size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic, size_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER,
                "Update attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexAttribute(stream, offset, type, semantic, semanticIndex);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute *VertexDeclaration::findAttributeBySemantic(VertexAttribute::Semantic semantic, size_t semanticIndex) const
    {
        VertexAttriListConstItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == semanticIndex)
            {
                return &(*itr);
                break;
            }
            ++itr;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    size_t VertexDeclaration::getVertexSize(size_t source) const
    {
        size_t s = 0;
        VertexAttriListConstItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (source == itr->getStream())
            {
                s += itr->getSize();
            }

            ++itr;
        }

        return s;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onLoad() 
    {
        return true;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onUnload() 
    { 
        return true;
    }

    //--------------------------------------------------------------------------
}

