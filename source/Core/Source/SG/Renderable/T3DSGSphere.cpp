

#include "SG/Renderable/T3DSGSphere.h"
#include "SG/T3DRenderQueue.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SGSpherePtr SGSphere::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGSpherePtr node = new SGSphere(unID);
        node->release();
        return node;
    }

    SGSphere::SGSphere(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mRadius(1.0)
    {

    }

    SGSphere::~SGSphere()
    {

    }

    bool SGSphere::loadSphere()
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

        mPrimitiveType = Renderer::E_PT_TRIANGLE_LIST;
        mUseIndices = true;

        return true;
    }

    SGNode::Type SGSphere::getNodeType() const
    {
        return E_NT_SPHERE;
    }

    SGNodePtr SGSphere::clone() const
    {
        SGSpherePtr node = new SGSphere();
        node->release();
        cloneProperties(node);
        return node;
    }

    void SGSphere::cloneProperties(const SGNodePtr &node) const
    {
        SGRenderable::cloneProperties(node);
    }

    void SGSphere::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }

    void SGSphere::setRadius(Real radius)
    {
        mRadius = radius;
        size_t vertexCount = MAX_VERTICES;
        void *vertices = mVertexData->getVertexBuffer()->lock(HardwareBuffer::E_HBL_DISCARD);
        loadVertices((Vector3*)vertices, vertexCount);
        mVertexData->getVertexBuffer()->unlock();
    }

    Vector3 SGSphere::getPoint(Real xyStep, Real zStep) const
    {
        Radian alpha(Math::PI * zStep);
        Radian beta(Math::PI * Real(2.0) * xyStep);
        Real x = mRadius * Math::Sin(alpha) * Math::Cos(beta);
        Real y = mRadius * Math::Sin(alpha) * Math::Sin(beta);
        Real z = mRadius * Math::Cos(alpha);
        return Vector3(x, y, z);
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
}
