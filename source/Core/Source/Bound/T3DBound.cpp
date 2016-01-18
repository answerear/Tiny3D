

#include "Bound/T3DBound.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "SG/Visual/T3DSGVisual.h"
#include "Math/T3DTransform.h"
#include "SG/Renderable/T3DSGRenderable.h"


namespace Tiny3D
{
    Bound::Bound(uint32_t unID, SGVisual *node)
        : mNode(node)
        , mID(unID)
        , mGroupID(0)
        , mMovable(false)
        , mCollisionSource(false)
        , mEnable(true)
    {

    }

    Bound::~Bound()
    {
        mNode = nullptr;
    }

    void Bound::setTransform(const Transform &transform)
    {
        updateBound(transform);
    }

    bool Bound::test(const BoundPtr &bound) const
    {
        bool result = false;

        switch (bound->getType())
        {
        case E_BT_SPHERE:
            {
                result = testSphere((const SphereBoundPtr &)bound);
            }
            break;
        case E_BT_AABB:
            {
                result = testAabb((const AabbBoundPtr &)bound);
            }
            break;
        case E_BT_OBB:
            {
                result = testObb((const ObbBoundPtr &)bound);
            }
            break;
        case E_BT_FRUSTUM:
            {
                result = testFrustum((const FrustumBoundPtr &)bound);
            }
            break;
        default:
            break;
        }

        return result;
    }

    void Bound::cloneProperties(const BoundPtr &bound) const
    {
        bound->mSphere = mSphere;
        bound->mNode = mNode;
        bound->mID = mID;
        bound->mGroupID = mGroupID;
        bound->mMovable = mMovable;
        bound->mCollisionSource = mCollisionSource;
        bound->mEnable = mEnable;
    }
}
