

#include "SG/Renderable/T3DSGLight.h"
#include "SG/T3DRenderQueue.h"
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

    SGNode::Type SGLight::getNodeType() const
    {
        return E_NT_LIGHT;
    }

    SGNodePtr SGLight::clone() const
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
}
