

#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"
#include "T3DSphere.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SphereBound : public Bound
    {
    public:
        static SphereBoundPtr create(uint32_t unID, SGVisual *node);

        virtual ~SphereBound();

        virtual Type getType() const override;
        virtual BoundPtr clone() const override;

        void setParams(const Vector3 &rkCenter, Real radius);

        const Sphere &getOriginalSphere() const { return mOriginalSphere; }

        virtual SGRenderablePtr getRenderable() override;

    protected:
        SphereBound(uint32_t unID, SGVisual *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform) override;

        virtual void cloneProperties(const BoundPtr &bound) const override;

    private:
        Sphere          mOriginalSphere;
        SGSpherePtr     mRenderable;
    };
}


#endif  /*__T3D_SPHERE_BOUND_H__*/
