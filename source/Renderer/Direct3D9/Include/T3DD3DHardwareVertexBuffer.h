

#ifndef __T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__
#define __T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_D3D9RENDERER_API D3D9HardwareVertexBuffer : public HardwareVertexBuffer
    {
    public:
        D3D9HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer);
        virtual ~D3D9HardwareVertexBuffer();

    protected:
        LPDIRECT3DVERTEXBUFFER9     mpVertexBuffer;
    };
}


#endif  /*__T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__*/
