/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_RENDERER_H__
#define __T3D_GLES3_RENDERER_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    class GLES3Renderer
        : public RHIRenderer
        , public Singleton<GLES3Renderer>
    {
    public:
        static GLES3RendererPtr create();

        ~GLES3Renderer() override;

        TResult init() override;

        TResult destroy() override;

        void getEditorInfo(void *info, RenderWindow *window) override;

    protected:
        GLES3Renderer();

        void cleanup();
    };

    #define GLES3_RENDERER  (GLES3Renderer::getInstance())
    #define GLES3_CONTEXT   (smart_pointer_cast<GLES3Context>(GLES3_RENDERER.getContext()))
}


#endif  /*__T3D_GLES3_RENDERER_H__*/
