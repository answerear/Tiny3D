

#ifndef __T3D_FRUSTUM_H__
#define __T3D_FRUSTUM_H__


#include "T3DMathPrerequisites.h"
#include "T3DPlane.h"


namespace Tiny3D
{
    class T3D_MATH_API Frustum
    {
    public:
        enum Face
        {
            E_FACE_TOP = 0,
            E_FACE_BOTTOM,
            E_FACE_LEFT,
            E_FACE_RIGHT,
            E_FACE_NEAR,
            E_FACE_FAR,
            E_MAX_FACE
        };

        Frustum();
        virtual ~Frustum();

        void setFace(Frustum::Face face, const Plane &plane);
        const Plane &getFace(Frustum::Face face) const;

    protected:
        Plane   mFace[E_MAX_FACE];
    };

    inline void Frustum::setFace(Frustum::Face face, const Plane &plane)
    {
        T3D_ASSERT(face >= 0 && face < E_MAX_FACE);
        mFace[face] = plane;
    }

    inline const Plane &Frustum::getFace(Frustum::Face face) const
    {
        T3D_ASSERT(face >= 0 && face < E_MAX_FACE);
        return mFace[face];
    }
}


#endif  /*__T3D_FRUSTUM_H__*/
