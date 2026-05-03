/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleWindow.h"
#include "T3DGL4ConsoleError.h"
#include "T3DGL4ConsoleRenderer.h"


namespace Tiny3D
{
    GL4ConsoleWindowPtr GL4ConsoleWindow::create()
    {
        return T3D_NEW GL4ConsoleWindow();
    }

    GL4ConsoleWindow::GL4ConsoleWindow() {}
    GL4ConsoleWindow::~GL4ConsoleWindow() {}

    TResult GL4ConsoleWindow::swapBuffers() { return T3D_OK; }
    TResult GL4ConsoleWindow::resize(uint32_t w, uint32_t h) { return T3D_OK; }
    void *GL4ConsoleWindow::getNativeObject() const { return nullptr; }
}
