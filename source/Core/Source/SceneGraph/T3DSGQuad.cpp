

#include "SceneGraph/T3DSGQuad.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderQueue.h"


namespace Tiny3D
{
    SGQuadPtr SGQuad::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGQuadPtr quad = new SGQuad(uID);
        if (quad != nullptr)
        {
            quad->release();
        }
        return quad;
    }

    SGQuad::SGQuad(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
    {

    }

    SGQuad::~SGQuad()
    {

    }

    NodePtr SGQuad::clone() const
    {
        SGQuadPtr quad = SGQuad::create();
        if (quad != nullptr)
        {
            cloneProperties(quad);
        }

        return quad;
    }

    void SGQuad::cloneProperties(const NodePtr &node) const
    {
        SGRenderable::cloneProperties(node);
    }

    void SGQuad::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (isVisible())
        {
            queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
        }
    }

    Node::Type SGQuad::getNodeType() const
    {
        return E_NT_QUAD;
    }

    MaterialPtr SGQuad::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGQuad::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGQuad::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGQuad::getIndexData() const
    {
        return nullptr;
    }

    bool SGQuad::isIndicesUsed() const
    {
        return false;
    }
}
