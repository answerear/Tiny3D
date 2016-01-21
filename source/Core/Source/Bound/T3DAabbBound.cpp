

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Math/T3DMath.h"
#include "SG/Visual/T3DSGVisual.h"
#include "SG/Renderable/T3DSGBox.h"


namespace Tiny3D
{
    AabbBoundPtr AabbBound::create(uint32_t unID, SGVisual *node)
    {
        AabbBoundPtr bound = new AabbBound(unID, node);
        bound->release();
        return bound;
    }

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
        aabbBound->mRenderable = (SGBox *)((SGNode *)mRenderable->clone());
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
        // 这里不用传统的变换8个顶点，然后逐个比较获取最大x,y,z来重新设置AABB
        // 这里使用快速变换的方法，减少矩阵变换带来的开销
        const Vector3 &translate = transform.getTranslate();
        const Matrix4 &M = transform.getAffineMatrix();

        Vector3 vMin = translate;
        Vector3 vMax = translate;

        // Calculate min x & max x
        if (M[0][0] > Real(0.0))
        {
            vMin.x() += M[0][0] * mOriginalAabb.getMinX();
            vMax.x() += M[0][0] * mOriginalAabb.getMaxX();
        }
        else
        {
            vMin.x() += M[0][0] * mOriginalAabb.getMaxX();
            vMax.x() += M[0][0] * mOriginalAabb.getMinX();
        }

        if (M[0][1] > Real(0.0))
        {
            vMin.x() += M[0][1] * mOriginalAabb.getMinY();
            vMax.x() += M[0][1] * mOriginalAabb.getMaxY();
        }
        else
        {
            vMin.x() += M[0][1] * mOriginalAabb.getMaxY();
            vMax.x() += M[0][1] * mOriginalAabb.getMinY();
        }

        if (M[0][2] > Real(0.0))
        {
            vMin.x() += M[0][2] * mOriginalAabb.getMinZ();
            vMax.x() += M[0][2] * mOriginalAabb.getMaxZ();
        }
        else
        {
            vMin.x() += M[0][2] * mOriginalAabb.getMaxZ();
            vMax.x() += M[0][2] * mOriginalAabb.getMinZ();
        }

        // Calculate min y & max y
        if (M[1][0] > Real(0.0))
        {
            vMin.y() += M[1][0] * mOriginalAabb.getMinX();
            vMax.y() += M[1][0] * mOriginalAabb.getMaxX();
        }
        else
        {
            vMin.y() += M[1][0] * mOriginalAabb.getMaxX();
            vMax.y() += M[1][0] * mOriginalAabb.getMinX();
        }

        if (M[1][1] > Real(0.0))
        {
            vMin.y() += M[1][1] * mOriginalAabb.getMinY();
            vMax.y() += M[1][1] * mOriginalAabb.getMaxY();
        }
        else
        {
            vMin.y() += M[1][1] * mOriginalAabb.getMaxY();
            vMax.y() += M[1][1] * mOriginalAabb.getMinY();
        }

        if (M[1][2] > Real(0.0))
        {
            vMin.y() += M[1][2] * mOriginalAabb.getMinZ();
            vMax.y() += M[1][2] * mOriginalAabb.getMaxZ();
        }
        else
        {
            vMin.y() += M[1][2] * mOriginalAabb.getMaxZ();
            vMax.y() += M[1][2] * mOriginalAabb.getMinZ();
        }

        // Calculate min z & max z
        if (M[2][0] > Real(0.0))
        {
            vMin.z() += M[2][0] * mOriginalAabb.getMinX();
            vMax.z() += M[2][0] * mOriginalAabb.getMaxX();
        }
        else
        {
            vMin.z() += M[2][0] * mOriginalAabb.getMaxX();
            vMax.z() += M[2][0] * mOriginalAabb.getMinX();
        }

        if (M[2][1] > Real(0.0))
        {
            vMin.z() += M[2][1] * mOriginalAabb.getMinY();
            vMax.z() += M[2][1] * mOriginalAabb.getMaxY();
        }
        else
        {
            vMin.z() += M[2][1] * mOriginalAabb.getMaxY();
            vMax.z() += M[2][1] * mOriginalAabb.getMinY();
        }

        if (M[2][2] > Real(0.0))
        {
            vMin.z() += M[2][2] * mOriginalAabb.getMinZ();
            vMax.z() += M[2][2] * mOriginalAabb.getMaxZ();
        }
        else
        {
            vMin.z() += M[2][2] * mOriginalAabb.getMaxZ();
            vMax.z() += M[2][2] * mOriginalAabb.getMinZ();
        }

        mAabb.setParam(vMin, vMax);
    }
}

