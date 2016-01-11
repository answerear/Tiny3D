
#ifndef __T3D_HARDWARE_VERTEX_BUFFER_H__
#define __T3D_HARDWARE_VERTEX_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareVertexBuffer : public HardwareBuffer
    {
    public:
        virtual ~HardwareVertexBuffer();

    protected:
        size_t  mVertexCount;
        size_t  mVertexSize;

    };

    class T3D_ENGINE_API VertexElement
    {
    public:
    };

    class T3D_ENGINE_API VertexDeclaration
    {

    };
}


#endif  /*__T3D_HARDWARE_VERTEX_BUFFER_H__*/
