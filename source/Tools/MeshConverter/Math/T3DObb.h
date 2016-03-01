

#ifndef __T3D_OBB_H__
#define __T3D_OBB_H__


#include "T3DPrerequisites.h"
#include "Math/T3DVector3.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Obb
    {
    public:
        Obb();
        Obb(const Vector3 &rkCenter, const Vector3 *akAxis, 
            const Real *afExtent);
        Obb(const Vector3 &rkCenter, const Vector3 &rkAxis0, 
            const Vector3 &rkAxis1, const Vector3 &rkAxis2,
            Real fExtent0, Real fExtent1, Real fExtent2);

        void computeVertices(Vector3 *akVertex) const;

        const Vector3 &getCenter() const;

        const Vector3 &getAxis(int32_t idx) const;

        const Vector3 *getAxis() const;

        Real getExtent(int32_t idx) const;

        const Real *getExtent() const;

        void setCenter(const Vector3 &rkCenter);

        void setAxis(const Vector3 &rkAxis0, const Vector3 &rkAxis1,
            const Vector3 &rkAxis2);

        void setExtent(int32_t idx, Real fExtent);

    private:
        Vector3 mCenter;
        Vector3 mAxis[3];
        Real    mExtent[3];
    };
}


#include "T3DObb.inl"


#endif  /*__T3D_OBB_H__*/
