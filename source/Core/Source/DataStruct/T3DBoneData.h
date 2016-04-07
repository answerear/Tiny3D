

#ifndef __T3D_BONE_DATA_H__
#define __T3D_BONE_DATA_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    class BoneData : public Object
    {
    public:
        static BoneDataPtr create(const String &ID);

        virtual ~BoneData();

    protected:
        BoneData(const String &ID);

    private:
        BoneData(const BoneData &);
        BoneData &operator =(const BoneData &);

    public:
        BoneDataPtr mParent;
        BoneDataPtr mChild;
        BoneDataPtr mPrevSibling;
        BoneDataPtr mNextSibling;

        String      mID;
        Matrix4     mBoneTransform;
    };
}

#endif  /*__T3D_BONE_DATA_H__*/
