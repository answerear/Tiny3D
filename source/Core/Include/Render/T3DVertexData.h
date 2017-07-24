
#ifndef __T3D_VERTEX_DATA_H__
#define __T3D_VERTEX_DATA_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API VertexData : public Object
    {
    public:
        static VertexDataPtr create(const VertexDeclarationPtr &decl);

        virtual ~VertexData();

        VertexDataPtr clone(bool copyData = true) const;

        bool addVertexBuffer(const HardwareVertexBufferPtr &buffer);
        bool insertVertexBuffer(const HardwareVertexBufferPtr &buffer, size_t index);
        bool removeVertexBuffer(const HardwareVertexBufferPtr &buffer);
        bool removeVertexBuffer(size_t index);

        const VertexDeclarationPtr &getDeclaration() const
        {
            return mDeclaration;
        }

        size_t getVertexBufferCount() const
        {
            return mBuffers.size();
        }

        const HardwareVertexBufferPtr &getVertexBuffer(size_t index) const
        {
            T3D_ASSERT(index <= mBuffers.size());
            return mBuffers[index];
        }

        HardwareVertexBufferPtr &getVertexBuffer(size_t index)
        {
            T3D_ASSERT(index <= mBuffers.size());
            return mBuffers[index];
        }

    protected:
        VertexData(const VertexDeclarationPtr &decl);

    private:
        VertexData(const VertexData &);
        VertexData &operator =(const VertexData &);

    protected:
        typedef std::vector<HardwareVertexBufferPtr>    VertexBuffers;
        typedef VertexBuffers::iterator                 VertexBuffersItr;
        typedef VertexBuffers::const_iterator           VertexBuffersConstItr;

        VertexDeclarationPtr    mDeclaration;
        VertexBuffers           mBuffers;
    };
}


#endif  /*__T3D_VERTEX_DATA_H__*/
