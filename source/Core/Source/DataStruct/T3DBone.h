

#ifndef __T3D_BONE_H__
#define __T3D_BONE_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class Bone : public Object
    {
    public:
        static BonePtr create(const String &ID);

        virtual ~Bone();


    protected:
        Bone(const String &ID);

        String      mID;
        Matrix4     mBoneTransformation;
    };
}

#endif  /*__T3D_BONE_H__*/
