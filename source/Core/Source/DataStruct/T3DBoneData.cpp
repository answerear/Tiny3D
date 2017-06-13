
#include "T3DBoneData.h"

namespace Tiny3D
{
    BoneDataPtr BoneData::create(const String &name)
    {
        BoneDataPtr data = new BoneData(name);

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    BoneData::BoneData(const String &name)
        : NodeData(name)
        , mOffsetMatrix(false)
        , mIsMatrixDirty(true)
    {

    }
}
