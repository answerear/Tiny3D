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


#include "T3DMetalWindow.h"
#include "T3DMetalError.h"
#include "Render/T3DRenderWindow.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>


namespace Tiny3D
{
    struct MetalWindow::Impl
    {
        CAMetalLayer            *layer {nil};
        id<CAMetalDrawable>     drawable {nil};
    };

    //--------------------------------------------------------------------------

    MetalWindowPtr MetalWindow::create(RenderWindow *renderWindow, void *device)
    {
        MetalWindowPtr window = T3D_NEW MetalWindow();
        if (window != nullptr && T3D_FAILED(window->init(renderWindow, device)))
        {
            window = nullptr;
        }
        return window;
    }

    //--------------------------------------------------------------------------

    MetalWindow::MetalWindow()
        : mImpl(new Impl())
    {

    }

    //--------------------------------------------------------------------------

    MetalWindow::~MetalWindow()
    {
        if (mImpl != nullptr)
        {
            mImpl->drawable = nil;
            mImpl->layer = nil;
            delete mImpl;
            mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    TResult MetalWindow::init(RenderWindow *renderWindow, void *device)
    {
        TResult ret = T3D_OK;

        do
        {
            if (renderWindow == nullptr || device == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            SysWMInfo info {};
            if (!renderWindow->getSystemInfo(info) || info.window == nullptr)
            {
                ret = T3D_ERR_METAL_LAYER;
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to get Cocoa NSWindow from RenderWindow !");
                break;
            }

            NSWindow *nsWindow = (__bridge NSWindow *)info.window;
            NSView *view = nsWindow.contentView;
            if (view == nil)
            {
                ret = T3D_ERR_METAL_LAYER;
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "NSWindow contentView is nil !");
                break;
            }

            id<MTLDevice> mtlDevice = (__bridge id<MTLDevice>)device;
            CAMetalLayer *layer = [CAMetalLayer layer];
            layer.device = mtlDevice;
            layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            // blit 到 drawable 需要关闭 framebufferOnly
            layer.framebufferOnly = NO;
            layer.opaque = YES;

            const RenderWindowDesc &desc = renderWindow->getDescriptor();
            const CGFloat pixelW = desc.Width > 0 ? (CGFloat)desc.Width : 1.0;
            const CGFloat pixelH = desc.Height > 0 ? (CGFloat)desc.Height : 1.0;
            layer.drawableSize = CGSizeMake(pixelW, pixelH);
            const NSSize viewSize = view.bounds.size;
            if (viewSize.width > 0.0)
            {
                layer.contentsScale = pixelW / viewSize.width;
            }

            view.wantsLayer = YES;
            view.layer = layer;

            mImpl->layer = layer;
            mWidth = (uint32_t)layer.drawableSize.width;
            mHeight = (uint32_t)layer.drawableSize.height;

            T3D_LOG_INFO(LOG_TAG_METALRENDERER,
                "CAMetalLayer attached. drawableSize=%ux%u contentsScale=%.2f",
                mWidth, mHeight, (float)layer.contentsScale);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetalWindow::swapBuffers()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalWindow::resize(uint32_t w, uint32_t h)
    {
        if (mImpl == nullptr || mImpl->layer == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        mImpl->layer.drawableSize = CGSizeMake(w > 0 ? (CGFloat)w : 1.0, h > 0 ? (CGFloat)h : 1.0);
        mWidth = (uint32_t)mImpl->layer.drawableSize.width;
        mHeight = (uint32_t)mImpl->layer.drawableSize.height;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void *MetalWindow::getNativeObject() const
    {
        return getLayer();
    }

    //--------------------------------------------------------------------------

    void *MetalWindow::getLayer() const
    {
        return (mImpl != nullptr) ? (__bridge void *)mImpl->layer : nullptr;
    }

    //--------------------------------------------------------------------------

    void *MetalWindow::acquireDrawable()
    {
        if (mImpl == nullptr || mImpl->layer == nil)
        {
            return nullptr;
        }

        mImpl->drawable = [mImpl->layer nextDrawable];
        return (__bridge void *)mImpl->drawable;
    }

    //--------------------------------------------------------------------------

    void *MetalWindow::getDrawable() const
    {
        return (mImpl != nullptr) ? (__bridge void *)mImpl->drawable : nullptr;
    }

    //--------------------------------------------------------------------------

    void MetalWindow::releaseDrawable()
    {
        if (mImpl != nullptr)
        {
            mImpl->drawable = nil;
        }
    }

    //--------------------------------------------------------------------------
}
