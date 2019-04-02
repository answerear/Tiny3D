/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "T3DR3DRenderer.h"
#include "T3DR3DRenderWindow.h"
#include "T3DR3DHardwareBufferManager.h"
#include "T3DR3DError.h"
#include "T3DR3DFramebuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DRendererPtr R3DRenderer::create()
    {
        R3DRendererPtr renderer = new R3DRenderer();
        renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    R3DRenderer::R3DRenderer()
        : Renderer()
        , mFramebuffer(nullptr)
        , mRenderWindow(nullptr)
        , mHardwareBufferMgr(nullptr)
        , mR3DHardwareBufferMgr(nullptr)
        , mFrustumBound(nullptr)
    {
        mName = Renderer::REFERENCE3D;

        mR3DHardwareBufferMgr = R3DHardwareBufferManager::create();
        mHardwareBufferMgr 
            = HardwareBufferManager::create(mR3DHardwareBufferMgr);
    }

    R3DRenderer::~R3DRenderer()
    {
        mRenderWindow = nullptr;
        mHardwareBufferMgr = nullptr;
        mR3DHardwareBufferMgr = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::init()
    {
        size_t i = 0;

        for (i = 0; i < E_TS_MAX; ++i)
        {
            mMatrices[i].makeIdentity();
        }

        return T3D_OK;
    }

    TResult R3DRenderer::destroy()
    {
        mRenderWindow = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RenderWindowPtr R3DRenderer::createRenderWindow(
        const String &name,
        const RenderWindowCreateParam &param,
        const RenderWindowCreateParamEx &paramEx)
    {
        TResult ret = T3D_OK;

        RenderWindowPtr window = nullptr;

        do 
        {
            window = R3DRenderWindow::create(name);
            if (window == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_R3DRENDERER, "Create render window \
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

            mRenderWindow = window;
        } while (0);

        return window;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::beginRender(size_t count, Rect *rects,
        uint32_t clearFlags, const ColorRGB &color, Real z,
        uint32_t stencil)
    {
        if (mRenderTarget == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_R3DRENDERER, "Invalid render target !");
            return T3D_ERR_R3D_INVALID_TARGET;
        }

        // 生成framebuffer对象，用于当前一帧的渲染
        mFramebuffer = R3DFramebuffer::create(mRenderTarget);

        if (clearFlags & E_CLEAR_TARGET)
        {
            // 清除背景
            mFramebuffer->fill(color, count, rects);
        }
        
        if (clearFlags & E_CLEAR_ZBUFFER)
        {

        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::endRender()
    {
        mFramebuffer = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool R3DRenderer::queryCapability(Capability cap) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setTransform(TransformState state, const Matrix4 &mat)
    {
        mMatrices[state] = mat;
        return T3D_OK;
    }

    const Matrix4 &R3DRenderer::getTransform(TransformState state) const
    {
        return mMatrices[state];
    }

    //--------------------------------------------------------------------------

    Matrix4 R3DRenderer::perspective(Real left, Real right, Real top,
        Real bottom, Real nearDist, Real farDist)
    {
        // Reference3D NDC (Normalized Device Coordinates) is : 
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
        //      | Xc |   | . .  . . |   | Xe |
        //      | Yc | = | . .  . . | * | Ye |
        //      | Zc |   | . .  . . |   | Ze |
        //      | Wc |   | 0 0 -1 0 |   | We |
        // 可得 : Wc = -Ze
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

        Real width = right - left;
        Real height = top - bottom;
        Real distance = nearDist - farDist;

        Real invertW = REAL_ONE / width;
        Real invertH = REAL_ONE / height;
        Real invertD = REAL_ONE / distance;

        Real m00 = 2 * nearDist * invertW;
        Real m02 = (right + left) * invertW;
        Real m11 = 2 * nearDist * invertH;
        Real m12 = (top + bottom) * invertH;
        Real m22 = farDist * invertD;
        Real m23 = nearDist * farDist  * invertD;

        return Matrix4(
            m00, 0, m02, 0,
            0, m11, m12, 0,
            0, 0, m22, m23,
            0, 0, -1, 0);
    }

    //--------------------------------------------------------------------------

    Matrix4 R3DRenderer::orthographic(Real left, Real right, Real top,
        Real bottom, Real nearDist, Real farDist)
    {
        // 参考透视投影的推导
        //
        //      | 2/(r-l)     0       0    -(r+l)/(r-l) |
        // Mp = |    0     2/(t-b)    0    -(t+b)/(t-b) |
        //      |    0        0    1/(n-f)    n/(n-f)   |
        //      |    0        0       0         1       |

        Real width = right - left;
        Real height = top - bottom;
        Real distance = nearDist - farDist;

        Real invertW = REAL_ONE / width;
        Real invertH = REAL_ONE / height;
        Real invertD = REAL_ONE / distance;

        Real m00 = 2 * invertW;
        Real m03 = -(right + left) * invertW;
        Real m11 = 2 * invertH;
        Real m13 = -(top - bottom) * invertH;
        Real m22 = invertD;
        Real m23 = nearDist * invertD;

        return Matrix4(
            m00,   0,   0, m03,
              0, m11,   0, m13,
              0,   0, m22, m23,
              0,   0,   0,   1);
    }

    //--------------------------------------------------------------------------

    Matrix4 R3DRenderer::makeViewportMatrix(ViewportPtr viewport)
    {
        Real m00 = viewport->getActualWidth() * REAL_HALF;
        Real m11 = -viewport->getActualHeight() * REAL_HALF;
        Real m22 = REAL_ONE;
        Real m33 = REAL_ONE;
        Real m03 = viewport->getActualLeft() + viewport->getActualWidth() * REAL_HALF;
        Real m13 = viewport->getActualTop() + viewport->getActualHeight() * REAL_HALF;
        Real m23 = REAL_ONE;
        return Matrix4(
            m00,   0,   0, m03,
              0, m11,   0, m13,
              0,   0, m22, m23,
              0,   0,   0, m33);
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::updateFrustum(const Matrix4 &m, FrustumBoundPtr bound)
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
        //  
        //  在 Reference3D 里，V'在这个空间里，则符合下面的不等式规则
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
        //              left    :   0 < V * (row3 + row0)   
        //                          a=m30+m00, b=m31+m01, c=m32+m02, d=m33+m03
        //              right   :   0 < V * (row3 - row0)   
        //                          a=m30-m00, b=m31-m01, c=m32-m02, d=m33-m03
        //              bottom  :   0 < V * (row3 + row1)   
        //                          a=m30+m10, b=m31+m11, c=m32+m12, d=m33+m13
        //              top     :   0 < V * (row3 - row1)   
        //                          a=m30-m10, b=m31-m11, c=m32-m12, d=m33-m13
        //              near    :   0 < V * row2            
        //                          a=m20,     b=m21,     c=m22,     d=m23
        //              far     :   0 < V * (row3 - row2)   
        //                          a=m30-m20, b=m31-m21, c=m32-m22, d=m33-m23
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

        if (mFrustumBound != bound)
            mFrustumBound = bound;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setCullingMode(CullingMode mode)
    {
        mCullingMode = mode;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setRenderMode(RenderMode mode)
    {
        mRenderMode = mode;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setViewport(ViewportPtr viewport)
    {
        mViewport = viewport;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::setMaterial(MaterialPtr material)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawVertexArray(VertexArrayObjectPtr vao)
    {
        TResult ret = T3D_OK;

        size_t indexCount = 0;
        uint8_t *indices = nullptr;
        bool is16Bits = false;

        if (vao->isIndicesUsed())
        {
            auto ibo = vao->getIndexBuffer();
            indexCount = ibo->getIndexCount();
            indices = (uint8_t*)ibo->lock(HardwareBuffer::E_HBL_READ_ONLY);
            is16Bits = (ibo->getIndexType()==HardwareIndexBuffer::E_IT_16BITS);
        }

        Vertex *vertices = nullptr;

        do 
        {
            mMV = mMatrices[E_TS_VIEW] * mMatrices[E_TS_WORLD];
            mMVP = mMatrices[E_TS_PROJECTION]
                * mMatrices[E_TS_VIEW]
                * mMatrices[E_TS_WORLD];

            auto vbo = vao->getVertexBuffer(0);
            size_t vertexCount = vbo->getVertexCount();
            vertices = new Vertex[vertexCount];

            ret = processVertices(vao, vertices, vertexCount);
            if (ret != T3D_OK)
            {
                break;
            }

            PrimitiveType primitive = vao->getPrimitiveType();

            switch (primitive)
            {
            case Renderer::E_PT_POINT_LIST:
                {
                    // Point list
                    ret = processPointList(vertices, vertexCount, indices, 
                        indexCount, is16Bits);
                }
                break;
            case Renderer::E_PT_LINE_LIST:
                {
                    // Line list
                    
                }
                break;
            case Renderer::E_PT_LINE_STRIP:
                {
                    // Line strip
                }
                break;
            case Renderer::E_PT_TRIANGLE_LIST:
                {
                    // Triangle list
                }
                break;
            case Renderer::E_PT_TRIANGLE_STRIP:
                {
                    // Triangle strip
                }
                break;
            case Renderer::E_PT_TRIANGLE_FAN:
                {
                    // Triangle fan
                }
                break;
            default:
                {
                    T3D_LOG_ERROR(LOG_TAG_R3DRENDERER, "Unsupported primitive type !");
                    ret = T3D_ERR_R3D_INVALID_PRIMITIVE;
                }
                break;
            }
        } while (0);

        T3D_SAFE_DELETE_ARRAY(vertices);

        if (vao->isIndicesUsed())
        {
            auto ibo = vao->getIndexBuffer();
            ibo->unlock();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawVertexList(PrimitiveType priType, 
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo)
    {
        VertexArrayObjectPtr vao 
            = T3D_HARDWARE_BUFFER_MGR.createVertexArrayObject(false);

        vao->setPrimitiveType(priType);
        vao->setVertexDeclaration(decl);
        vao->addVertexBuffer(vbo);

        return drawVertexArray(vao);
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::drawIndexList(PrimitiveType priType, 
        VertexDeclarationPtr decl, HardwareVertexBufferPtr vbo, 
        HardwareIndexBufferPtr ibo)
    {
        VertexArrayObjectPtr vao
            = T3D_HARDWARE_BUFFER_MGR.createVertexArrayObject(true);

        vao->setPrimitiveType(priType);
        vao->setVertexDeclaration(decl);
        vao->addVertexBuffer(vbo);
        vao->setIndexBuffer(ibo);

        return drawVertexArray(vao);
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::processVertices(VertexArrayObjectPtr vao, 
        Vertex *vertices, size_t vertexCount)
    {
        TResult ret = T3D_OK;

        struct BufferInfo
        {
            uint8_t *buffer;
            size_t  vertexSize;
            size_t  vertexCount;
        };

        TArray<BufferInfo> buffers(vao->getVertexBufferCount());
        size_t i = 0;
        for (i = 0; i < vao->getVertexBufferCount(); ++i)
        {
            auto vbo = vao->getVertexBuffer(0);

            BufferInfo info;
            info.buffer = (uint8_t*)vbo->lock(HardwareBuffer::E_HBL_READ_ONLY);
            info.vertexSize = vbo->getVertexSize();
            info.vertexCount = vbo->getVertexCount();
            if (info.vertexCount != vertexCount)
            {
                ret = T3D_ERR_R3D_MISMATCH_VERTEX_COUNT;
                break;
            }
            buffers.push_back(info);
        }

        if (ret == T3D_OK)
        {
            VertexDeclarationPtr decl = vao->getVertexDeclaration();

            const VertexDeclaration::VertexAttriList &attributes
                = decl->getAttributes();

            auto itr = attributes.begin();
            while (itr != attributes.end())
            {
                auto attr = *itr;
                size_t idx = attr.getStream();
                auto info = buffers[idx];
                processVertices(info.buffer, info.vertexSize, attr, vertices,
                    vertexCount);

                ++itr;
            }
        }

        for (i = 0; i < vao->getVertexBufferCount(); ++i)
        {
            auto vbo = vao->getVertexBuffer(0);
            vbo->unlock();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::processVertices(uint8_t *buffer, size_t vertexSize,
        const VertexAttribute &attr, Vertex *vertices, size_t vertexCount)
    {
        TResult ret = T3D_OK;

        size_t start = 0;
        size_t srcOffset = attr.getOffset();
        size_t size = attr.getSize();
        size_t idx = 0;
        size_t dstOffset = 0;
        size_t dstVertexSize = sizeof(Vertex);
        bool needTransform = false;

        switch (attr.getSemantic())
        {
        case VertexAttribute::E_VAS_POSITION:
            dstOffset = 0;
            needTransform = true;
            break;
        case VertexAttribute::E_VAS_NORMAL:
            dstOffset = sizeof(Vector4);
            needTransform = true;
            break;
        case VertexAttribute::E_VAS_DIFFUSE:
            dstOffset = sizeof(Vector4) + sizeof(Vector4) + sizeof(Vector2);
            break;
        case VertexAttribute::E_VAS_SPECULAR:
            dstOffset = sizeof(Vector4) + sizeof(Vector4) + sizeof(Vector2)
                + sizeof(ColorRGB);
            break;
        case VertexAttribute::E_VAS_TEXCOORD:
            dstOffset = sizeof(Vector4) + sizeof(Vector4);
            break;
        }

        size_t i = 0;

        for (i = 0; i < vertexCount; ++i)
        {
            size_t offset = start + srcOffset;

            if (VertexAttribute::E_VAS_POSITION == attr.getSemantic())
            {
                memcpy(&vertices[i].pos, &buffer[offset], sizeof(Real) * 3);
                vertices[i].pos.w() = REAL_ONE;
                vertices[i].pos = mMVP * vertices[i].pos;
            }
            else if (VertexAttribute::E_VAS_NORMAL == attr.getSemantic())
            {
                memcpy(&vertices[i].normal, &buffer[offset], sizeof(Real) * 3);
                vertices[i].normal.w() = REAL_ZERO;
                vertices[i].normal = mMV * vertices[i].normal;
                vertices[i].normal.normalize();
            }
            else
            {
                memcpy(&vertices[dstOffset], &buffer[offset], size);
            }

            vertices += dstVertexSize;
            start += size;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::processPointList(Vertex *vertices, size_t vertexCount,
        uint8_t *indices, size_t indexCount, bool is16Bits)
    {
        TResult ret = T3D_OK;

        if (indexCount == 0 || indices == nullptr)
        {
            // 不使用顶点索引
            Vertex *verts = nullptr;
            size_t vertCount = 0;

            // 视锥体裁剪
            clipPointList(vertices, vertexCount, verts, vertCount);

            T3D_SAFE_DELETE_ARRAY(verts);
        }
        else
        {
            // 使用顶点索引
            uint8_t *dstIndices = nullptr;
            size_t dstIndexCount = 0;

            // 视锥体裁剪
            clipIndexPointList(vertices, vertexCount, indices, indexCount,
                dstIndices, dstIndexCount, is16Bits);

            T3D_SAFE_DELETE_ARRAY(dstIndices);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::clipPointList(Vertex *srcVerts, size_t srcVertCount, 
        Vertex *&dstVerts, size_t &dstVertCount)
    {
        TResult ret = T3D_OK;

        dstVerts = new Vertex[srcVertCount];
        dstVertCount = 0;
        size_t i = 0;

        IntrPointFrustum intr;
        const Frustum &frustum = mFrustumBound->getFrustum();
        intr.setFrustum(&frustum);

        for (i = 0; i < srcVertCount; ++i)
        {
            const Vector4 &pt = srcVerts[i].pos;
            Vector3 pos(pt.x(), pt.y(), pt.z());
            intr.setPoint(&pos);
            if (intr.test())
            {
                // 在视锥体内，不用裁剪
                dstVerts[dstVertCount] = srcVerts[i];
                dstVertCount++;
            }
            else
            {
                // 裁减掉
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult R3DRenderer::clipIndexPointList(Vertex *vertices,
        size_t vertexCount, uint8_t *srcIndices, size_t srcIdxCount,
        uint8_t *&dstIndices, size_t &dstIdxCount, bool is16Bits)
    {
        TResult ret = T3D_OK;

        if (is16Bits)
        {
            uint16_t *indices = new uint16_t[srcIdxCount];
            dstIndices = (uint8_t*)indices;
            size_t i = 0;
            uint16_t *srcIdx = (uint16_t*)srcIndices;
            dstIdxCount = 0;

            IntrPointFrustum intr;
            const Frustum &frustum = mFrustumBound->getFrustum();
            intr.setFrustum(&frustum);

            for (i = 0; i < srcIdxCount; ++i)
            {
                uint16_t index = srcIdx[i];
                const Vector4 &pt = vertices[index].pos;
                Vector3 pos(pt.x(), pt.y(), pt.z());
                intr.setPoint(&pos);
                if (intr.test())
                {
                    // 在视锥体内，不用裁剪
                    indices[dstIdxCount] = index;
                    dstIdxCount++;
                }
                else
                {
                    // 裁减掉
                }
            }
        }
        else
        {

        }

        return ret;
    }
}
