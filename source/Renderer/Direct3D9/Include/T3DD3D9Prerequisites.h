

#ifndef __T3D_D3D9PREREQUISITES_H__
#define __T3D_D3D9PREREQUISITES_H__


#include <T3DPlatform.h>
#include <T3DLog.h>
#include <Tiny3D.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>


#if defined T3DD3D9RENDERER_EXPORT
    #define T3D_D3D9RENDERER_API        T3D_EXPORT_API
#else
    #define T3D_D3D9RENDERER_API        T3D_IMPORT_API
#endif


#define D3D_SAFE_RELEASE(p) \
    if ((p) != nullptr)   \
    {   \
        (p)->Release();   \
        (p) = nullptr;    \
    }


namespace Tiny3D
{
    class D3D9Renderer;
    class D3D9RenderWindow;
    class D3D9Texture;
    class D3D9HardwareVertexBuffer;
    class D3D9HardwareIndexBuffer;
    class D3D9HardwareBufferManager;
    class D3D9Material;
    class D3D9VertexDeclaration;
}


#endif  /*__T3D_D3D9PREREQUISITES_H__*/

