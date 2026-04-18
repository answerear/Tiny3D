/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4Window.h"
#include "T3DNullGL4Error.h"
#include "T3DNullGL4Renderer.h"


namespace Tiny3D
{
    NullGL4WindowPtr NullGL4Window::create()
    {
        return T3D_NEW NullGL4Window();
    }

    NullGL4Window::NullGL4Window() {}
    NullGL4Window::~NullGL4Window() {}

    TResult NullGL4Window::swapBuffers() { return T3D_OK; }
    TResult NullGL4Window::clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil) { return T3D_OK; }
    TResult NullGL4Window::resize(uint32_t w, uint32_t h) { return T3D_OK; }
    void *NullGL4Window::getNativeObject() const { return nullptr; }
}
