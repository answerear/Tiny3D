

#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "Render/T3DRenderTarget.h"
#include "Misc/T3DEntrance.h"
#include "Listener/T3DApplicationListener.h"
#include "Math/T3DMatrix4.h"
#include <T3DLog.h>


namespace Tiny3D
{
    D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
        : mInstance(hInstance)
        , mD3D(nullptr)
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
        paramEx["instance"].setLong((long_t)mInstance);

        window->create("D3D9RenderWindow", rkCreateParam, paramEx);
        mD3DDevice = ((D3D9RenderWindow *)window)->getD3DDevice();

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
        HRESULT hr = S_OK;
        if (FAILED(hr = mD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 255), 1.0f, 0)))
        {

        }

        if (FAILED(hr = mD3DDevice->BeginScene()))
        {

        }

        return true;
    }

    bool D3D9Renderer::endRender()
    {
        HRESULT hr = S_OK;

        if (FAILED(hr = mD3DDevice->EndScene()))
        {

        }

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
        HRESULT hr;

        D3DMATRIX d3dmat = toD3DMatrix(mat);

        switch (state)
        {
        case Renderer::E_TS_VIEW:
            {
                if (FAILED(hr = mD3DDevice->SetTransform(D3DTS_VIEW, &d3dmat)))
                {

                }
            }
            break;
        case Renderer::E_TS_WORLD:
            {
                if (FAILED(hr = mD3DDevice->SetTransform(D3DTS_WORLD, &d3dmat)))
                {

                }
            }
            break;
        case Renderer::E_TS_PROJECTION:
            {
                if (FAILED(hr = mD3DDevice->SetTransform(D3DTS_PROJECTION, &d3dmat)))
                {

                }
            }
            break;
        }
    }

    const Matrix4 &D3D9Renderer::getTransform(TransformState state) const
    {
        return Matrix4::IDENTITY;
    }

    void D3D9Renderer::setMaterial(const MaterialPtr &material)
    {

    }

    void D3D9Renderer::setCullingMode(CullingMode mode)
    {
        DWORD d3dmode;

        mCullingMode = mode;

        switch (mode)
        {
        case CULL_NONE:
            {
                d3dmode = D3DCULL_NONE;
            }
            break;
        case CULL_CLOCKWISE:
            {
                d3dmode = D3DCULL_CW;
            }
            break;
        case CULL_ANTICLOCKWISE:
            {
                d3dmode = D3DCULL_CCW;
            }
            break;
        }

        HRESULT hr;
        if (FAILED(hr = mD3DDevice->SetRenderState(D3DRS_CULLMODE, d3dmode)))
        {

        }
    }

    void D3D9Renderer::setViewport(const ViewportPtr &viewport)
    {
        if (viewport != mViewport)
        {
            mViewport = viewport;

            setCullingMode(mCullingMode);

            D3DVIEWPORT9 d3dvp;

            d3dvp.X = viewport->getActualLeft();
            d3dvp.Y = viewport->getActualTop();
            d3dvp.Width = viewport->getActualWidth();
            d3dvp.Height = viewport->getActualHeight();
            d3dvp.MinZ = 0.0f;
            d3dvp.MaxZ = 1.0f;

            HRESULT hr;
            if (FAILED(hr = mD3DDevice->SetViewport(&d3dvp)))
            {

            }
        }
    }

    void D3D9Renderer::drawVertexList(PrimitiveType primitiveType, 
        const VertexListPtr &vertices, uint32_t startIdx, 
        uint32_t primitiveCount)
    {

    }

    void D3D9Renderer::drawIndexList(PrimitiveType primitiveType, 
        const VertexListPtr &vertices, const IndexListPtr &indicies, 
        uint32_t startIdx, uint32_t pritimitiveCount)
    {

    }

    void D3D9Renderer::makeProjectionMatrix(const Radian &rkFovY, Real aspect, 
        Real nearDist, Real farDist, bool ortho, Matrix4 &mat)
    {
        if (ortho)
        {
            // 正交投影
            Real tanThetaY = Math::Tan(rkFovY * Real(0.5));
            Real h = Real(1.0) / (nearDist * tanThetaY);
            Real w = h / aspect;
            Real q = Real(1.0) / (farDist - nearDist);
            Real qn = -nearDist * q;
            mat.makeZero();
            mat[0][0] = w;
            mat[1][1] = h;
            mat[2][2] = q;
            mat[2][3] = qn;
            mat[3][3] = 1.0;
        }
        else
        {
            // 透视投影
            Real tanThetaY = Math::Tan(rkFovY * Real(0.5));
            Real h = Real(1.0) / (tanThetaY);
            Real w = h / aspect;            
            Real q = farDist / (nearDist - farDist);
            Real qn = -nearDist * q;
            mat.makeZero();
            mat[0][0] = w;
            mat[1][1] = h;
            mat[2][2] = q;
            mat[2][3] = qn;
            mat[3][2] = 1.0;
        }
    }

    D3DMATRIX D3D9Renderer::toD3DMatrix(const Matrix4 &mat) const
    {
        // 转置矩阵
        // D3D9 使用行向量 i.e. V*M
        // Tiny3D, OpenGL 等用列向量 i.e. M*V
        D3DXMATRIX d3dMat;
        d3dMat.m[0][0] = mat[0][0];
        d3dMat.m[0][1] = mat[1][0];
        d3dMat.m[0][2] = mat[2][0];
        d3dMat.m[0][3] = mat[3][0];

        d3dMat.m[1][0] = mat[0][1];
        d3dMat.m[1][1] = mat[1][1];
        d3dMat.m[1][2] = mat[2][1];
        d3dMat.m[1][3] = mat[3][1];

        d3dMat.m[2][0] = mat[0][2];
        d3dMat.m[2][1] = mat[1][2];
        d3dMat.m[2][2] = mat[2][2];
        d3dMat.m[2][3] = mat[3][2];

        d3dMat.m[3][0] = mat[0][3];
        d3dMat.m[3][1] = mat[1][3];
        d3dMat.m[3][2] = mat[2][3];
        d3dMat.m[3][3] = mat[3][3];

        return d3dMat;
    }

    Matrix4 D3D9Renderer::toT3DMatrix(const D3DMATRIX &mat) const
    {
        Matrix4 t3dMat;
        t3dMat[0][0] = mat.m[0][0];
        t3dMat[1][0] = mat.m[0][1];
        t3dMat[2][0] = mat.m[0][2];
        t3dMat[3][0] = mat.m[0][3];

        t3dMat[0][1] = mat.m[1][0];
        t3dMat[1][1] = mat.m[1][1];
        t3dMat[2][1] = mat.m[1][2];
        t3dMat[3][1] = mat.m[1][3];

        t3dMat[0][2] = mat.m[2][0];
        t3dMat[1][2] = mat.m[2][1];
        t3dMat[2][2] = mat.m[2][2];
        t3dMat[3][2] = mat.m[2][3];

        t3dMat[0][3] = mat.m[3][0];
        t3dMat[1][3] = mat.m[3][1];
        t3dMat[2][3] = mat.m[3][2];
        t3dMat[3][3] = mat.m[3][3];

        return t3dMat;
    }
}
