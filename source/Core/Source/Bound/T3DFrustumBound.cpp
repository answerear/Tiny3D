

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Math/T3DMath.h"
#include "SG/Visual/T3DSGVisual.h"
#include "SG/Renderable/T3DSGBox.h"


namespace Tiny3D
{
    FrustumBoundPtr FrustumBound::create(uint32_t unID, SGVisual *node)
    {
        FrustumBoundPtr bound = new FrustumBound(unID, node);
        bound->release();
        return bound;
    }

    FrustumBound::FrustumBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
        , mRenderable(nullptr)
    {

    }

    FrustumBound::~FrustumBound()
    {
        mRenderable = nullptr;
    }

    Bound::Type FrustumBound::getType() const
    {
        return E_BT_OBB;
    }

    SGRenderablePtr FrustumBound::getRenderable()
    {
        return mRenderable;
    }

    BoundPtr FrustumBound::clone() const
    {
        FrustumBoundPtr bound = new FrustumBound(getID(), getVisualNode());
        bound->release();
        cloneProperties(bound);
        return bound;
    }

    void FrustumBound::setParam(const Plane *plane, size_t planeCount)
    {
        size_t i = 0;
        for (i = 0; i < planeCount; ++i)
        {
            mFrustum.setFace((Frustum::Face)i, plane[i]);
        }
    }

    bool FrustumBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(bound->getSphere(), mFrustum);
    }

    bool FrustumBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(bound->getAlignAxisBox(), mFrustum);
    }

    bool FrustumBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(bound->getObb(), mFrustum);
    }

    bool FrustumBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return false;//Math::intersects(mFrustum, bound->getFrustum());
    }

    void FrustumBound::updateBound(const Transform &transform)
    {

    }

    void FrustumBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const FrustumBoundPtr &newBound = (const FrustumBoundPtr &)bound;
        newBound->mFrustum = mFrustum;
    }
}
