

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
        updateVertices((Vector3*)vertices, vertexCount);
        vertexBuffer->unlock();

        mVertexData = VertexData::create(vertexDecl, vertexBuffer);

        mPrimitiveType = Renderer::E_PT_TRIANGLE_LIST;
        mUseIndices = false;

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
        updateVertices((Vector3*)vertices, vertexCount);
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

    void SGSphere::updateVertices(Vector3 *vertices, size_t vertexCount)
    {
        size_t i = 0;
        size_t j = 0;
        size_t idx = 0;

        Real zStep = Real(1.0) / Real(SPLITE_PIECES_Z);
        Real xyStep = Real(1.0) / Real(SPLITE_PIECES_XY);
        Real z(0.0);
        Real xy(0.0);

        // 最下端顶点
        for (i = 0; i < SPLITE_PIECES_XY; ++i)
        {
            vertices[idx++] = getPoint(0, 0);
            vertices[idx++] = getPoint(xy, zStep);
            vertices[idx++] = getPoint(xy+xyStep, zStep);
            xy += xyStep;
        }

        // 中间顶点
        xy = 0.0;
        z = zStep;
        for (i = 1; i < SPLITE_PIECES_Z - 1; ++i)
        {
            for (j = 0; j < SPLITE_PIECES_XY; ++j)
            {
                vertices[idx++] = getPoint(xy, zStep);
                vertices[idx++] = getPoint(xy+xyStep, z);
                vertices[idx++] = getPoint(xy+xyStep, z+zStep);

                vertices[idx++] = getPoint(xy, zStep);
                vertices[idx++] = getPoint(xy+xyStep, z+zStep);
                vertices[idx++] = getPoint(xy, z+zStep);

                xy += xyStep;
            }
        }

        // 最上端顶点
        xy = 0.0;
        for (j = 0; j < SPLITE_PIECES_XY; ++j)
        {
            vertices[idx++] = getPoint(0.0, 1.0);
            vertices[idx++] = getPoint(xy, Real(1.0)-zStep);
            vertices[idx++] = getPoint(xy+xyStep, Real(1.0)-zStep);
        }
    }
}
