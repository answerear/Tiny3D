/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4CONSOLE_WINDOW_H__
#define __T3D_GL4CONSOLE_WINDOW_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    class GL4ConsoleWindow : public RHIRenderWindow
    {
    public:
        static GL4ConsoleWindowPtr create();
        virtual ~GL4ConsoleWindow();

        TResult swapBuffers() override;
        TResult resize(uint32_t w, uint32_t h) override;
        void *getNativeObject() const override;

    protected:
        GL4ConsoleWindow();
    };
}


#endif  /*__T3D_GL4CONSOLE_WINDOW_H__*/
