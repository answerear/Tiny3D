

#include "Bound/T3DAabbBound.h"


namespace Tiny3D
{
    AabbBound::AabbBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
    {

    }

    AabbBound::~AabbBound()
    {

    }

    Bound::Type AabbBound::getType() const
    {
        return E_BT_AABB;
    }
}

