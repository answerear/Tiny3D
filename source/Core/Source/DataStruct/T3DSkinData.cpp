

#include "T3DSkinData.h"


namespace Tiny3D
{
    SkinDataPtr SkinData::create(const String &name, const Matrix4 &m)
    {
        SkinDataPtr skin = new SkinData(name, m);
        if (skin != nullptr)
        {
            skin->release();
        }

        return skin;
    }

    SkinData::SkinData(const String &name, const Matrix4 &m)
        : mName(name)
        , mBindPose(m)
    {

    }

    SkinData::~SkinData()
    {

    }
}
