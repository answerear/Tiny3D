

#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Renderer
    {
    public:
        static const char * const DIRECT3D9;
        static const char * const DIRECT3D11;
        static const char * const OPENGL3PLUS;
        static const char * const OPENGLES2;
        static const char * const OPENGLES3;

        Renderer();
        virtual ~Renderer();

        virtual TString getName() const = 0;

        virtual RenderWindow *createRenderWindow() = 0;
    };
}


#endif    /*__T3D_RENDERER_H__*/
