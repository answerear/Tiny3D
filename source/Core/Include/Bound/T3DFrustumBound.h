

#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "T3DBound.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FrustumBound : public Bound
    {
    public:
        FrustumBound();
        virtual ~FrustumBound();
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
