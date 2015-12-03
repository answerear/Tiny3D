

#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"
#include "Math/T3DAabb.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ObbBound : public Bound
    {
    public:
        ObbBound(uint32_t unID, SGVisual *node);
        virtual ~ObbBound();

        const Obb &getObb() const;
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
