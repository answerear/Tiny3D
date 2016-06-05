

#include "T3DBoneData.h"


namespace Tiny3D
{
    BonePtr Bone::create(const String &name, const Matrix4 &m)
    {
        BonePtr bone = new Bone(name, m);
        if (bone != nullptr)
        {
            bone->release();
        }
        else
        {
            T3D_SAFE_RELEASE(bone);
        }
        return bone;
    }

    Bone::Bone(const String &name, const Matrix4 &m)
        : SGTransformNode()
        , mName(name)
    {

    }

    Bone::~Bone()
    {

    }
}

