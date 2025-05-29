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


#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Render/T3DRenderWindow.h"
#include "RHI/T3DRHIRenderTarget.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RenderTargetPtr RenderTarget::create(RenderWindow *renderWindow, RenderTexture *depthStencil)
    {
        return new RenderTarget(renderWindow, depthStencil);
    }
    
    //--------------------------------------------------------------------------

    RenderTargetPtr RenderTarget::create(RenderTexture *renderTexture, RenderTexture *depthStencil)
    {
        return new RenderTarget(renderTexture, depthStencil);
    }
    
    //--------------------------------------------------------------------------

    RenderTargetPtr RenderTarget::create(const RenderTextures &renderTextures, RenderTexture *depthStencil)
    {
        return new RenderTarget(renderTextures, depthStencil);
    }
    
    //--------------------------------------------------------------------------
    
    RenderTarget::RenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil)
        : mRenderWindow(renderWindow)
        , mDepthStencil(depthStencil)
    {

    }
    
    //--------------------------------------------------------------------------

    RenderTarget::RenderTarget(RenderTexture *renderTexture, RenderTexture *depthStencil)
        : mDepthStencil(depthStencil)
        , mNumOfTextures(1)
    {
        mRenderTextures[0] = renderTexture;
    }
    
    //--------------------------------------------------------------------------

    RenderTarget::RenderTarget(const RenderTextures &renderTextures, RenderTexture *depthStencil)
        : mDepthStencil(depthStencil)
    {
        for (size_t i = 0; i < renderTextures.size(); ++i)
        {
            mRenderTextures[i] = renderTextures[i];
        }

        mNumOfTextures = static_cast<uint32_t>(renderTextures.size());
    }
    
    //--------------------------------------------------------------------------

    RenderTarget::~RenderTarget()
    {

    }

    //--------------------------------------------------------------------------

    void RenderTarget::releaseAllResources()
    {
        for (RenderTexturePtr &rt : mRenderTextures)
        {
            if (rt != nullptr)
            {
                T3D_TEXTURE_MGR.unload(rt);
                rt = nullptr;
            }
        }

        mNumOfTextures = 0;

        if (mDepthStencil != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mDepthStencil);
            mDepthStencil = nullptr;
        }   
    }

    //--------------------------------------------------------------------------
}

