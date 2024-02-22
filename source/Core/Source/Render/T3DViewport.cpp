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


#include "Render/T3DViewport.h"
#include "Render/T3DRenderTarget.h"
// #include "Component/T3DCamera.h"
// #include "Scene/T3DSceneManager.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    // //--------------------------------------------------------------------------
    //
    // ViewportPtr Viewport::create(RenderTargetPtr target, Real left, Real top, Real width, Real height, long_t nZOrder)
    // {
    //     ViewportPtr viewport = new Viewport(target, left, top, width, height, nZOrder);
    //     // viewport->release();
    //     return viewport;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // Viewport::Viewport(RenderTargetPtr target, Real left, Real top, Real width, Real height, long_t zOrder)
    //     : mRenderTarget(target)
    //     , mLeft(left)
    //     , mTop(top)
    //     , mWidth(width)
    //     , mHeight(height)
    //     , mActualLeft(0)
    //     , mActualTop(0)
    //     , mActualWidth(0)
    //     , mActualHeight(0)
    //     , mClearFlags(0)
    //     , mClearZ(1.0f)
    //     , mBkgndColor(0, 0, 0)
    //     , mMatrix(false)
    // {
    //     updateDimensions();
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // Viewport::~Viewport()
    // {
    //
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Viewport::render()
    // {
    //     // T3D_SCENE_MGR.render(this);
    //     T3D_AGENT.getActiveRHIContext()->setViewport(this);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Viewport::setDimensions(Real left, Real top, Real width, Real height)
    // {
    //     mLeft = left;
    //     mTop = top;
    //     mWidth = width;
    //     mHeight = height;
    //
    //     updateDimensions();
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Viewport::updateDimensions()
    // {
    //     // size_t nWidth = mRenderTarget->getWidth();
    //     // size_t nHeight = mRenderTarget->getHeight();
    //
    //     // mActualLeft = size_t(nWidth * mLeft);
    //     // mActualTop = size_t(nHeight * mTop);
    //     // mActualWidth = size_t(nWidth * mWidth);
    //     // mActualHeight = size_t(nHeight * mHeight);
    //
    //     // RenderContextPtr renderer = T3D_AGENT.getActiveRenderer();
    //     // mMatrix = T3D_AGENT.getActiveRHIContext()->makeViewportMatrix(this);
    // }
}

