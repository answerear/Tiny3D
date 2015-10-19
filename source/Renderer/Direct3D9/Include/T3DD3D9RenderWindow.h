

#ifndef __T3D_D3D9_RENDER_WINDOW_H__
#define __T3D_D3D9_RENDER_WINDOW_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    class D3D9RenderWindow : public RenderWindow
    {
    public:
        D3D9RenderWindow();
        virtual ~D3D9RenderWindow();
    };
}


#endif  /*__T3D_D3D9_RENDER_WINDOW_H__*/
