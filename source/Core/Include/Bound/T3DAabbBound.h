

#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "T3DBound.h"
#include "Math/T3DAabb.h"


namespace Tiny3D
{
    class T3D_ENGINE_API AabbBound : public Bound
    {
    public:
        AabbBound(uint32_t unID, SGVisual *node);
        virtual ~AabbBound();

        virtual Type getType() const override;

        const Aabb &getAlignAxisBox() const { return mAabb; }

    protected:
        Aabb    mAabb;
    };
}


#endif  /*__T3D_AABB_BOUND_H__*/
