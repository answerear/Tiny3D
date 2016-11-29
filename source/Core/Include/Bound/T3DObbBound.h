

#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"
#include "T3DObb.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ObbBound : public Bound
    {
    public:
        static ObbBoundPtr create(uint32_t unID, SGVisual *node);

        virtual ~ObbBound();

        virtual Type getType() const override;
        virtual SGRenderablePtr getRenderable() override;
        virtual BoundPtr clone() const override;

        const Obb &getObb() const   { return mObb; }

    protected:
        ObbBound(uint32_t unID, SGVisual *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform) override;

        virtual void cloneProperties(const BoundPtr &bound) const;

    protected:
        Obb         mObb;
        Obb         mOriginalObb;
        SGBoxPtr    mRenderable;
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
