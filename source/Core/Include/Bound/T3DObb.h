

#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "T3DBound.h"


namespace Tiny3D
{
    class T3D_ENGINE_API AabbBound : public Bound
    {
    public:
        AabbBound();
        virtual ~AabbBound();
    };
}


#endif  /*__T3D_AABB_BOUND_H__*/
