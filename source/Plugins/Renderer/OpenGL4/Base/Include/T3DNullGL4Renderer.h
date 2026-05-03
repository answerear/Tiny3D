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
    {
    public:
        static NullGL4RendererPtr create();

        virtual ~NullGL4Renderer();

        TResult init() override;
        TResult destroy() override;
        void getEditorInfo(void *info, RenderWindow *window) override;

    protected:
        NullGL4Renderer();
        void cleanup();
    };
}


#endif    /*__T3D_NULLGL4_RENDERER_H__*/
