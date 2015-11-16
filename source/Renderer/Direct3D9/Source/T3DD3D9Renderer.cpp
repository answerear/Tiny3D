

#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "Render/T3DRenderTarget.h"
#include "Misc/T3DEntrance.h"
#include "Listener/T3DApplicationListener.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
        : mInstance(hInstance)
        , mD3D(nullptr)
        , mD3DDevice(nullptr)
    {

    }

    D3D9Renderer::~D3D9Renderer()
    {

    }

    String D3D9Renderer::getName() const
    {
        return DIRECT3D9;
    }

    RenderWindow *D3D9Renderer::createRenderWindow(
        const RenderWindowCreateParam &rkCreateParam, 
        const RenderWindowCreateParamEx &rkCreateParamEx)
    {
        RenderWindow *window = new D3D9RenderWindow();

        RenderWindowCreateParamEx paramEx = rkCreateParamEx;
        char buf[64] = {0};
        ltoa((long_t)mInstance, buf, 10);
        String s(buf);
        paramEx["instance"] = s;

        window->create("D3D9RenderWindow", rkCreateParam, paramEx);

        attachRenderTarget(window);

        return window;
    }

    void D3D9Renderer::startRendering()
    {
        if (Entrance::getInstance().getApplicationListener())
            Entrance::getInstance().getApplicationListener()->applicationDidFinishLaunching();

        MSG msg;

        // Render this window
        PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

        while (mRenderTargets.size())
        {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                    break;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (!fireFrameStarted())
                    break;

                RenderTargetListItr itr = mRenderTargets.begin();
                while (itr != mRenderTargets.end())
                {
                    itr->second->update();
                    ++itr;
                }

                if (!fireFrameEnded())
                    break;
            }
        }
    }

    bool D3D9Renderer::initialize()
    {
        mD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
        if (mD3D == nullptr)
            return false;

        // Prints device information.
        UINT uAdapter = 0;
        for (uAdapter = 0; uAdapter < mD3D->GetAdapterCount(); ++uAdapter)
        {
            D3DADAPTER_IDENTIFIER9 d3dai;
            D3DDISPLAYMODE d3ddm;
            mD3D->GetAdapterIdentifier(uAdapter, 0, &d3dai);
            mD3D->GetAdapterDisplayMode(uAdapter, &d3ddm);
            int a = 0;
        }

        return true;
    }

    void D3D9Renderer::uninitialize()
    {
        if (mD3D != nullptr)
        {
            mD3D->Release();
            mD3D = nullptr;
        }
    }

    bool D3D9Renderer::beginRender()
    {
        return true;
    }

    bool D3D9Renderer::endRender()
    {
        return true;
    }

    bool D3D9Renderer::queryCapability(Capability cap)
    {
        return false;
    }

    void D3D9Renderer::enableCapability(Capability cap, bool enabled /* = true */)
    {

    }

    void D3D9Renderer::setTransform(TransformState state, const Matrix4 &mat)
    {

    }

    const Matrix4 &D3D9Renderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    void D3D9Renderer::setMaterial()
    {

    }

    void D3D9Renderer::drawVertexList(PrimitiveType primitiveType, 
        const VertexList &vertices, uint32_t startIdx, 
        uint32_t primitiveCount)
    {

    }

    void D3D9Renderer::drawIndexList(PrimitiveType primitiveType, 
        const VertexList &vertices, const IndexList *indicies, 
        uint32_t startIdx, uint32_t pritimitiveCount)
    {

    }
}
