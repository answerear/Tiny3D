

#include "SG/Renderable/T3DSGSphere.h"
#include "SG/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SGSpherePtr SGSphere::create(const String &materialName, uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGSpherePtr node = new SGSphere(unID);
        if (node != nullptr && node->init(materialName))
        {
            node->release();
        }
        else
        {
            T3D_SAFE_RELEASE(node);
        }
        return node;
    }

    SGSphere::SGSphere(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGGeometry(uID)
        , mRadius(1.0)
        , mMaterial(nullptr)
        , mVertexData(nullptr)
        , mIndexData(nullptr)
    {

    }

    SGSphere::~SGSphere()
    {

    }

    bool SGSphere::init(const String &materialName)
    {
        size_t vertexCount = MAX_VERTICES;

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        const VertexElement &vertexElem = vertexDecl->addElement(0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION);

        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexElem.getSize(), vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        void *vertices = vertexBuffer->lock(HardwareBuffer::E_HBL_DISCARD);
        loadVertices((Vector3*)vertices, vertexCount);
        vertexBuffer->unlock();

        mVertexData = VertexData::create(vertexDecl, vertexBuffer);

        size_t indexCount = MAX_TRIANGLES * 3;
        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        void *indices = indexBuffer->lock(HardwareBuffer::E_HBL_DISCARD);
        loadIndices((uint16_t*)indices, indexCount);
        indexBuffer->unlock();

        mIndexData = IndexData::create(indexBuffer);

        return true;
    }

    Node::Type SGSphere::getNodeType() const
    {
        return E_NT_SPHERE;
    }

    NodePtr SGSphere::clone() const
    {
        SGSpherePtr node = create(mMaterial->getName());
        cloneProperties(node);
        return node;
    }

    void SGSphere::cloneProperties(const NodePtr &node) const
    {
        SGRenderable::cloneProperties(node);

        const SGSpherePtr &newNode = (const SGSpherePtr &)node;
        newNode->mRadius = mRadius;
    }

    void SGSphere::setRadius(Real radius)
    {
        mRadius = radius;
        size_t vertexCount = MAX_VERTICES;
        void *vertices = mVertexData->getVertexBuffer()->lock(HardwareBuffer::E_HBL_DISCARD);
        loadVertices((Vector3*)vertices, vertexCount);
        mVertexData->getVertexBuffer()->unlock();
    }

    void SGSphere::loadVertices(Vector3 *vertices, size_t vertexCount)
    {
        Radian alphaStep(Real(2.0) * Math::PI / MAX_STACKS);
        Radian betaStep(Math::PI / MAX_SLICES);
        Radian alpha(0.0);
        Radian beta;

        int32_t i = 0, j = 0, idx = 0;

        for (i = 0; i <= MAX_STACKS; ++i)
        {
            Real sinAlpha = Math::Sin(alpha);
            Real cosAlpha = Math::Cos(alpha);
            beta = -Math::PI * Real(0.5);

            for (j = 0; j <= MAX_SLICES; ++j, ++idx)
            {
                Real sinBeta = Math::Sin(beta);
                Real cosBeta = Math::Cos(beta);

                vertices[idx].x() = mRadius * cosBeta * cosAlpha;
                vertices[idx].y() = mRadius * sinBeta;
                vertices[idx].z() = mRadius * cosBeta * sinAlpha;

                beta += betaStep;
            }

            alpha += alphaStep;
        }
    }

    void SGSphere::loadIndices(uint16_t *indices, size_t indexCount)
    {
        uint16_t base = 0;
        int32_t idx = 0;
        int32_t i = 0, j = 0;

        for (i = 0; i < MAX_STACKS; ++i)
        {
            for (j = 0; j < MAX_SLICES; ++j)
            {
                indices[idx++] = base;
                indices[idx++] = base + 1;
                indices[idx++] = base + MAX_SLICES + 1;

                indices[idx++] = base + 1;
                indices[idx++] = base + MAX_SLICES + 2;
                indices[idx++] = base + MAX_SLICES + 1;

                base++;
            }

            base++;
        }
    }

    MaterialPtr SGSphere::getMaterial() const
    {
        return mMaterial;
    }

    Renderer::PrimitiveType SGSphere::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGSphere::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGSphere::getIndexData() const
    {
        return mIndexData;
    }

    bool SGSphere::isIndicesUsed() const
    {
        return true;
    }
}
