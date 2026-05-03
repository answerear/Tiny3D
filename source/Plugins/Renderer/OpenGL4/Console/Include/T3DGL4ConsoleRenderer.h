/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4CONSOLE_RENDERER_H__
#define __T3D_GL4CONSOLE_RENDERER_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4RendererConsole
        : public RHIRenderer
    {
    public:
        static GL4RendererConsolePtr create();

        virtual ~GL4RendererConsole();

        TResult init() override;
        TResult destroy() override;
        void getEditorInfo(void *info, RenderWindow *window) override;

    protected:
        GL4RendererConsole();
        void cleanup();
    };
}


#endif    /*__T3D_GL4CONSOLE_RENDERER_H__*/
