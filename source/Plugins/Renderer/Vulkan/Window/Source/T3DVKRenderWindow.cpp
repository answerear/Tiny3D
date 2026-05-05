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


#include "T3DVKRenderWindow.h"
#include "T3DVKRenderer.h"
#include "T3DVKContext.h"

#define VK_DEVICE()    (VK_CONTEXT->getVkDevice())
#define VK_INSTANCE()  (VK_CONTEXT->getVkInstance())


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKRenderWindowPtr VKRenderWindow::create(RenderWindow *renderWindow)
    {
        VKRenderWindowPtr window = T3D_NEW VKRenderWindow();
        if (window != nullptr && !window->init(renderWindow))
        {
            window = nullptr;
        }
        return window;
    }

    //--------------------------------------------------------------------------

    VKRenderWindow::VKRenderWindow()
    {
    }

    //--------------------------------------------------------------------------

    VKRenderWindow::~VKRenderWindow()
    {
        VkDevice device = VK_DEVICE();
        VkInstance instance = VK_INSTANCE();

        if (device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(device);

            // Destroy framebuffers FIRST (they reference image views)
            for (auto framebuffer : VkFramebuffers)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
            VkFramebuffers.clear();

            // Destroy depth resources
            if (VkDepthImageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(device, VkDepthImageView, nullptr);
                VkDepthImageView = VK_NULL_HANDLE;
            }
            if (VkDepthImage != VK_NULL_HANDLE)
            {
                vkDestroyImage(device, VkDepthImage, nullptr);
                VkDepthImage = VK_NULL_HANDLE;
            }
            if (VkDepthImageMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, VkDepthImageMemory, nullptr);
                VkDepthImageMemory = VK_NULL_HANDLE;
            }

            // Destroy swap chain image views
            for (auto imageView : VkSwapChainImageViews)
            {
                vkDestroyImageView(device, imageView, nullptr);
            }
            VkSwapChainImageViews.clear();

            // Destroy swap chain
            if (VkSwapChain != VK_NULL_HANDLE)
            {
                vkDestroySwapchainKHR(device, VkSwapChain, nullptr);
                VkSwapChain = VK_NULL_HANDLE;
            }
        }

        // Destroy surface
        if (instance != VK_NULL_HANDLE && VkSurface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(instance, VkSurface, nullptr);
            VkSurface = VK_NULL_HANDLE;
        }
    }

    //--------------------------------------------------------------------------

    bool VKRenderWindow::init(RenderWindow *renderWindow)
    {
        return true;
    }

    //--------------------------------------------------------------------------

    TResult VKRenderWindow::swapBuffers()
    {
        return VK_CONTEXT->swapBackBuffer(this);
    }

    //--------------------------------------------------------------------------

    TResult VKRenderWindow::resize(uint32_t w, uint32_t h)
    {
        return VK_CONTEXT->resizeRenderWindow(this, w, h);
    }

    //--------------------------------------------------------------------------

    void *VKRenderWindow::getNativeObject() const
    {
        return (void *)VkSwapChain;
    }
}
