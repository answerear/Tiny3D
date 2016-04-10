

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Math/T3DMath.h"
#include "SG/Visual/T3DSGVisual.h"
#include "SG/Renderable/T3DSGBox.h"

namespace Tiny3D
{
    ObbBoundPtr ObbBound::create(uint32_t unID, SGVisual *node)
    {
        ObbBoundPtr bound = new ObbBound(unID, node);
        bound->release();
        return bound;
    }

    ObbBound::ObbBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
    {

    }

    ObbBound::~ObbBound()
    {

    }

    Bound::Type ObbBound::getType() const
    {
        return E_BT_OBB;
    }

    SGRenderablePtr ObbBound::getRenderable()
    {
        return mRenderable;
    }

    BoundPtr ObbBound::clone() const
    {
        ObbBoundPtr bound = new ObbBound(getID(), getVisualNode());
        bound->release();
        cloneProperties(bound);
        return bound;
    }

    bool ObbBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(bound->getSphere(), mObb);
    }

    bool ObbBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(bound->getAlignAxisBox(), mObb);
    }

    bool ObbBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(mObb, bound->getObb());
    }

    bool ObbBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return Math::intersects(mObb, bound->getFrustum());
    }

    void ObbBound::updateBound(const Transform &transform)
    {

    }

    void ObbBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const ObbBoundPtr &newBound = (const ObbBoundPtr &)bound;
        newBound->mObb = mObb;
        newBound->mOriginalObb = mOriginalObb;
        newBound->mRenderable = smart_pointer_cast<SGBox>(mRenderable->clone());

    }
}
