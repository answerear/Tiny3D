

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
        const Vector3 vertices[] = 
        {
            Vector3(-1.0, 1.0, 1.0),    // 0
            Vector3(-1.0, -1.0, 1.0),   // 1
            Vector3(1.0, 1.0, 1.0),     // 2
            Vector3(1.0, -1.0, 1.0),    // 3
            Vector3(1.0, 1.0, -1.0),    // 4
            Vector3(1.0, -1.0, -1.0),   // 5
            Vector3(-1.0, 1.0, -1.0),   // 6
            Vector3(-1.0, -1.0, -1.0)   // 7
        };

        size_t vertexCount = sizeof(vertices) / sizeof(Vector3);

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        const VertexElement &vertexElem = vertexDecl->addElement(0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);

        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexElem.getSize(), vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        vertexBuffer->writeData(0, vertexElem.getSize() * vertexCount, vertices);

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
