

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "T3DTransform.h"
#include "T3DMath.h"
#include "SG/Renderable/T3DSGRenderable.h"
#include "SG/Node/T3DSGNode.h"
#include "SG/Renderable/T3DSGSphere.h"


namespace Tiny3D
{
    SphereBoundPtr SphereBound::create(uint32_t unID, SGNode *node)
    {
        SphereBoundPtr bound = new SphereBound(unID, node);
        bound->release();
        return bound;
    }

    SphereBound::SphereBound(uint32_t unID, SGNode *node)
        : Bound(unID, node)
        , mOriginalSphere()
        , mRenderable(nullptr)
    {

    }

    SphereBound::~SphereBound()
    {
        mRenderable = nullptr;
    }

    Bound::Type SphereBound::getType() const
    {
        return E_BT_SPHERE;
    }

    SGRenderablePtr SphereBound::getRenderable()
    {
        return (SGRenderablePtr)mRenderable;
    }

    BoundPtr SphereBound::clone() const
    {
        SphereBoundPtr bound = new SphereBound(getID(), getNode());
        cloneProperties(bound);
        return bound;
    }

    void SphereBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const SphereBoundPtr &sphereBound = (const SphereBoundPtr &)bound;
        sphereBound->mOriginalSphere = mOriginalSphere;
        sphereBound->mRenderable = smart_pointer_cast<SGSphere>(mRenderable->clone());
    }

    void SphereBound::setParams(const Vector3 &rkCenter, Real radius)
    {
        mOriginalSphere.setCenter(rkCenter);
        mOriginalSphere.setRadius(radius);

        mSphere.setCenter(rkCenter);
        mSphere.setRadius(radius);
    }

    bool SphereBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(mSphere, bound->getOriginalSphere());
    }

    bool SphereBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(mSphere, bound->getAlignAxisBox());
    }

    bool SphereBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(mSphere, bound->getObb());
    }

    bool SphereBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return Math::intersects(mSphere, bound->getFrustum());
    }

    void SphereBound::updateBound(const Transform &transform)
    {
        // 更新变换后用于碰撞检测的球体
        const Matrix4 &M = transform.getAffineMatrix();
        Vector3 center = M * mOriginalSphere.getCenter();

        const Vector3 &S = transform.getScale();
        Real factor = std::max(std::max(S.x(), S.y()), S.z());
        Real radius = factor * mOriginalSphere.getRadius();

        mSphere.setCenter(center);
        mSphere.setRadius(radius);

        // 原始球体，只更新球心位置，不更新半径大小
        center = M * mOriginalSphere.getCenter();
        mOriginalSphere.setCenter(center);
    }
}
