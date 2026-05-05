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


#ifndef __T3D_VK_RENDER_WINDOW_H__
#define __T3D_VK_RENDER_WINDOW_H__


#include "T3DVKPrerequisites.h"
#include <vector>


namespace Tiny3D
{
    class VKRenderWindow : public RHIRenderWindow
    {
    public:
        static VKRenderWindowPtr create(RenderWindow *renderWindow);

        ~VKRenderWindow() override;

        bool init(RenderWindow *renderWindow);

        TResult swapBuffers() override;

        TResult resize(uint32_t w, uint32_t h) override;

        void *getNativeObject() const override;

        /// Swap chain
        VkSwapchainKHR      VkSwapChain {VK_NULL_HANDLE};
        /// Win32 surface
        VkSurfaceKHR        VkSurface {VK_NULL_HANDLE};
        /// Swap chain images
        std::vector<VkImage>        VkSwapChainImages;
        /// Swap chain image views
        std::vector<VkImageView>    VkSwapChainImageViews;
        /// Framebuffers (one per swap chain image)
        std::vector<VkFramebuffer>  VkFramebuffers;
        /// Swap chain image format
        VkFormat            VkSwapChainImageFormat {VK_FORMAT_B8G8R8A8_UNORM};
        /// Swap chain extent
        VkExtent2D          VkSwapChainExtent {0, 0};
        /// Depth stencil image
        VkImage             VkDepthImage {VK_NULL_HANDLE};
        /// Depth stencil image memory
        VkDeviceMemory      VkDepthImageMemory {VK_NULL_HANDLE};
        /// Depth stencil image view
        VkImageView         VkDepthImageView {VK_NULL_HANDLE};
        /// Current image index in swap chain
        uint32_t            VkCurrentImageIndex {0};

    protected:
        VKRenderWindow();
    };
}



#endif  /*__T3D_VK_RENDER_WINDOW_H__*/
