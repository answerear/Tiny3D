

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Math/T3DMath.h"
#include "SG/Visual/T3DSGVisual.h"
#include "SG/Renderable/T3DSGRenderable.h"


namespace Tiny3D
{
    AabbBound::AabbBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
    {

    }

    AabbBound::~AabbBound()
    {

    }

    Bound::Type AabbBound::getType() const
    {
        return E_BT_AABB;
    }

    BoundPtr AabbBound::clone() const
    {
        AabbBoundPtr bound = new AabbBound(getID(), getVisualNode());
        cloneProperties(bound);
        return bound;
    }

    void AabbBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const AabbBoundPtr &aabbBound = (const AabbBoundPtr &)bound;

        aabbBound->mAabb = mAabb;
        aabbBound->mOriginalAabb = mOriginalAabb;


    }

    bool AabbBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(bound->getOriginalSphere(), mAabb);
    }

    bool AabbBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(mAabb, bound->getAlignAxisBox());
    }

    bool AabbBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(mAabb, bound->getObb());
    }

    bool AabbBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return Math::intersects(mAabb, bound->getFrustum());
    }

    void AabbBound::updateBound(const Transform &transform)
    {
        

        const Matrix4 &M = transform.getAffineMatrix();
        Vector3 center = M * mAabb.getCenter();
        
        Vector3 TLF(mAabb.getMinX(), mAabb.getMaxY(), mAabb.getMaxZ());

    }
}

