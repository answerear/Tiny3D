

#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "T3DD3D9HardwareBufferManager.h"
#include "T3DD3D9HardwareVertexBuffer.h"
#include "T3DD3D9HardwareIndexBuffer.h"
#include "T3DD3D9VertexDeclaration.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(D3D9Renderer);

    D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
        : mInstance(hInstance)
        , mD3D(nullptr)
        , mD3DDevice(nullptr)
        , mHardwareBufferMgr(nullptr)
        , mD3DHwBufferMgr(new D3D9HardwareBufferManager())
    {
        mHardwareBufferMgr = new HardwareBufferManager(mD3DHwBufferMgr);
    }

    D3D9Renderer::~D3D9Renderer()
    {
        T3D_SAFE_DELETE(mD3DHwBufferMgr);
        T3D_SAFE_DELETE(mHardwareBufferMgr);
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

    bool D3D9Renderer::beginRender(const Color4 &bkgndColor)
    {
        HRESULT hr = S_OK;
        if (FAILED(hr = mD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, bkgndColor.A8R8G8B8(), 1.0f, 0)))
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
        case E_CULL_NONE:
            {
                d3dmode = D3DCULL_NONE;
            }
            break;
        case E_CULL_CLOCKWISE:
            {
                d3dmode = D3DCULL_CW;
            }
            break;
        case E_CULL_ANTICLOCKWISE:
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

    void D3D9Renderer::setRenderMode(RenderMode mode)
    {
        DWORD d3dmode = D3DFILL_SOLID;

        HRESULT hr;

        switch (mode)
        {
        case E_RM_POINT:
            {
                d3dmode = D3DFILL_POINT;
                hr = mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
                if (FAILED(hr))
                {

                }
            }
            break;
        case E_RM_WIREFRAME:
            {
                d3dmode = D3DFILL_WIREFRAME;
                hr = mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
                if (FAILED(hr))
                {

                }
            }
            break;
        case E_RM_SOLID:
        default:
            {
                d3dmode = D3DRS_FILLMODE;
                hr = mD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
                if (FAILED(hr))
                {

                }
            }
            break;
        }

        if (FAILED(hr = mD3DDevice->SetRenderState(D3DRS_FILLMODE, d3dmode)))
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
        const VertexDataPtr &vertexData, uint32_t startIdx, 
        uint32_t primitiveCount)
    {

    }

    void D3D9Renderer::drawIndexList(PrimitiveType primitiveType, 
        const VertexDataPtr &vertexData, const IndexDataPtr &indexData, 
        uint32_t startIdx, uint32_t pritimitiveCount)
    {
        HRESULT hr;

        VertexDeclarationPtr decl = vertexData->getDeclaration();
        HardwareVertexBufferPtr vb = vertexData->getVertexBuffer();
        D3D9HardwareVertexBuffer *vertices = (D3D9HardwareVertexBuffer *)(HardwareVertexBuffer *)vb;
        hr = mD3DDevice->SetStreamSource(0, vertices->getD3DVertexBuffer(), 0, decl->getVertexSize());

        HardwareIndexBufferPtr ib = indexData->getIndexBuffer();
        D3D9HardwareIndexBuffer *indices = (D3D9HardwareIndexBuffer *)(HardwareIndexBuffer *)ib;
        hr = mD3DDevice->SetIndices(indices->getD3DIndexBuffer());

        D3D9VertexDeclaration *vertexDecl = (D3D9VertexDeclaration *)(VertexDeclaration *)decl;
        hr = mD3DDevice->SetVertexDeclaration(vertexDecl->getD3D9VertexDeclaration());

        hr = mD3DDevice->DrawIndexedPrimitive(D3D9Mappings::get(primitiveType), 0, 0, vertices->getVertexCount(), 0, pritimitiveCount);
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
            Real q = Real(1.0) / (nearDist - farDist);
            Real qn = nearDist * q;

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
            //          | w 0  0  0 |
            //          | 0 h  0  0 |
            //      M = | 0 0  q qn |
            //          | 0 0 -1  0 |
            // 其中
            //      w = 1.0 / tan(Y/2) / aspect_ratio
            //      h = 1.0 / tan(Y/2)
            //      q = f / (n - f)
            //      qn = n * f / (n - f)

            Real tanThetaY = Math::Tan(rkFovY * Real(0.5));
            Real h = Real(1.0) / (tanThetaY);
            Real w = h / aspect;            
            Real q = farDist / (nearDist - farDist);
            Real qn = nearDist * q;

            mat.makeZero();
            mat[0][0] = w;
            mat[1][1] = h;
            mat[2][2] = q;
            mat[2][3] = qn;
            mat[3][2] = -1.0;
        }
    }

    void D3D9Renderer::updateFrustum(const Matrix4 &m, Plane *plane, size_t planeCount)
    {
        // 快速计算视棱锥六个裁剪平面原理：
        //
        //  假设点V'是投影变换后的点，V是投影变换前相机坐标系的点，M是投影变换矩阵，则可得：
        //      V' = M * V
        //  其中
        //      V' = (x' y' z' w')
        //
        //      V = (x y z w), (w = 1)
        //
        //          | m00 m01 m02 m03 |
        //      M = | m10 m11 m12 m13 |
        //          | m20 m21 m22 m23 |
        //          | m30 m31 m32 m33 |
        //  则
        //      | m00 m01 m02 m03 |   | x |   | x*m00 + y*m01 + z*m02 + w*m03 |   | V * row0 |
        //      | m10 m11 m12 m13 |   | y |   | x*m10 + y*m11 + z*m12 + w*m13 |   | V * row1 |
        //      | m20 m21 m22 m23 | * | z | = | x*m20 + y*m21 + z*m22 + w*m23 | = | V * row2 |
        //      | m30 m31 m32 m33 |   | w |   | x*m30 + y*m31 + z*m32 + w*m33 |   | V * row3 |
        //
        //  这里的*代表点积，而rowi = (mi0 mi1 mi2 mi3)
        // 
        //  经过转换后，V'在齐次裁剪空间里。这个空间实际上现在已经是一个轴对齐的盒子。
        //  如果V'在这个盒子空间里，则变换前的V点也在这个没有经过变换的盒子空间里。
        //  这里要分一下DX和OpenGL的区别，因为投影空间不一样：
        //
        //      1、在DX里，V'在这个空间里，则符合下面的不等式规则
        //              -w' < x' < w'
        //              -w' < y' < w'
        //               0 < z' < w'
        //          用 -w' < x' 推导：
        //              -(V * row3) < (V * row0)
        //          移动项得：
        //              0 < (V * row3) + (V * row0)
        //          合并同类项，最终可得：
        //              0 < V * (row3 + row0)
        //
        //          如此类推可得：
        //              left    :   0 < V * (row3 + row0)   a=m30+m00, b=m31+m01, c=m32+m02, d=m33+m03
        //              right   :   0 < V * (row3 - row0)   a=m30-m00, b=m31-m01, c=m32-m02, d=m33-m03
        //              bottom  :   0 < V * (row3 + row1)   a=m30+m10, b=m31+m11, c=m32+m12, d=m33+m13
        //              top     :   0 < V * (row3 - row1)   a=m30-m10, b=m31-m11, c=m32-m12, d=m33-m13
        //              near    :   0 < V * row2            a=m20,     b=m21,     c=m22,     d=m23
        //              far     :   0 < V * (row3 - row2)   a=m30-m20, b=m31-m21, c=m32-m22, d=m33-m23
        //

        T3D_ASSERT(planeCount == Frustum::E_MAX_FACE);

        // Left
        plane[Frustum::E_FACE_LEFT][0] = m[3][0] + m[0][0];
        plane[Frustum::E_FACE_LEFT][1] = m[3][1] + m[0][1];
        plane[Frustum::E_FACE_LEFT][2] = m[3][2] + m[0][2];
        plane[Frustum::E_FACE_LEFT][3] = m[3][3] + m[0][3];
        plane[Frustum::E_FACE_LEFT].normalize();

        // Right
        plane[Frustum::E_FACE_RIGHT][0] = m[3][0] - m[0][0];
        plane[Frustum::E_FACE_RIGHT][1] = m[3][1] - m[0][1];
        plane[Frustum::E_FACE_RIGHT][2] = m[3][2] - m[0][2];
        plane[Frustum::E_FACE_RIGHT][3] = m[3][3] - m[0][3];
        plane[Frustum::E_FACE_RIGHT].normalize();

        // Bottom
        plane[Frustum::E_FACE_BOTTOM][0] = m[3][0] + m[1][0];
        plane[Frustum::E_FACE_BOTTOM][1] = m[3][1] + m[1][1];
        plane[Frustum::E_FACE_BOTTOM][2] = m[3][2] + m[1][2];
        plane[Frustum::E_FACE_BOTTOM][3] = m[3][3] + m[1][3];
        plane[Frustum::E_FACE_BOTTOM].normalize();

        // Top
        plane[Frustum::E_FACE_TOP][0] = m[3][0] - m[1][0];
        plane[Frustum::E_FACE_TOP][1] = m[3][1] - m[1][1];
        plane[Frustum::E_FACE_TOP][2] = m[3][2] - m[1][2];
        plane[Frustum::E_FACE_TOP][3] = m[3][3] - m[1][3];
        plane[Frustum::E_FACE_TOP].normalize();

        // Near
        plane[Frustum::E_FACE_NEAR][0] = m[2][0];
        plane[Frustum::E_FACE_NEAR][1] = m[2][1];
        plane[Frustum::E_FACE_NEAR][2] = m[2][2];
        plane[Frustum::E_FACE_NEAR][3] = m[2][3];
        plane[Frustum::E_FACE_NEAR].normalize();

        // Far
        plane[Frustum::E_FACE_FAR][0] = m[3][0] - m[2][0];
        plane[Frustum::E_FACE_FAR][1] = m[3][1] - m[2][1];
        plane[Frustum::E_FACE_FAR][2] = m[3][2] - m[2][2];
        plane[Frustum::E_FACE_FAR][3] = m[3][3] - m[2][3];
        plane[Frustum::E_FACE_FAR].normalize();
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
