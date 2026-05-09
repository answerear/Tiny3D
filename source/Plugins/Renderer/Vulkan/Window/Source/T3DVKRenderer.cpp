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


#include "T3DVKRenderer.h"
#include "T3DVKContext.h"
#include "T3DVKRenderWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKRendererPtr VKRenderer::create()
    {
        return T3D_NEW VKRenderer();
    }
    
    //--------------------------------------------------------------------------

    VKRenderer::VKRenderer()
    {
        mName = VULKAN;
    }
    
    //--------------------------------------------------------------------------

    VKRenderer::~VKRenderer()
    {
        cleanup();
    }
    
    //--------------------------------------------------------------------------

    TResult VKRenderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            mContext = VKContext::create();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKRenderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mContext != nullptr)
            {
                VKContext *vkCtx = static_cast<VKContext *>(mContext.get());
                VkDevice device = vkCtx->getVkDevice();
                if (device != VK_NULL_HANDLE)
                    vkDeviceWaitIdle(device);

                // Clear cached render state (mCurrentRenderTarget, etc.) to
                // break indirect references to GPU resources that will be
                // released later.  Do NOT cleanup() here — VkDevice must stay
                // alive until all vkDestroyXXX calls are done.
                mContext->reset();
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void VKRenderer::getEditorInfo(void *info, RenderWindow *window)
    {
#ifdef T3D_EDITOR
        // TODO: Fill editor info for Vulkan
#endif
    }

    //--------------------------------------------------------------------------

    void VKRenderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
