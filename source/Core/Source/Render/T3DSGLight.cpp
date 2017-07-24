

#include "Render/T3DSGLight.h"
#include "Render/T3DRenderQueue.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    SGLightPtr SGLight::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGLightPtr light = new SGLight(uID);
        light->release();
        return light;
    }

    SGLight::SGLight(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mLightType(E_LT_POINT)
        , mAmbientColor(Color4::WHITE)
        , mDiffuseColor(Color4::WHITE)
        , mSpecularColor(Color4::WHITE)
    {

    }

    SGLight::~SGLight()
    {

    }

    Node::Type SGLight::getNodeType() const
    {
        return E_NT_LIGHT;
    }

    NodePtr SGLight::clone() const
    {
        SGLightPtr light = create();
        cloneProperties(light);
        return light;
    }

    void SGLight::updateTransform()
    {
    }

    void SGLight::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_LIGHT, this);
    }

    MaterialPtr SGLight::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGLight::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGLight::getVertexData() const
    {
        return nullptr;
    }

    IndexDataPtr SGLight::getIndexData() const
    {
        return nullptr;
    }

    bool SGLight::isIndicesUsed() const
    {
        return false;
    }
}
