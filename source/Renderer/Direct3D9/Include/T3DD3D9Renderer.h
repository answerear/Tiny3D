

#ifndef __T3D_D3D9_RENDERER_H__
#define __T3D_D3D9_RENDERER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class D3D9Renderer : public Renderer
    {
    public:
        D3D9Renderer();
        virtual ~D3D9Renderer();
    };
}


#endif  /*__T3D_D3D9_RENDERER_H__*/
