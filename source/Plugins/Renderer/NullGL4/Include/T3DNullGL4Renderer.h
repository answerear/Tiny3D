/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_NULLGL4_RENDERER_H__
#define __T3D_NULLGL4_RENDERER_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4Renderer
        : public RHIRenderer
        , public Singleton<NullGL4Renderer>
    {
    public:
        static NullGL4RendererPtr create();

        ~NullGL4Renderer() override;

        TResult init() override;
        TResult destroy() override;
        void getEditorInfo(void *info, RenderWindow *window) override;

    protected:
        NullGL4Renderer();
        void cleanup();
    };

    #define NULL_GL4_RENDERER      (NullGL4Renderer::getInstance())
}


#endif    /*__T3D_NULLGL4_RENDERER_H__*/
