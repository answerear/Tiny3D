

#include "Bound/T3DSphereBound.h"


namespace Tiny3D
{
    SphereBound::SphereBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
        , mOriginalSphere()
        , mRenderable(nullptr)
    {

    }

    SphereBound::~SphereBound()
    {

    }

    Bound::Type SphereBound::getType() const
    {
        return E_BT_SPHERE;
    }

    void SphereBound::setParams(const Vector3 &rkCenter, Real radius)
    {
        mOriginalSphere.setCenter(rkCenter);
        mOriginalSphere.setRadius(radius);
    }

    SGRenderable *SphereBound::getRenderable()
    {
        return nullptr;
    }

    bool SphereBound::testSphere(const SphereBound &bound)
    {

    }
}
