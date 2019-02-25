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


#include "SceneGraph/T3DSGBox.h"
#include "Render/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DVertexArrayObject.h"
#include "Bound/T3DObbBound.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    /** 长方体顶点数据格式 */
    struct BoxVertex
    {
        BoxVertex()
            : position(Vector3::ZERO)
            , diffuse(Color4::WHITE.A8R8G8B8())
        {

        }

        Vector3     position;
        uint32_t    diffuse;
    };

    //--------------------------------------------------------------------------

    SGBoxPtr SGBox::create(const Vector3 &center, const Vector3 &extent,
        ID uID /* = E_NID_AUTOMATIC */)
    {
        SGBoxPtr box = new SGBox(uID);
        box->release();
        
        if (box->init(center, extent) != T3D_OK)
        {
            box = nullptr;
        }

        return box;
    }

    //--------------------------------------------------------------------------

    SGBox::SGBox(ID uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mVAO(nullptr)
        , mBound(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SGBox::~SGBox()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    //--------------------------------------------------------------------------

    TResult SGBox::init(const Vector3 &center, const Vector3 &extent)
    {
        TResult ret = T3D_OK;

        BoxVertex vertices[23];
        setupVertices(&vertices, 23);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGBox::setupVertices(void *vertices, size_t vertexCount)
    {
        BoxVertex *vert = (BoxVertex *)vertices;

        vert[0].position = Vector3(-0.5, -0.5, 0);
        vert[0].diffuse = Color4::WHITE.A8R8G8B8();
        vert[1].position = Vector3(-0.5, -0.5, 0);
        vert[1].diffuse = Color4::WHITE.A8R8G8B8();
        vert[2].position = Vector3(-0.5, 0.5, 0);
        vert[2].diffuse = Color4::WHITE.A8R8G8B8();
        vert[3].position = Vector3(-0.5, 0.5, 0);
        vert[3].diffuse = Color4::WHITE.A8R8G8B8();
        vert[4].position = Vector3(0.5, 0.5, 0);
        vert[4].diffuse = Color4::WHITE.A8R8G8B8();
        vert[5].position = Vector3(0.5, 0.5, 1.0);
        vert[5].diffuse = Color4::WHITE.A8R8G8B8();
        vert[6].position = Vector3(-0.5, -0.5, 1.0);
        vert[6].diffuse = Color4::WHITE.A8R8G8B8();
        vert[7].position = Vector3(-0.5, 0.5, 1.0);
        vert[7].diffuse = Color4::WHITE.A8R8G8B8();
        vert[8].position = Vector3(-0.5, -0.5, 1.0);
        vert[8].diffuse = Color4::WHITE.A8R8G8B8();
        vert[9].position = Vector3(0.5, -0.5, 0);
        vert[9].diffuse = Color4::WHITE.A8R8G8B8();
        vert[10].position = Vector3(-0.5, 0.5, 0);
        vert[10].diffuse = Color4::WHITE.A8R8G8B8();
        vert[11].position = Vector3(0.5, -0.5, 0);
        vert[11].diffuse = Color4::WHITE.A8R8G8B8();
        vert[12].position = Vector3(-0.5, -0.5, 0);
        vert[12].diffuse = Color4::WHITE.A8R8G8B8();
        vert[13].position = Vector3(0.5, 0.5, 1.0);
        vert[13].diffuse = Color4::WHITE.A8R8G8B8();
        vert[14].position = Vector3(-0.5, -0.5, 1.0);
        vert[14].diffuse = Color4::WHITE.A8R8G8B8();
        vert[15].position = Vector3(-0.5, 0.5, 1.0);
        vert[15].diffuse = Color4::WHITE.A8R8G8B8();
        vert[16].position = Vector3(0.5, -0.5, 1.0);
        vert[16].diffuse = Color4::WHITE.A8R8G8B8();
        vert[17].position = Vector3(0.5, -0.5, 0);
        vert[17].diffuse = Color4::WHITE.A8R8G8B8();
        vert[18].position = Vector3(0.5, -0.5, 1.0);
        vert[18].diffuse = Color4::WHITE.A8R8G8B8();
        vert[19].position = Vector3(0.5, 0.5, 0);
        vert[19].diffuse = Color4::WHITE.A8R8G8B8();
        vert[20].position = Vector3(0.5, 0.5, 0);
        vert[20].diffuse = Color4::WHITE.A8R8G8B8();
        vert[21].position = Vector3(0.5, 0.5, 1.0);
        vert[21].diffuse = Color4::WHITE.A8R8G8B8();
        vert[22].position = Vector3(-0.5, 0.5, 1.0);
        vert[22].diffuse = Color4::WHITE.A8R8G8B8();
    }

    //--------------------------------------------------------------------------

    NodePtr SGBox::clone() const
    {
        SGBoxPtr box = new SGBox();
        box->release();

        if (box->cloneProperties(box) != T3D_OK)
        {
            box = nullptr;
        }

        return box;
    }

    //--------------------------------------------------------------------------

    TResult SGBox::cloneProperties(NodePtr node) const
    {
        TResult ret = SGRenderable::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SGBoxPtr box = smart_pointer_cast<SGBox>(node);
            ret = box->init(box->getCenter(), box->getExtent());
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGBox::updateTransform()
    {
        SGRenderable::updateTransform();
    }

    //--------------------------------------------------------------------------

    void SGBox::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {

    }

    //--------------------------------------------------------------------------

    MaterialPtr SGBox::getMaterial() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr SGBox::getVertexArrayObject() const
    {
        return nullptr;
    }
}
