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

        mCenter = center;
        mExtent = extent;

        const size_t MAX_VERTICES = 23;
        const size_t MAX_INDICES = 36;

        BoxVertex vertices[MAX_VERTICES];
        uint16_t indices[MAX_INDICES];
        setupBox(&vertices, MAX_VERTICES, indices, MAX_INDICES);

        do 
        {
            mVAO = T3D_HARDWARE_BUFFER_MGR.createVertexArrayObject(true);
            if (mVAO == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create VAO for SGBox failed !");
                break;
            }

            ret = mVAO->beginBinding();
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Binding VAO for SGBox failed !");
                break;
            }

            // 创建顶点声明
            VertexDeclarationPtr decl
                = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
            if (decl == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create vertex declaration for \
                    SGBox failed !");
                break;
            }

            decl->addAttribute(VertexAttribute(0, 0,
                VertexAttribute::E_VAT_FLOAT3,
                VertexAttribute::E_VAS_POSITION));
            decl->addAttribute(VertexAttribute(0, sizeof(Vector3),
                VertexAttribute::E_VAT_COLOR,
                VertexAttribute::E_VAS_DIFFUSE));

            // 创建VBO
            HardwareVertexBufferPtr vbo
                = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(BoxVertex),
                    MAX_VERTICES, HardwareVertexBuffer::E_HBU_STATIC, false);
            if (vbo == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create vertex buffer for \
                    SGBox failed !");
                break;
            }

            // 写顶点数据
            ret = vbo->writeData(0, sizeof(BoxVertex) * MAX_VERTICES, vertices);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Write vertices data for SGBox \
                    failed !");
                break;
            }

            // 创建IBO
            HardwareIndexBufferPtr ibo
                = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(
                    HardwareIndexBuffer::E_IT_16BITS, MAX_INDICES,
                    HardwareIndexBuffer::E_HBU_STATIC, false);
            if (ibo == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Create index buffer for SGBox \
                    failed !");
                break;
            }

            // 写索引数据
            ret = ibo->writeData(0, sizeof(uint16_t) * MAX_INDICES, indices);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_SCENE, "Write indices data for SGBox \
                    failed !");
                break;
            }

            mVAO->setVertexDeclaration(decl);
            mVAO->addVertexBuffer(vbo);
            mVAO->setIndexBuffer(ibo);
            mVAO->setPrimitiveType(Renderer::E_PT_TRIANGLE_LIST);

            mVAO->endBinding();

            // 构建碰撞体
            mBound->create(this);
            mBound->setCenter(center);
            mBound->setAxis(Vector3::UNIT_X * mExtent[0],
                Vector3::UNIT_Y * mExtent[1], Vector3::UNIT_Z * mExtent[2]);

            // 需要刷新碰撞体的世界变换
            setDirty(true);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGBox::setupBox(void *vertices, size_t vertexCount,
        uint16_t *indices, size_t indexCount)
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

        indices[0] = 11, indices[1] = 0, indices[2] = 19;
        indices[3] = 0, indices[4] = 10, indices[5] = 19;
        indices[6] = 13, indices[7] = 7, indices[8] = 8;
        indices[9] = 13, indices[10] = 8, indices[11] = 18;
        indices[12] = 16, indices[13] = 6, indices[14] = 12;
        indices[15] = 16, indices[16] = 12, indices[17] = 17;
        indices[18] = 5, indices[19] = 16, indices[20] = 9;
        indices[21] = 5, indices[22] = 9, indices[23] = 4;
        indices[24] = 22, indices[25] = 21, indices[26] = 20;
        indices[27] = 22, indices[28] = 20, indices[29] = 3;
        indices[30] = 14, indices[31] = 15, indices[32] = 2;
        indices[33] = 14, indices[34] = 2, indices[35] = 1;
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
        // 更新碰撞体
        mBound->updateBound(getLocalToWorldTransform());

        SGRenderable::updateTransform();
    }

    //--------------------------------------------------------------------------

    void SGBox::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        if (bound->test(mBound))
        {
            // 在视锥体内，放进去渲染队列，准备渲染
            queue->addRenderable(RenderQueue::E_GRPID_WIREFRAME, this);
        }
    }

    //--------------------------------------------------------------------------

    MaterialPtr SGBox::getMaterial() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr SGBox::getVertexArrayObject() const
    {
        return mVAO;
    }
}
