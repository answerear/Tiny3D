

#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "T3DBound.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ObbBound : public Bound
    {
    public:
        ObbBound();
        virtual ~ObbBound();
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
