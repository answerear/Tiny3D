

#ifndef __T3D_BONE_DATA_H__
#define __T3D_BONE_DATA_H__


#include "Misc/T3DObject.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"


namespace Tiny3D
{
    class BoneData : public Object
    {
    public:
        static BoneDataPtr create(const String &name);

        String      mName;
        uint16_t     mParentBone;
        Matrix4     mOffsetMatrix;
        Matrix4     mLocalMatrix;

    protected:
        BoneData(const String &name);

    private:
        BoneData(const BoneData &rkOther);
        BoneData &operator =(const BoneData &rkOther);
    };
}


#endif  /*__T3D_BONE_DATA_H__*/
