

#ifndef __T3D_D3D9_MAPPING_H__
#define __T3D_D3D9_MAPPING_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class D3D9Mappings
    {
    public:
        static D3DPRIMITIVETYPE get(Renderer::PrimitiveType priType);
        static DWORD get(HardwareBuffer::Usage usage);
        static DWORD get(HardwareBuffer::Usage usage, HardwareBuffer::LockOptions options);
        static D3DFORMAT get(HardwareIndexBuffer::Type type);
        static D3DDECLTYPE get(VertexElement::Type type);
        static D3DDECLUSAGE get(VertexElement::Semantic semantic);
        static D3DFORMAT get(PixelFormat format);
        static PixelFormat get(D3DFORMAT d3dfmt);
        static D3DCOLORVALUE get(const Color4 &color);
    };
}


#endif  /*__T3D_D3D_MAPPING_H__*/
