

#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"
#include "Math/T3DFrustum.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FrustumBound : public Bound
    {
    public:
        FrustumBound(uint32_t unID, SGVisual *node);
        virtual ~FrustumBound();

        const Frustum &getFrustum() const;

    private:
        Frustum     mFrustum;
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
