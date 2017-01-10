
#ifndef __T3D_VERTEX_DATA_H__
#define __T3D_VERTEX_DATA_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API VertexData : public Object
    {
    public:
        static VertexDataPtr create();

        virtual ~VertexData();

        VertexDataPtr clone(bool copyData = true) const;

        bool addVertexStream(const VertexStreamPtr &stream);
        bool insertVertexStream(const VertexStreamPtr &stream, size_t index);
        bool removeVertexStream(const VertexStreamPtr &stream);
        bool removeVertexStream(size_t index);

        const VertexStreamPtr &getStream(size_t index) const
        {
            T3D_ASSERT(index <= mStreams.size());
            return mStreams[index];
        }

        VertexStreamPtr &getStream(size_t index)
        {
            T3D_ASSERT(index <= mStreams.size());
            return mStreams[index];
        }

    protected:
        VertexData();

    private:
        VertexData(const VertexData &);
        VertexData &operator =(const VertexData &);

    protected:
        typedef std::vector<VertexStreamPtr>    Streams;
        typedef Streams::iterator               StreamsItr;
        typedef Streams::const_iterator         StreamsConstItr;

        Streams     mStreams;
    };
}


#endif  /*__T3D_VERTEX_DATA_H__*/
