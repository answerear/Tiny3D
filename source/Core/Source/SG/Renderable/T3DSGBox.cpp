

#include "SG/Renderable/T3DSGBox.h"
#include "SG/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    const uint32_t SGBox::MAX_FACE_NUM = 6;
    const uint32_t SGBox::MAX_VERTICES_NUM = 8;
    const uint32_t SGBox::INDICES_PER_FACE = 6;

    SGBoxPtr SGBox::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGBox *box = new SGBox(unID);
        SGBoxPtr ptr(box);
        box->release();
        return ptr;
    }

    SGBox::SGBox(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGRenderable(unID)
    {
        // 创建顶点缓存
        const size_t vertexCount = MAX_VERTICES_NUM;
        const Vector3 vertices[vertexCount] = 
        {
            Vector3(1.0, 1.0, 1.0),
            Vector3(1.0, 1.0, -1.0),
            Vector3(1.0, -1.0, 1.0),
            Vector3(1.0, -1.0, -1.0),
            Vector3(-1.0, 1.0, 1.0),
            Vector3(-1.0, 1.0, -1.0),
            Vector3(-1.0, -1.0, 1.0),
            Vector3(-1.0, -1.0, -1.0)
        };

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        const VertexElement &vertexElem = vertexDecl->addElement(0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);

        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexElem.getSize(), vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        vertexBuffer->writeData(0, vertexElem.getSize() * vertexCount, vertices);

        mVertexData = VertexData::create(vertexDecl, vertexBuffer);

        // 创建索引缓存
        const size_t indexCount = MAX_FACE_NUM * INDICES_PER_FACE;
        const uint16_t indices[indexCount] =
        {
            0, 4, 6, 6, 0, 2,
            0, 2, 1, 1, 3, 2,
            1, 3, 5, 5, 7, 3,
            5, 7, 4, 4, 6, 7,
            0, 1, 4, 4, 5, 1,
            2, 6, 7, 7, 2, 3
        };

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        indexBuffer->writeData(0, indexCount * sizeof(uint16_t), indices);

        mIndexData = IndexData::create(indexBuffer);
    }

    SGBox::~SGBox()
    {
        mVertexData = nullptr;
        mIndexData = nullptr;
    }

    SGNode::Type SGBox::getNodeType() const
    {
        return E_NT_BOX;
    }

    void SGBox::setMinX(Real x)
    {
        
    }

    SGNodePtr SGBox::clone() const
    {
        SGBox *node = new SGBox();
        SGNodePtr ptr(node);
        cloneProperties(node);
        node->release();
        return ptr;
    }

    void SGBox::cloneProperties(SGNode *node) const
    {
        SGRenderable::cloneProperties(node);
    }

    void SGBox::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }
}
