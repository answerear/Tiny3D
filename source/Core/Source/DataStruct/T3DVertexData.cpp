

#include "DataStruct/T3DVertexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    VertexDataPtr VertexData::create()
    {
        VertexData *data = new VertexData();
        VertexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    VertexData::VertexData()
    {

    }

    VertexData::~VertexData()
    {

    }

    bool VertexData::addVertexStream(const VertexStreamPtr &stream)
    {
        mStreams.push_back(stream);
        return true;
    }

    bool VertexData::insertVertexStream(const VertexStreamPtr &stream, size_t index)
    {
        bool found = false;
        size_t i = 0;

        if (index <= mStreams.size())
        {
            mStreams.insert(mStreams.begin()+index, stream);
        }
        else
        {
            mStreams.push_back(stream);
        }

        return true;
    }

    bool VertexData::removeVertexStream(const VertexStreamPtr &stream)
    {
        bool found = false;

        auto itr = mStreams.begin();

        while (itr != mStreams.end())
        {
            if (stream == *itr)
            {
                found = true;
                mStreams.erase(itr);
                break;
            }

            ++itr;
        }

        return found;
    }

    bool VertexData::removeVertexStream(size_t index)
    {
        bool found = false;

        if (index < mStreams.size())
        {
            mStreams.erase(mStreams.begin()+index);
            found = true;
        }

        return found;
    }
}
