
#include "T3DSkinData.h"

namespace Tiny3D
{
    SkinDataPtr SkinData::create(const String &name)
    {
        SkinDataPtr data = new SkinData(name);

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    SkinData::SkinData(const String &name)
        : mName(name)
        , mOffsetMatrix(false)
    {

    }
}
