

#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"
#include "Bound/T3DSphere.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SphereBound : public Bound
    {
    public:
        SphereBound(uint32_t unID, SGVisual *node);
        virtual ~SphereBound();

        Type getType() const override;

        void setParams(const Vector3 &rkCenter, Real radius);

        virtual SGRenderable *getRenderable() override;

    protected:
        virtual bool testSphere(const SphereBound &bound) const override;
        virtual bool testAabb(const AabbBound &bound) const override;
        virtual bool testObb(const ObbBound &bound) const override;
        virtual bool testFrustum(const FrustumBound &bound) const override;

        virtual void updateBound(const Matrix4 &m) override;

    private:
        Sphere      mOriginalSphere;
        SGSphere    *mRenderable;
    };
}


#endif  /*__T3D_SPHERE_BOUND_H__*/
