

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

    }

    Renderer::~Renderer()
    {

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

        return RenderTargetPtr::NULL_PTR;
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
}
