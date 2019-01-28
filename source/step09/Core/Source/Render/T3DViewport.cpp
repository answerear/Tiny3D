/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Render/T3DViewport.h"
#include "Render/T3DRenderTarget.h"


namespace Tiny3D
{
    ViewportPtr Viewport::create(RenderTarget *target,
        Real left, Real top, Real width, Real height, long_t nZOrder)
    {
        ViewportPtr viewport = new Viewport(target, left, top, width, height, 
            nZOrder);
        viewport->release();
        return viewport;
    }

    Viewport::Viewport(RenderTargetPtr target,
        Real left, Real top, Real width, Real height, long_t zOrder)
        : mRenderTarget(target)
        , mLeft(left)
        , mTop(top)
        , mWidth(width)
        , mHeight(height)
        , mActualLeft(0)
        , mActualTop(0)
        , mActualWidth(0)
        , mActualHeight(0)
        , mBkgndColor(0, 0, 0)
    {
        updateDimensions();
    }

    Viewport::~Viewport()
    {

    }

    void Viewport::update()
    {
        
    }

    void Viewport::setDimensions(Real left, Real top, Real width, Real height)
    {
        mLeft = left;
        mTop = top;
        mWidth = width;
        mHeight = height;

        updateDimensions();
    }

    void Viewport::updateDimensions()
    {
        int32_t nWidth = mRenderTarget->getWidth();
        int32_t nHeight = mRenderTarget->getHeight();

        mActualLeft = int32_t(nWidth * mLeft);
        mActualTop = int32_t(nHeight * mTop);
        mActualWidth = int32_t(nWidth * mWidth);
        mActualHeight = int32_t(nHeight * mHeight);

//         RendererPtr renderer = T3D_ENTRANCE.getActiveRenderer();
//         renderer->makeViewportMatrix(this, mMatrix);
    }
}

