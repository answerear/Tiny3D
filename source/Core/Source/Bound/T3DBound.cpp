

#include "Bound/T3DBound.h"
#include "SG/T3DSGVisual.h"


namespace Tiny3D
{
    Bound::Bound(uint32_t unID, SGVisual *node)
        : mID(unID)
        , mNode(node)
    {
        T3D_SAFE_ACQUIRE(mNode);
    }

    Bound::~Bound()
    {
        T3D_SAFE_RELEASE(mNode);
    }

    void Bound::setTransform(const Matrix4 &m)
    {
        Vector3 T;
        Vector3 S;
        Quaternion R;
        m.decomposition(T, S, R);
        Vector3 center = m.extractTranslation() + mSphere.getCenter();
        Real radius = m

        updateBound(m);
    }
}
