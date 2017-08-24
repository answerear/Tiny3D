/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#include "Render/T3DRenderTarget.h"
#include "Render/T3DViewport.h"


namespace Tiny3D
{
    RenderTargetPtr RenderTarget::create()
    {
        RenderTarget *rt = new RenderTarget();
        RenderTargetPtr ptr(rt);
        rt->release();
        return ptr;
    }

    RenderTarget::RenderTarget()
        : mWidth(0)
        , mHeight(0)
        , mColorDepth(0)
        , mIsActive(true)
    {

    }

    RenderTarget::~RenderTarget()
    {

    }

    void RenderTarget::update()
    {
        auto itr = mViewportList.begin();

        while (itr != mViewportList.end())
        {
            itr->second->update();
            ++itr;
        }
    }

    ViewportPtr RenderTarget::addViewport(SGCamera *camera, int32_t nZOrder, 
        Real left, Real top, Real width, Real height)
    {
        ViewportPtr viewport;

        auto itr = mViewportList.find(nZOrder);

        if (itr == mViewportList.end())
        {
            viewport = Viewport::create(camera, this, left, top, width, height, nZOrder);
            mViewportList.insert(ViewportValue(nZOrder, viewport));
        }

        return viewport;
    }

    void RenderTarget::removeViewport(int32_t nZOrder)
    {
        auto itr = mViewportList.find(nZOrder);

        if (itr != mViewportList.end())
        {
            mViewportList.erase(itr);
        }
    }

    void RenderTarget::removeAllViewports()
    {
        mViewportList.clear();
    }
}
