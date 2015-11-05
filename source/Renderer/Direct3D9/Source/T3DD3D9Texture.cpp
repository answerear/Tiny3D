

#ifndef __T3D_D3D9_RENDERER_H__
#define __T3D_D3D9_RENDERER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class D3D9Renderer : public Renderer
    {
    public:
        D3D9Renderer(HINSTANCE hInstance);
        virtual ~D3D9Renderer();

        virtual String getName() const;

        virtual RenderWindow *createRenderWindow(
            const RenderWindowCreateParam &rkCreateParam, 
            const RenderWindowCreateParamEx &rkCreateParamEx);

        virtual void startRendering();

    protected:
        HINSTANCE   mInstance;
    };
}


#endif  /*__T3D_D3D9_RENDERER_H__*/
