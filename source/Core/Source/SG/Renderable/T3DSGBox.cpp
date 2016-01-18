

#include "SG/Renderable/T3DSGBox.h"
#include "SG/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"


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
        // 正方体顶点定义如下：
        //          v6-------v4
        //          /|       /|
        //         / |      / |
        //        v0-------v2 |
        //        | v7-----|-v5
        //        | /      | /
        //        |/       |/
        //        v1-------v3
        //

        // 创建顶点缓存
        mVertices[0] = Vector3(-1.0, 1.0, 1.0);
        mVertices[1] = Vector3(-1.0, -1.0, 1.0);
        mVertices[2] = Vector3(1.0, 1.0, 1.0);
        mVertices[3] = Vector3(1.0, -1.0, 1.0);
        mVertices[4] = Vector3(1.0, 1.0, -1.0);
        mVertices[5] = Vector3(1.0, -1.0, -1.0);
        mVertices[6] = Vector3(-1.0, 1.0, -1.0);
        mVertices[7] = Vector3(-1.0, -1.0, -1.0);
    }

    SGBox::~SGBox()
    {
        mVertexData = nullptr;
        mIndexData = nullptr;
    }

    bool SGBox::loadBox()
    {
        size_t vertexCount = sizeof(mVertices) / sizeof(Vector3);

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        const VertexElement &vertexElem = vertexDecl->addElement(0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);

        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexElem.getSize(), vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        vertexBuffer->writeData(0, vertexElem.getSize() * vertexCount, mVertices);

        mVertexData = VertexData::create(vertexDecl, vertexBuffer);

        // 创建索引缓存
        const uint16_t indices[] =
        {
            // front face
            0, 1, 2,
            1, 3, 2,
            // right face
            2, 3, 4,
            3, 5, 4,
            // back face
            4, 5, 6,
            5, 7, 6,
            // left face
            6, 7, 0,
            7, 1, 0,
            // top face
            4, 6, 2,
            6, 0, 2,
            // bottom face
            7, 5, 1,
            5, 3, 1
        };

        size_t indexCount = sizeof(indices) / sizeof(uint16_t);

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        indexBuffer->writeData(0, indexCount * sizeof(uint16_t), indices);

        mIndexData = IndexData::create(indexBuffer);

        mPrimitiveType = Renderer::E_PT_TRIANGLE_LIST;

        return true;
    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    void SGBox::setMinX(Real x)
    {
        // v0, v1, v6, v7
        mVertices[0].x() = x;
        mVertices[1].x() = x;
        mVertices[6].x() = x;
        mVertices[7].x() = x;

        updateVertices();
    }

    void SGBox::setMaxX(Real x)
    {
        // v2, v3, v4, v5
        mVertices[2].x() = x;
        mVertices[3].x() = x;
        mVertices[4].x() = x;
        mVertices[5].x() = x;

        updateVertices();
    }

    void SGBox::setMinY(Real y)
    {
        // v1, v3, v5, v7
        mVertices[1].y() = y;
        mVertices[3].y() = y;
        mVertices[5].y() = y;
        mVertices[7].y() = y;

        updateVertices();
    }

    void SGBox::setMaxY(Real y)
    {
        // v0, v2, v4, v6
        mVertices[0].y() = y;
        mVertices[2].y() = y;
        mVertices[4].y() = y;
        mVertices[6].y() = y;

        updateVertices();
    }

    void SGBox::setMinZ(Real z)
    {
        // v4, v5, v6, v7
        mVertices[4].z() = z;
        mVertices[5].z() = z;
        mVertices[6].z() = z;
        mVertices[7].z() = z;

        updateVertices();
    }

    void SGBox::setMaxZ(Real z)
    {
        // v0, v1, v2, v3
        mVertices[0].z() = z;
        mVertices[1].z() = z;
        mVertices[2].z() = z;
        mVertices[3].z() = z;

        updateVertices();
    }

    Real SGBox::getMinX() const
    {
        // v0, v1, v6, v7
        return mVertices[0].x();
    }

    Real SGBox::getMaxX() const
    {
        // v2, v3, v4, v5
        return mVertices[2].x();
    }

    Real SGBox::getMinY() const
    {
        // v1, v3, v5, v7
        return mVertices[1].y();
    }

    Real SGBox::getMaxY() const
    {
        // v0, v2, v4, v6
        return mVertices[0].y();
    }

    Real SGBox::getMinZ() const
    {
        // v4, v5, v6, v7
        return mVertices[4].z();
    }

    Real SGBox::getMaxZ() const
    {
        // v0, v1, v2, v3
        return mVertices[0].z();
    }

    void SGBox::updateVertices()
    {
        HardwareVertexBufferPtr vertexBuffer = mVertexData->getVertexBuffer();
        VertexDeclarationPtr vertexDecl = mVertexData->getDeclaration();
        const VertexElement *vertexElem = vertexDecl->findElementBySemantic(VertexElement::E_VES_POSITION);

        size_t vertexCount = sizeof(mVertices) / sizeof(Vector3);
        vertexBuffer->writeData(0, vertexElem->getSize() * vertexCount, mVertices, true);
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
