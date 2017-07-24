

#include "Render/T3DSGAxis.h"
#include "Render/T3DHardwareBufferManager.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DRenderQueue.h"


namespace Tiny3D
{
    SGAxisPtr SGAxis::create(Real fAxisX, Real fAxisY, Real fAxisZ, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGAxisPtr axis = new SGAxis(uID);

        if (axis != nullptr && axis->init(fAxisX, fAxisY, fAxisZ))
        {
            axis->release();
        }
        else
        {
            T3D_SAFE_RELEASE(axis);
        }

        return axis;
    }

    SGAxis::SGAxis(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mIsDataDirty(false)
    {

    }

    SGAxis::~SGAxis()
    {

    }

    bool SGAxis::init(Real fAxisX, Real fAxisY, Real fAxisZ)
    {
        HardwareVertexBufferPtr vb = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(sizeof(Vertex), 6, HardwareVertexBuffer::E_HBU_DYNAMIC, false);

        VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        decl->addElement(VertexElement(0, 0, VertexElement::E_VET_FLOAT3, VertexElement::E_VES_POSITION));
        decl->addElement(VertexElement(0, 12, VertexElement::E_VET_COLOR, VertexElement::E_VES_DIFFUSE));

        mVertexData = VertexData::create(decl);
        mVertexData->addVertexBuffer(vb);

        mVertices[0].color = Color4::RED;
        mVertices[1].color = Color4::RED;

        mVertices[2].color = Color4::GREEN;
        mVertices[3].color = Color4::GREEN;

        mVertices[4].color = Color4::BLUE;
        mVertices[5].color = Color4::BLUE;

        setAxisLength(AXIS_X, fAxisX);
        setAxisLength(AXIS_Y, fAxisY);
        setAxisLength(AXIS_Z, fAxisZ);

        return updateVertices();
    }

    Real SGAxis::getAxisLength(Axis axis) const
    {
        Real length = 0.0f;

        switch (axis)
        {
        case AXIS_X:
            {
                length = fabs(mVertices[1].position.x());
            }
            break;
        case AXIS_Y:
            {
                length = fabs(mVertices[3].position.y());
            }
            break;
        case AXIS_Z:
            {
                length = fabs(mVertices[5].position.z());
            }
            break;
        default:
            break;
        }

        return length;
    }

    void SGAxis::setAxisLength(Axis axis, Real fLength)
    {
        switch (axis)
        {
        case AXIS_X:
            {
                if (fLength != mVertices[1].position.x())
                {
                    mVertices[1].position[0] = fLength;
                    mIsDataDirty = true;
                }
            }
            break;
        case AXIS_Y:
            {
                if (fLength != mVertices[3].position.y())
                {
                    mVertices[3].position[1] = fLength;
                    mIsDataDirty = true;
                }
            }
            break;
        case AXIS_Z:
            {
                if (fLength != mVertices[5].position.z())
                {
                   mVertices[5].position[2] = fLength;
                   mIsDataDirty = true;
                }
            }
            break;
        }
    }

    bool SGAxis::updateVertices()
    {
        bool ret = false;

        if (mIsDataDirty)
        {
            HardwareVertexBufferPtr vb = mVertexData->getVertexBuffer(0);
            ret = vb->writeData(0, sizeof(Vertex) * 6, &mVertices[0]);
            mIsDataDirty = false;
        }
        else
        {
            ret = true;
        }

        return ret;
    }

    Node::Type SGAxis::getNodeType() const
    {
        return E_NT_AXIS;
    }

    NodePtr SGAxis::clone() const
    {
        SGAxisPtr axis = SGAxis::create(mVertices[1].position.x(), mVertices[3].position.y(), mVertices[5].position.z());
        cloneProperties(axis);
        return axis;
    }

    void SGAxis::cloneProperties(const NodePtr &node) const
    {
        SGRenderable::cloneProperties(node);
    }

    void SGAxis::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_INDICATOR, this);
    }

    MaterialPtr SGAxis::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGAxis::getPrimitiveType() const
    {
        return Renderer::E_PT_LINE_LIST;
    }

    VertexDataPtr SGAxis::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGAxis::getIndexData() const
    {
        return nullptr;
    }

    bool SGAxis::isIndicesUsed() const
    {
        return false;
    }
}
