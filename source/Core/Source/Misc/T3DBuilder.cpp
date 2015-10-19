

#include "Misc/T3DBuilder.h"
#include "Render/T3DRenderer.h"
#include "Render/T3DRenderWindow.h"

#include "T3DPlatform.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Builder);

    Builder::Builder()
        : mSystem(new System())
    {
        
    }

    Builder::~Builder()
    {
        T3D_SAFE_DELETE(mSystem);
    }

    void Builder::enumerateRendererList(RendererNameList &rNameList) const
    {
        rNameList.clear();

        if (DeviceInfo::PLATFORM_WINDOWS == T3D_DEVICE_INFO.getPlatform())
        {
            rNameList.push_back(Renderer::DIRECT3D9);
            rNameList.push_back(Renderer::DIRECT3D11);
            rNameList.push_back(Renderer::OPENGL3PLUS);
        }
        else if (DeviceInfo::PLATFORM_MACOSX == T3D_DEVICE_INFO.getPlatform()
            || DeviceInfo::PLATFORM_LINUX == T3D_DEVICE_INFO.getPlatform())
        {
            rNameList.push_back(Renderer::OPENGL3PLUS);
        }
        else if (DeviceInfo::PLATFORM_IOS == T3D_DEVICE_INFO.getPlatform())
        {
            rNameList.push_back(Renderer::OPENGLES3);
        }
        else if (DeviceInfo::PLATFORM_ANDROID == T3D_DEVICE_INFO.getPlatform())
        {
            rNameList.push_back(Renderer::OPENGLES2);
        }
    }

    Renderer *Builder::createRenderer(const TString &strRendererName)
    {
        Renderer *pRender = nullptr;

        return pRender;
    }

    void Builder::setActiveRenderer(Renderer *pRenderer)
    {

    }

    RenderWindow *Builder::createRenderWindow(
        const RenderWindowCreateParam &rkCreatedParam, 
        const RenderWindowCreateParamEx &rkCreatedParamEx
        )
    {
        return nullptr;
    }

    bool Builder::run()
    {
        return true;
    }

    void Builder::setApplicationListener(ApplicationListener *pListener)
    {

    }
}
