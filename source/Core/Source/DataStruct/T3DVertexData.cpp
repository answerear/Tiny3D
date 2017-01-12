

#include "DataStruct/T3DVertexData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    VertexDataPtr VertexData::create(const VertexDeclarationPtr &decl)
    {
        VertexData *data = new VertexData(decl);
        VertexDataPtr ptr(data);
        data->release();
        return ptr;
    }

    VertexData::VertexData(const VertexDeclarationPtr &decl)
        : mDeclaration(decl)
    {

    }

    VertexData::~VertexData()
    {

    }

    bool VertexData::addVertexBuffer(const HardwareVertexBufferPtr &buffer)
    {
        mBuffers.push_back(buffer);
        return true;
    }

    bool VertexData::insertVertexBuffer(const HardwareVertexBufferPtr &buffer, size_t index)
    {
        bool found = false;
        size_t i = 0;

        if (index <= mBuffers.size())
        {
            mBuffers.insert(mBuffers.begin()+index, buffer);
        }
        else
        {
            mBuffers.push_back(buffer);
        }

        return true;
    }

    bool VertexData::removeVertexBuffer(const HardwareVertexBufferPtr &buffer)
    {
        bool found = false;

        auto itr = mBuffers.begin();

        while (itr != mBuffers.end())
        {
            if (buffer == *itr)
            {
                found = true;
                mBuffers.erase(itr);
                break;
            }

            ++itr;
        }

        return found;
    }

    bool VertexData::removeVertexBuffer(size_t index)
    {
        bool found = false;

        if (index < mBuffers.size())
        {
            mBuffers.erase(mBuffers.begin()+index);
            found = true;
        }

        return found;
    }
}
