

#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "T3DBound.h"
#include "Math/T3DAabb.h"


namespace Tiny3D
{
    class T3D_ENGINE_API AabbBound : public Bound
    {
    public:
        static AabbBoundPtr create(uint32_t unID, SGVisual *node);

        virtual ~AabbBound();

        virtual Type getType() const override;
        virtual BoundPtr clone() const override;

        const Aabb &getAlignAxisBox() const { return mAabb; }
        void setParam(Real minX, Real maxX, Real minY, Real maxY, Real minZ, Real maxZ);

        virtual SGRenderablePtr getRenderable() override;

    protected:
        AabbBound(uint32_t unID, SGVisual *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform);

        virtual void cloneProperties(const BoundPtr &bound) const override;

    protected:
        Aabb        mAabb;
        Aabb        mOriginalAabb;
        SGBoxPtr    mRenderable;
    };
}


#endif  /*__T3D_AABB_BOUND_H__*/
