

#include "Render/T3DRenderer.h"
#include "Render/T3DRenderTarget.h"


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

    RenderTarget *Renderer::getRenderTarget(const String &name)
    {
        RenderTargetListItr itr = mRenderTargets.find(name);

        if (itr != mRenderTargets.end())
        {
            return itr->second;
        }

        return nullptr;
    }
}
