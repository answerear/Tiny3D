/*******************************************************************************
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
 ******************************************************************************/

#include "Render/T3DRenderer.h"
#include "Render/T3DRenderTarget.h"
#include "Listener/T3DFrameListener.h"
#include <T3DPlatform.h>


namespace Tiny3D
{
    const char * const Renderer::DIRECT3D9 = "Direct3D9";
    const char * const Renderer::DIRECT3D11 = "Direct3D11";
    const char * const Renderer::OPENGL3PLUS = "OpenGL 3+";
    const char * const Renderer::OPENGLES2 = "OpenGL ES 2";
    const char * const Renderer::OPENGLES3 = "OpenGL ES 3";

    Renderer::Renderer()
    {
        mCullingMode = E_CULL_CLOCKWISE;
        mRenderMode = E_RM_SOLID;
    }

    Renderer::~Renderer()
    {

    }

    bool Renderer::renderOneFrame()
    {
        if (!fireFrameStarted())
            return false;

        RenderTargetListItr itr = mRenderTargets.begin();
        while (itr != mRenderTargets.end())
        {
            itr->second->update();
            ++itr;
        }

        if (!fireFrameEnded())
            return false;

        return true;
    }

    bool Renderer::attachRenderTarget(RenderTarget *target)
    {
        std::pair<RenderTargetListItr, bool> ret = 
            mRenderTargets.insert(RenderTargetListValue(target->getName(),
            target));

        return ret.second;
    }

    void Renderer::detachRenderTarget(const String &name)
    {
        mRenderTargets.erase(name);
    }

    RenderTargetPtr Renderer::getRenderTarget(const String &name)
    {
        RenderTargetListItr itr = mRenderTargets.find(name);

        if (itr != mRenderTargets.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    void Renderer::addFrameListener(FrameListener *listener)
    {
        mFrameListeners.insert(listener);
    }

    void Renderer::removeFrameListener(FrameListener *listener)
    {
        mFrameListeners.erase(listener);
    }

    bool Renderer::fireFrameStarted()
    {
        /// 每帧渲染开始，先让底层平台层更新一遍
        T3D_SYSTEM.process();

        uint64_t timestamp = DateTime::currentMSecsSinceEpoch();

        FrameEvent evt;
        evt.timeSinceLastEvent = (uint32_t)(timestamp - mLastEndTime);
        evt.timeSinceLastFrame = (uint32_t)(timestamp - mLastStartTime);

        mLastStartTime = timestamp;

        return fireFrameStarted(evt);
    }

    bool Renderer::fireFrameEnded()
    {
        uint64_t timestamp = DateTime::currentMSecsSinceEpoch();

        FrameEvent evt;
        evt.timeSinceLastEvent = (uint32_t)(timestamp - mLastStartTime);
        evt.timeSinceLastFrame = (uint32_t)(timestamp - mLastEndTime);

        mLastEndTime = timestamp;

        return fireFrameEnded(evt);
    }

    bool Renderer::fireFrameStarted(const FrameEvent &evt)
    {
        bool ret = true;
        FrameListenerListItr itr = mFrameListeners.begin();
        
        while (itr != mFrameListeners.end())
        {
            ret = (*itr)->onFrameStarted(evt);
            if (!ret)
                break;
            ++itr;
        }

        return ret;
    }

    bool Renderer::fireFrameEnded(const FrameEvent &evt)
    {
        bool ret = true;
        FrameListenerListItr itr = mFrameListeners.begin();

        while (itr != mFrameListeners.end())
        {
            ret = (*itr)->onFrameEnded(evt);
            if (!ret)
                break;
            ++itr;
        }

        return ret;
    }

    ViewportPtr Renderer::getViewport()
    {
        return mViewport;
    }

    Renderer::CullingMode Renderer::getCullingMode() const
    {
        return mCullingMode;
    }

    Renderer::RenderMode Renderer::getRenderMode() const
    {
        return mRenderMode;
    }
}
