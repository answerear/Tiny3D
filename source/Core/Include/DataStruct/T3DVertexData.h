
#ifndef __T3D_VERTEX_DATA_H__
#define __T3D_VERTEX_DATA_H__


#include "Misc/T3DObject.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API VertexData : public Object
    {
    public:
        static VertexDataPtr create(VertexDeclaration *decl, HardwareVertexBuffer *buffer);

        virtual ~VertexData();

        VertexDataPtr clone(bool copyData = true) const;

        const VertexDeclarationPtr getDeclaration() const       { return mDeclaration; }
        const HardwareVertexBufferPtr getVertexBuffer() const   { return mVertexBuffer; }

    protected:
        VertexData(VertexDeclaration *decl, HardwareVertexBuffer *buffer);

    private:
        VertexData(const VertexData &);
        VertexData &operator =(const VertexData &);

    protected:
        VertexDeclarationPtr    mDeclaration;
        HardwareVertexBufferPtr mVertexBuffer;
    };
}


#endif  /*__T3D_VERTEX_DATA_H__*/
