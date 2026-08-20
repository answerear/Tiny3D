/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_METAL_WINDOW_H__
#define __T3D_METAL_WINDOW_H__


#include "T3DMetalPrerequisites.h"


namespace Tiny3D
{
    class MetalWindow : public RHIRenderWindow
    {
    public:
        static MetalWindowPtr create(RenderWindow *renderWindow, void *device);

        virtual ~MetalWindow();

        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;

        void *getLayer() const;

        void *acquireDrawable();

        void *getDrawable() const;

        void releaseDrawable();

        uint32_t getWidth() const { return mWidth; }

        uint32_t getHeight() const { return mHeight; }

    protected:
        MetalWindow();

        TResult init(RenderWindow *renderWindow, void *device);

        struct Impl;
        Impl        *mImpl {nullptr};
        uint32_t    mWidth {0};
        uint32_t    mHeight {0};
    };
}


#endif  /*__T3D_METAL_WINDOW_H__*/
