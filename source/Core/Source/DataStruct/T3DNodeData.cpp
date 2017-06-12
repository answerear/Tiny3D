
#include "T3DNodeData.h"

namespace Tiny3D
{
    NodeDataPtr NodeData::create(const String &name)
    {
        NodeDataPtr data = new NodeData(name);

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    NodeData::NodeData(const String &name)
        : mName(name)
        , mParent(0xFFFF)
        , mLocalMatrix(false)
    {

    }
}
