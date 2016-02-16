

#include "SG/Visual/T3DSGLight.h"
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
        : SGVisual(uID)
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
        SGVisual::updateTransform();
    }

    void SGLight::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (mIsVisible)
        {
            
        }
    }
}
