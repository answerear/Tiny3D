
#include "T3DModelData.h"

namespace Tiny3D
{
    ModelDataPtr ModelData::create()
    {
        ModelDataPtr data = new ModelData();

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    ModelData::ModelData()
        : mIsVertexShared(true)
    {

    }
}
