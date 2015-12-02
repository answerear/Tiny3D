

#ifndef __T3D_FRUSTUM_H__
#define __T3D_FRUSTUM_H__


#include "T3DPrerequisites.h"
#include "Math/T3DPlane.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Frustum
    {
    public:
        enum Face
        {
            E_FACE_UP = 0,
            E_FACE_DOWN,
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
        Plane   mFace[6];
    };

    inline void Frustum::setFace(Frustum::Face face, const Plane &plane)
    {
        mFace[face] = plane;
    }

    inline const Plane &Frustum::getFace(Frustum::Face face) const
    {
        return mFace[face];
    }
}


#endif  /*__T3D_FRUSTUM_H__*/
