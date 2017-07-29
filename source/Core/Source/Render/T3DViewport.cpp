/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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
#include "SceneGraph/T3DSGCamera.h"


namespace Tiny3D
{
    ViewportPtr Viewport::create(SGCamera *camera, RenderTarget *target, 
        Real left, Real top, Real width, Real height, int32_t nZOrder)
    {
        Viewport *vp = new Viewport(camera, target, left, top, width, height, nZOrder);
        ViewportPtr viewport(vp);
        vp->release();
        return viewport;
    }

    Viewport::Viewport(SGCamera *camera, RenderTarget *target, 
        Real left, Real top, Real width, Real height, int32_t nZOrder)
        : mCamera(camera)
        , mTarget(target)
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
        updateDimemsions();
    }

    Viewport::~Viewport()
    {

    }

    void Viewport::update()
    {
        mCamera->renderScene(this);
    }

    void Viewport::setDimensions(Real left, Real top, Real width, Real height)
    {
        mLeft = left;
        mTop = top;
        mWidth = width;
        mHeight = height;

        updateDimemsions();
    }

    void Viewport::updateDimemsions()
    {
        int32_t nWidth = mTarget->getWidth();
        int32_t nHeight = mTarget->getHeight();

        mActualLeft = int32_t(nWidth * mLeft);
        mActualTop = int32_t(nHeight * mTop);
        mActualWidth = int32_t(nWidth * mWidth);
        mActualHeight = int32_t(nHeight * mHeight);
    }
}
