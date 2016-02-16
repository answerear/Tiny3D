

#include "SG/Renderable/T3DSGBox.h"
#include "SG/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Resource/T3DMaterialManager.h"


namespace Tiny3D
{
    SGBoxPtr SGBox::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGBoxPtr box = new SGBox(unID);
        box->release();
        return box;
    }

    SGBox::SGBox(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGRenderable(unID)
    {
        // 因为一个顶点要对应一个纹理UV，所以这里无法用8个顶点表示一个立方体，
        // 而需要每个立方体面用4个顶点表示
        // 正方体顶点定义如下：
        //
        //          v6-------v4
        //          /|       /|
        //         / |      / |
        //        v0-------v2 |
        //        | v7-----|-v5
        //        | /      | /
        //        |/       |/
        //        v1-------v3
        //
        // 分界成6个面为：
        //
        //         v0-------v2          v6-------v0          v6-------v4
        //         |         |          |         |          |         |
        // Front : |         |   Left : |         |    Top : |         |
        //         |         |          |         |          |         |
        //         v1-------v3          v7-------v1          v0-------v2
        //
        //         v4-------v6          v2-------v4          v1-------v3
        //         |         |          |         |          |         |
        //  Back : |         |  Right : |         | Bottom : |         |
        //         |         |          |         |          |         |
        //         v5-------v7          v3-------v5          v7-------v5


        // 创建顶点缓存
        // Front
        mVertices[0].position = Vector3(-1.0, 1.0, 1.0);
        mVertices[0].texcoord = Vector2(0.0, 0.0);
        mVertices[1].position = Vector3(-1.0, -1.0, 1.0);
        mVertices[1].texcoord = Vector2(0.0, 1.0);
        mVertices[2].position = Vector3(1.0, 1.0, 1.0);
        mVertices[2].texcoord = Vector2(1.0, 0.0);
        mVertices[3].position = Vector3(1.0, -1.0, 1.0);
        mVertices[3].texcoord = Vector2(1.0, 1.0);

        // Right
        mVertices[4].position = Vector3(1.0, 1.0, 1.0);
        mVertices[4].texcoord = Vector2(0.0, 0.0);
        mVertices[5].position = Vector3(1.0, -1.0, 1.0);
        mVertices[5].texcoord = Vector2(0.0, 1.0);
        mVertices[6].position = Vector3(1.0, 1.0, -1.0);
        mVertices[6].texcoord = Vector2(1.0, 0.0);
        mVertices[7].position = Vector3(1.0, -1.0, -1.0);
        mVertices[7].texcoord = Vector2(1.0, 1.0);

        // Back
        mVertices[8].position = Vector3(1.0, 1.0, -1.0);
        mVertices[8].texcoord = Vector2(0.0, 0.0);
        mVertices[9].position = Vector3(1.0, -1.0, -1.0);
        mVertices[9].texcoord = Vector2(0.0, 1.0);
        mVertices[10].position = Vector3(-1.0, 1.0, -1.0);
        mVertices[10].texcoord = Vector2(1.0, 0.0);
        mVertices[11].position = Vector3(-1.0, -1.0, -1.0);
        mVertices[11].texcoord = Vector2(1.0, 1.0);

        // Left
        mVertices[12].position = Vector3(-1.0, 1.0, -1.0);
        mVertices[12].texcoord = Vector2(0.0, 0.0);
        mVertices[13].position = Vector3(-1.0, -1.0, -1.0);
        mVertices[13].texcoord = Vector2(0.0, 1.0);
        mVertices[14].position = Vector3(-1.0, 1.0, 1.0);
        mVertices[14].texcoord = Vector2(1.0, 0.0);
        mVertices[15].position = Vector3(-1.0, -1.0, 1.0);
        mVertices[15].texcoord = Vector2(1.0, 1.0);

        // Top
        mVertices[16].position = Vector3(-1.0, 1.0, -1.0);
        mVertices[16].texcoord = Vector2(0.0, 0.0);
        mVertices[17].position = Vector3(-1.0, 1.0, 1.0);
        mVertices[17].texcoord = Vector2(0.0, 1.0);
        mVertices[18].position = Vector3(1.0, 1.0, -1.0);
        mVertices[18].texcoord = Vector2(1.0, 0.0);
        mVertices[19].position = Vector3(1.0, 1.0, 1.0);
        mVertices[19].texcoord = Vector2(1.0, 1.0);

        // Bottom
        mVertices[20].position = Vector3(-1.0, -1.0, 1.0);
        mVertices[20].texcoord = Vector2(0.0, 0.0);
        mVertices[21].position = Vector3(-1.0, -1.0, -1.0);
        mVertices[21].texcoord = Vector2(0.0, 1.0);
        mVertices[22].position = Vector3(1.0, -1.0, 1.0);
        mVertices[22].texcoord = Vector2(1.0, 0.0);
        mVertices[23].position = Vector3(1.0, -1.0, -1.0);
        mVertices[23].texcoord = Vector2(1.0, 1.0);
    }

    SGBox::~SGBox()
    {
        mVertexData = nullptr;
        mIndexData = nullptr;
    }

    bool SGBox::loadBox()
    {
        size_t vertexCount = sizeof(mVertices) / sizeof(Vertex);

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        size_t offset = 0;
        const VertexElement &posElem = vertexDecl->addElement(offset, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);
        offset += posElem.getSize();
        const VertexElement &texElem = vertexDecl->addElement(offset, VertexElement::E_VET_FLOAT2, VertexElement::E_VES_TEXCOORD);
        offset += texElem.getSize();

        size_t vertexSize = offset;
        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexSize, vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        vertexBuffer->writeData(0, vertexSize * vertexCount, mVertices);

        mVertexData = VertexData::create(vertexDecl, vertexBuffer);

        // 创建索引缓存
        const uint16_t indices[] =
        {
            // front face
            0, 1, 2,
            1, 3, 2,
            // right face
            4, 5, 6,
            5, 7, 6,
            // back face
            8, 9, 10,
            9, 11, 10,
            // left face
            12, 13, 14,
            13, 15, 14,
            // top face
            16, 17, 18,
            17, 19, 18,
            // bottom face
            20, 21, 22,
            21, 23, 22
        };

        size_t indexCount = sizeof(indices) / sizeof(uint16_t);

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        indexBuffer->writeData(0, indexCount * sizeof(uint16_t), indices);

        mIndexData = IndexData::create(indexBuffer);

        mPrimitiveType = Renderer::E_PT_TRIANGLE_LIST;

        mMaterial = nullptr;
        mMaterial = T3D_MATERIAL_MGR.loadMaterial("Material_Box", Material::E_MT_MANUAL);
        mMaterial->setAmbientColor(Color4::WHITE);
        mMaterial->setDiffuseColor(Color4::WHITE);
        mMaterial->setSpecularColor(Color4::WHITE);
        mMaterial->setEmissiveColor(Color4::WHITE);
        mMaterial->setTexture(0, "blocks.png");

        return true;
    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    void SGBox::setMinX(Real x)
    {
        // v0, v1, v10, v11, v12, v13, v14, v15, v16, v17, v20, v21
        mVertices[0].position.x() = x;
        mVertices[1].position.x() = x;
        mVertices[10].position.x() = x;
        mVertices[11].position.x() = x;
        mVertices[12].position.x() = x;
        mVertices[13].position.x() = x;
        mVertices[14].position.x() = x;
        mVertices[15].position.x() = x;
        mVertices[16].position.x() = x;
        mVertices[17].position.x() = x;
        mVertices[20].position.x() = x;
        mVertices[21].position.x() = x;

        updateVertices();
    }

    void SGBox::setMaxX(Real x)
    {
        // v2, v3, v4, v5, v6, v7, v8, v9, v18, v19, v22, v23
        mVertices[2].position.x() = x;
        mVertices[3].position.x() = x;
        mVertices[4].position.x() = x;
        mVertices[5].position.x() = x;
        mVertices[6].position.x() = x;
        mVertices[7].position.x() = x;
        mVertices[8].position.x() = x;
        mVertices[9].position.x() = x;
        mVertices[18].position.x() = x;
        mVertices[19].position.x() = x;
        mVertices[22].position.x() = x;
        mVertices[23].position.x() = x;

        updateVertices();
    }

    void SGBox::setMinY(Real y)
    {
        // v1, v3, v5, v7, v9, v11, v13, v15, v20, v21, v22, v23
        mVertices[1].position.y() = y;
        mVertices[3].position.y() = y;
        mVertices[5].position.y() = y;
        mVertices[7].position.y() = y;
        mVertices[9].position.y() = y;
        mVertices[11].position.y() = y;
        mVertices[13].position.y() = y;
        mVertices[15].position.y() = y;
        mVertices[20].position.y() = y;
        mVertices[21].position.y() = y;
        mVertices[22].position.y() = y;
        mVertices[23].position.y() = y;

        updateVertices();
    }

    void SGBox::setMaxY(Real y)
    {
        // v0, v2, v4, v6, v8, v10, v12, v14, v16, v17, v18, v19
        mVertices[0].position.y() = y;
        mVertices[2].position.y() = y;
        mVertices[4].position.y() = y;
        mVertices[6].position.y() = y;
        mVertices[8].position.y() = y;
        mVertices[10].position.y() = y;
        mVertices[12].position.y() = y;
        mVertices[14].position.y() = y;
        mVertices[16].position.y() = y;
        mVertices[17].position.y() = y;
        mVertices[18].position.y() = y;
        mVertices[19].position.y() = y;

        updateVertices();
    }

    void SGBox::setMinZ(Real z)
    {
        // v6, v7, v8, v9, v10, v11, v12, v13, v16, v18, v21, v23
        mVertices[6].position.z() = z;
        mVertices[7].position.z() = z;
        mVertices[8].position.z() = z;
        mVertices[9].position.z() = z;
        mVertices[10].position.z() = z;
        mVertices[11].position.z() = z;
        mVertices[12].position.z() = z;
        mVertices[13].position.z() = z;
        mVertices[16].position.z() = z;
        mVertices[18].position.z() = z;
        mVertices[21].position.z() = z;
        mVertices[23].position.z() = z;

        updateVertices();
    }

    void SGBox::setMaxZ(Real z)
    {
        // v0, v1, v2, v3, v4, v5, v14, v15, v17, v19, v20, v22
        mVertices[0].position.z() = z;
        mVertices[1].position.z() = z;
        mVertices[2].position.z() = z;
        mVertices[3].position.z() = z;
        mVertices[4].position.z() = z;
        mVertices[5].position.z() = z;
        mVertices[14].position.z() = z;
        mVertices[15].position.z() = z;
        mVertices[17].position.z() = z;
        mVertices[19].position.z() = z;
        mVertices[20].position.z() = z;
        mVertices[22].position.z() = z;

        updateVertices();
    }

    Real SGBox::getMinX() const
    {
        // v0, v1, v10, v11, v12, v13, v14, v15, v16, v17, v20, v21
        return mVertices[0].position.x();
    }

    Real SGBox::getMaxX() const
    {
        // v2, v3, v4, v5, v6, v7, v8, v9, v18, v19, v22, v23
        return mVertices[2].position.x();
    }

    Real SGBox::getMinY() const
    {
        // v1, v3, v5, v7, v9, v11, v13, v15, v20, v21, v22, v23
        return mVertices[1].position.y();
    }

    Real SGBox::getMaxY() const
    {
        // v0, v2, v4, v6, v8, v10, v12, v14, v16, v17, v18, v19
        return mVertices[0].position.y();
    }

    Real SGBox::getMinZ() const
    {
        // v6, v7, v8, v9, v10, v11, v12, v13, v16, v18, v21, v23
        return mVertices[6].position.z();
    }

    Real SGBox::getMaxZ() const
    {
        // v0, v1, v2, v3, v4, v5, v14, v15, v17, v19, v20, v22
        return mVertices[0].position.z();
    }

    void SGBox::updateVertices()
    {
        HardwareVertexBufferPtr vertexBuffer = mVertexData->getVertexBuffer();
        VertexDeclarationPtr vertexDecl = mVertexData->getDeclaration();
        size_t vertexSize = 0;
        const VertexElement *posElem = vertexDecl->findElementBySemantic(VertexElement::E_VES_POSITION);
        vertexSize += posElem->getSize();
        const VertexElement *texElem = vertexDecl->findElementBySemantic(VertexElement::E_VES_TEXCOORD);
        vertexSize += texElem->getSize();

        size_t vertexCount = sizeof(mVertices) / sizeof(Vertex);
        vertexBuffer->writeData(0, vertexSize * vertexCount, mVertices, true);
    }

    SGNodePtr SGBox::clone() const
    {
        SGBoxPtr node = new SGBox();
        node->release();
        cloneProperties(node);
        return node;
    }

    void SGBox::cloneProperties(const SGNodePtr &node) const
    {
        SGRenderable::cloneProperties(node);

        const SGBoxPtr &newNode = (const SGBoxPtr &)node;
        size_t sizeInBytes = sizeof(mVertices);
        memcpy(newNode->mVertices, mVertices, sizeInBytes);
    }

    void SGBox::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }
}
