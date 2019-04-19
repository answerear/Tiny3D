/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "T3DD3D9HardwareBufferManager.h"
#include "T3DD3D9Error.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(D3D9Renderer);

    //--------------------------------------------------------------------------

    D3D9RendererPtr D3D9Renderer::create()
    {
        D3D9RendererPtr renderer = new D3D9Renderer();
        renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    D3D9Renderer::D3D9Renderer()
    {
        mName = Renderer::DIRECT3D9;
    }

    //--------------------------------------------------------------------------

    D3D9Renderer::~D3D9Renderer()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            // 创建 IDirect3D9 对象
            mD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
            if (nullptr == mD3D)
            {
                ret = T3D_ERR_D3D9_CREATE_FAILED;
                T3D_LOG_ERROR(LOG_TAG_D3D9RENDERER, "Create Direct3D9 \
                    interface failed !");
                break;
            }

            // 输出设备信息
            UINT uAdapter = 0;
            for (uAdapter = 0; uAdapter < mD3D->GetAdapterCount(); ++uAdapter)
            {
                D3DADAPTER_IDENTIFIER9 d3dai;
                D3DDISPLAYMODE d3ddm;
                mD3D->GetAdapterIdentifier(uAdapter, 0, &d3dai);
                mD3D->GetAdapterDisplayMode(uAdapter, &d3ddm);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            D3D_SAFE_RELEASE(mD3D);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr D3D9Renderer::createRenderWindow(const String &name,
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        TResult ret = T3D_OK;

        RenderWindowPtr window = nullptr;

        do
        {
            window = D3D9RenderWindow::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D9RENDERER, "Create render window \
                    failed !");
                break;
            }

            ret = window->create(param, paramEx);
            if (ret != T3D_OK)
            {
                window->release();
                window = nullptr;
                break;
            }

        } while (0);

        return window;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::beginRender(size_t count, Rect *rects,
        uint32_t clearFlags, const ColorRGB &color, Real z, uint32_t stencil)
    {
        TResult ret = T3D_OK;
        D3DRECT *pRects = nullptr;

        do 
        {
            Color4 clr;
            clr.from(color);
            HRESULT hr = S_OK;


            if (count > 0)
            {
                pRects = new D3DRECT[count];
                for (size_t i = 0; i < count; ++i)
                {
                    pRects[i].x1 = rects[i].left;
                    pRects[i].x2 = rects[i].right;
                    pRects[i].y1 = rects[i].top;
                    pRects[i].y2 = rects[i].bottom;
                }
            }

            if (FAILED(hr = mD3DDevice->Clear(count, pRects, 
                D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clr.A8R8G8B8(), 1.0f, 0)))
            {

            }

            if (FAILED(hr = mD3DDevice->BeginScene()))
            {

            }
        } while (0);

        T3D_SAFE_DELETE_ARRAY(pRects);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::endRender()
    {
        TResult ret = T3D_OK;

        do 
        {
            HRESULT hr = S_OK;

            if (FAILED(hr = mD3DDevice->EndScene()))
            {

            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool D3D9Renderer::queryCapability(Capability cap) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        TResult ret = T3D_OK;

        do 
        {
            HRESULT hr = D3D_OK;

            D3DMATRIX d3dmat = D3D9Mappings::toD3DMatrix(mat);

            switch (state)
            {
            case Renderer::E_TS_VIEW:
                hr = mD3DDevice->SetTransform(D3DTS_VIEW, &d3dmat);
            	break;
            case Renderer::E_TS_WORLD:
                hr = mD3DDevice->SetTransform(D3DTS_WORLD, &d3dmat);
                break;
            case Renderer::E_TS_PROJECTION:
                hr = mD3DDevice->SetTransform(D3DTS_PROJECTION, &d3dmat);
                break;
            default:
                break;
            }

            if (FAILED(hr))
            {

            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &D3D9Renderer::getTransform(TransformState state) const
    {
        HRESULT hr = D3D_OK;

        D3DMATRIX d3dmat;

        switch (state)
        {
        case Tiny3D::Renderer::E_TS_VIEW:
            hr = mD3DDevice->GetTransform(D3DTS_VIEW, &d3dmat);
            break;
        case Tiny3D::Renderer::E_TS_WORLD:
            hr = mD3DDevice->GetTransform(D3DTS_WORLD, &d3dmat);
            break;
        case Tiny3D::Renderer::E_TS_PROJECTION:
            hr = mD3DDevice->GetTransform(D3DTS_PROJECTION, &d3dmat);
            break;
        default:
            break;
        }

        if (FAILED(hr))
        {

        }

        return D3D9Mappings::toT3DMatrix(d3dmat);
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::perspective(Real left, Real right,
        Real top, Real bottom, Real nearDist, Real farDist)
    {
        // Direct3D 9 NDC (Normalized Device Coordinates) is : 
        //      [left, right] => [-1, 1]
        //      [bottom, top] => [-1, 1]
        //      [near, far] => [0, 1]
        //
        // 设：
        //      观察空间的点 : Ve = (Xe, Ye, Ze)
        //      投影平面的点 : Vp = (Xp, Yp, Zp)
        //      裁剪空间的点 : Vc = (Xc, Yc, Zc, Wc)
        //      NDC的点 : Vn = (Xn, Yn, Zn, Wn)
        //      左边界 : l
        //      右边界 : r
        //      上边界 : t
        //      下边界 : b
        //      近平面 : n
        //      远平面 : f
        //      投影矩阵 : Mp
        //
        // 根据相似三角形可得 :
        //      Xp / Xe = -n / Ze
        //  则 :
        //      Xp = (-n * Xe) / Ze = (n * Xe) / (-Ze)
        // 同理 :
        //      Yp / Ye = -n / Ze
        //  则 :
        //      Yp = (-n * Ye) / Ze = (n * Ye) / (-Ze)
        //
        // 因为 :
        //      | Xc |        | Xe |     | Xn |   | Xc/Wc |
        //      | Yc | = Mp * | Ye |     | Yn | = | Yc/Wc |
        //      | Zc |        | Ze |     | Zn |   | Zc/Wc |
        //      | Wc |        | We |
        // 又因为 : 
        //      | Xc |   | . . . . |   | Xe |
        //      | Yc | = | . . . . | * | Ye |
        //      | Zc |   | . . . . |   | Ze |
        //      | Wc |   | 0 0 1 0 |   | We |
        // 可得 : Wc = Ze
        //
        // 根据 Xp 和 Yp 映射到 Xn 和 Yn (NDC) 是线性关系，即 :
        //      [left, right] => [-1, 1]
        //      [bottom, top] => [-1, 1]
        // 得 :
        //      Xn = (1 - (-1)) * Xp / (r - l) + C
        // 把 (r, 1) 代入 (Xp, Xn) 可得 :
        //      1 = 2 * r / (r - l) + C
        // 求 C :
        //      C = - (r + l) / (r - l)
        // 代入原式，得 :
        //      Xn = 2Xp / (r - l) - (r + l) / (r - l)
        //
        // 同理可得 :
        //      Yn = 2Yp / (t - b) - (t + b) / (t - b)
        //
        // 进一步整理公式公因式 : 
        //      Xn = 2Xp / (r - l) - (r + l) / (r - l)
        //         = [2n/(r - l) * Xe + (r + l)/(r - l) * Ze] / -Ze
        //      Yn = 2Yp / (t - b) - (t + b) / (t - b)
        //         = [2n/(t - b) * Ye + (t + b)/(t - b) * Ze] / -Ze
        //
        // 从上式看得 :
        //      | Xc |   | 2n/(r-l)    0     (r+l)/(r-l) 0 |   | Xe |
        //      | Yc | = |    0     2n/(t-b) (t+b)/(t-b) 0 | * | Ye |
        //      | Zc |   |    0        0          A      B |   | Ze |
        //      | Wc |   |    0        0         -1      0 |   | We |
        // 则 :
        //      Zn = Zc / Wc = (A * Ze + B * We) / -Ze
        //
        // 观察空间中，We = 1，可得 :
        //      Zn = (A * Ze + B) / -Ze
        //
        // 现在问题变成求 A 和 B，我们利用(Ze, Zn)映射关系为(-n, 0)和(-f, 1) 
        // 代入上式，联列方程组得 :
        //      (-A * n + B) / n = 0
        //      (-A * f + B) / f = 1
        // 解方程组得 :
        //      A = f / (n - f)
        //      B = n * f / (n - f)
        //
        // 最终求得透视投影矩阵为 :
        //           | 2n/(r-l)    0     (r+l)/(r-l)    0      |
        //      Mp = |    0     2n/(t-b) (t+b)/(t-b)    0      |
        //           |    0        0        f/(n-f)  n*f/(n-f) |
        //           |    0        0         -1         0      |
        //
        return Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::orthographic(Real left, Real right,
        Real top, Real bottom, Real nearDist, Real farDist)
    {
        Real invert0 = REAL_ONE / (right - left);
        Real m00 = Real(2.0) * invert0;
        Real m03 = -(right + left) * invert0;
        Real invert1 = REAL_ONE / (top - bottom);
        Real m11 = Real(2.0) * invert1;
        Real m13 = -(top + bottom) * invert1;
        Real m22 = REAL_ONE / (nearDist - farDist);
        Real m23 = -farDist * m22;

        return Matrix4(
            m00,   0,   0, m03,
              0, m11,   0, m13,
              0,   0, m22, m23,
              0,   0,   0,   1);
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D9Renderer::makeViewportMatrix(ViewportPtr viewport)
    {
        Matrix4 mat(false);
        mat[0][0] = viewport->getActualWidth() * REAL_HALF;
        mat[1][1] = -viewport->getActualHeight() * REAL_HALF;
        mat[2][2] = REAL_ONE;
        mat[3][3] = REAL_ONE;
        mat[0][3] = viewport->getActualLeft() 
            + viewport->getActualWidth() * REAL_HALF;
        mat[1][3] = viewport->getActualTop() 
            + viewport->getActualHeight() * REAL_HALF;
        mat[2][3] = REAL_ZERO;
        return mat;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::updateFrustum(const Matrix4 &m, FrustumBoundPtr bound)
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

        Plane plane[Frustum::E_MAX_FACE];

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

        bound->setFrustumFaces(plane, Frustum::E_MAX_FACE);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setCullingMode(CullingMode mode)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mode == mCullingMode)
            {
                // 相同模式就不切换了
                break;
            }

            DWORD d3dmode;

            switch (mode)
            {
            case E_CULL_NONE:
                d3dmode = D3DCULL_NONE;
                break;
            case E_CULL_CLOCKWISE:
                d3dmode = D3DCULL_CW;
                break;
            case E_CULL_ANTICLOCKWISE:
                d3dmode = D3DCULL_CCW;
                break;
            default:
                d3dmode = D3DCULL_CCW;
                break;
            }

            HRESULT hr = D3D_OK;
            hr = mD3DDevice->SetRenderState(D3DRS_CULLMODE, d3dmode);
            if (FAILED(hr))
            {

            }

            mCullingMode = mode;

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setRenderMode(RenderMode mode)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mode == mRenderMode)
            {
                // 相同模式就不切换了
                break;
            }

            DWORD d3dmode;

            switch (mode)
            {
            case Tiny3D::Renderer::E_RM_POINT:
                d3dmode = D3DFILL_POINT;
                break;
            case Tiny3D::Renderer::E_RM_WIREFRAME:
                d3dmode = D3DFILL_WIREFRAME;
                break;
            case Tiny3D::Renderer::E_RM_SOLID:
                d3dmode = D3DFILL_SOLID;
                break;
            default:
                d3dmode = D3DFILL_SOLID;
                break;
            }

            HRESULT hr = D3D_OK;
            hr = mD3DDevice->SetRenderState(D3DRS_FILLMODE, d3dmode);
            if (FAILED(hr))
            {

            }

            mRenderMode = mode;

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setViewport(ViewportPtr viewport)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (viewport == mViewport)
            {
                // 视口相同就不切换了
                break;
            }

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

            mViewport = viewport;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::setMaterial(MaterialPtr material)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawVertexArray(VertexArrayObjectPtr vao)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawVertexList(PrimitiveType priType,
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D9Renderer::drawIndexList(PrimitiveType priType,
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo,
        HardwareIndexBufferPtr ibo)
    {
        TResult ret = T3D_OK;

        return ret;
    }
}

