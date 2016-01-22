

#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"
#include "Math/T3DFrustum.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FrustumBound : public Bound
    {
    public:
        static FrustumBoundPtr create(uint32_t unID, SGVisual *node);

        virtual ~FrustumBound();

        virtual Type getType() const override;
        virtual SGRenderablePtr getRenderable() override;
        virtual BoundPtr clone() const override;

        const Frustum &getFrustum() const   { return mFrustum; }
        void setFrustum(Frustum::Face face, const Plane &plane) { mFrustum.setFace(face, plane); }

        void setParam(const Plane *plane, size_t planeCount);

    protected:
        FrustumBound(uint32_t unID, SGVisual *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform) override;

        virtual void cloneProperties(const BoundPtr &bound) const;

        Frustum     mFrustum;
        SGBoxPtr    mRenderable;
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
