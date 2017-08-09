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

#include "SceneGraph/T3DSGText2D.h"
#include "Resource/T3DFont.h"
#include "Resource/T3DFontManager.h"


namespace Tiny3D
{
    SGText2DPtr SGText2D::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGText2DPtr text = new SGText2D(uID);

        if (text != nullptr && text->init())
        {
            text->release();
        }
        else
        {
            T3D_SAFE_DELETE(text);
        }

        return text;
    }

    SGText2D::SGText2D(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
    {

    }

    SGText2D::~SGText2D()
    {

    }

    bool SGText2D::init()
    {
        return true;
    }

    Node::Type SGText2D::getNodeType() const
    {
        return E_NT_TEXT2D;
    }

    NodePtr SGText2D::clone() const
    {
        return SGText2D::create();
    }

    void SGText2D::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {

    }

    MaterialPtr SGText2D::getMaterial() const
    {
        return mMaterial;
    }

    Renderer::PrimitiveType SGText2D::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGText2D::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGText2D::getIndexData() const
    {
        return nullptr;
    }

    bool SGText2D::isIndicesUsed() const
    {
        return false;
    }
}

