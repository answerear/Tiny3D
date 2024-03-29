/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Component/T3DCamera.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderWindow.h"
#include "Component/T3DTransform3D.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    CameraPtr Camera::create()
    {
        return new Camera();
    }

    //--------------------------------------------------------------------------
    
    Camera::~Camera() 
    {
        if (mRenderTexture != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mRenderTexture);
            mRenderTexture = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getViewMatrix() const
    {
        // Transform3DPtr xformNode = getGameObject()->getComponent<Transform3D>();
        //
        // if (xformNode != nullptr)
        // {
        //     bool isDirty = xformNode->isDirty();
        //     if (isDirty)
        //     {
        //         const Transform &xform = xformNode->getLocalToWorldTransform();
        //         const Matrix4 &worldMatrix = xform.getAffineMatrix();
        //
        //         // 视图矩阵推导：
        //         // 其中C是相机进行世界变换的矩阵，
        //         //  T是平移变换
        //         //  R是旋转变换
        //         //  S是缩放变换
        //         //
        //         // 由 C = T * R * S
        //         // 得 C(-1) = (T * R * S) (-1) = S(-1) * R(-1) * T(-1)
        //         // 
        //
        //         // 旋转矩阵
        //         Matrix4 R = xform.getOrientation();
        //         // 旋转矩阵是正交矩阵，正交矩阵的逆矩阵是其转置矩阵
        //         Matrix4 invertR = R.transpose();
        //         // 平移矩阵
        //         Matrix4 invertT(false);
        //         invertT.makeTranslate(-xform.getTranslation());
        //         // 缩放矩阵
        //         Matrix4 invertS(false);
        //         const Vector3 &scale = xform.getScaling();
        //         invertS[0][0] = REAL_ONE / scale.x();
        //         invertS[1][1] = REAL_ONE / scale.y();
        //         invertS[2][2] = REAL_ONE / scale.z();
        //
        //         mViewMatrix = invertS * invertR * invertT;
        //     }
        // }
        
        return mViewMatrix;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getProjectMatrix() const
    {
        // if (mIsProjDirty)
        // {
        //     if (mProjectionType == Projection::kPerspective)
        //     {
        //         // 透视投影
        //         
        //         // P = | 1 / (aspectRatio * tan(fov / 2))  0                 0                             0                              |
        //         //     | 0                                 1 / tan(fov / 2)  0                             0                              |
        //         //     | 0                                 0                 -(far + near) / (far - near)  -2 * far * near / (far - near) |
        //         //     | 0                                 0                 -1                            0                              |
        //         
        //         const Radian radian = mFovY * REAL_HALF;
        //         const Real m11 = REAL_ONE / Math::tan(radian);
        //         const Real m00 = m11 / mAspectRatio;
        //         const Real m22 = -(mFar + mNear) / (mFar - mNear);
        //         const Real m23 = - 2 * mFar * mNear / (mFar - mNear);
        //     
        //         mProjectMatrix.make(
        //             m00, REAL_ZERO, REAL_ZERO, REAL_ZERO,
        //             REAL_ZERO, m11, REAL_ZERO, REAL_ZERO,
        //             REAL_ZERO, REAL_ZERO, m22, m23,
        //             REAL_ZERO, REAL_ZERO, -REAL_ONE, REAL_ZERO);
        //     }
        //     else
        //     {
        //         // 正交投影
        //
        //         // O = | 2 / width 0          0                  0                            |
        //         //     |      0    2 / height 0                  0                            |
        //         //     |      0    0          -2 / (far - near)  -(far + near) / (far - near) |
        //         //     |      0    0          0                  1                            |
        //
        //         const Real m00 = 2.0f / mWidth;
        //         const Real m11 = 2.0f / mHeight;
        //         const Real m22 = -2.0f / (mFar - mNear);
        //         const Real m23 = -(mFar + mNear) / (mFar - mNear);
        //
        //         mProjectMatrix.make(
        //             m00, REAL_ZERO, REAL_ZERO, REAL_ZERO,
        //             REAL_ZERO, m11, REAL_ZERO, REAL_ZERO,
        //             REAL_ZERO, REAL_ZERO, m22, m23,
        //             REAL_ZERO, REAL_ZERO, REAL_ZERO, REAL_ONE);
        //     }
        //     
        //     mIsProjDirty = false;
        // }
        
        return mProjectMatrix;
    }

    //--------------------------------------------------------------------------

    void Camera::setRenderTarget(RenderTargetPtr target)
    {
        if (target != mRenderTarget)
        {
            if (mRenderTexture != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mRenderTexture);
                mRenderTexture = nullptr;
            }

            if (target != nullptr)
            {
                switch (target->getType())
                {
                case RenderTarget::Type::E_RT_WINDOW:
                    {
                        RenderWindowPtr renderWindow = target->getRenderWindow();
                    
                        // 新建个 render texture ，相机先渲染到 render texture 上，然后再画到屏幕上
                        setupRenderTexture(renderWindow);
                    
                        // 新建个 quad ，用于最后 blit render texture 到 render windows 上用
                        setupQuad(renderWindow);
                    }
                    break;
                case RenderTarget::Type::E_RT_TEXTURE:
                    {
                        // 渲染目标本来就是渲染纹理，则不创建了渲染纹理，直接绘制到渲染纹理上
                    }
                    break;
                default:
                    break;
                }
            }
        }
        
        mRenderTarget = target;
    }

    //--------------------------------------------------------------------------

    ComponentPtr Camera::clone() const
    {
        CameraPtr camera = new Camera();
        TResult ret = camera->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            camera = nullptr;
        }
        return camera;
    }

    //--------------------------------------------------------------------------

    TResult Camera::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Component::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const Camera * other = static_cast<const Camera * const>(src);
            setRenderTarget(other->mRenderTarget);

            mOrder = other->mOrder;
            mCullingMask = other->mOrder;
            mViewport = other->mViewport;
            mClearColor = other->mClearColor;
            mClearDepth = other->mClearDepth;
            mClearStencil = other->mClearStencil;
            mViewMatrix = other->mViewMatrix;
            mProjectMatrix = other->mProjectMatrix;
            mIsProjDirty = true;
            mProjectionType = other->mProjectionType;
            mFovY = other->mFovY;
            mAspectRatio = other->mAspectRatio;
            mWidth = other->mWidth;
            mHeight = other->mHeight;
            mFar = other->mFar;
            mNear = other->mNear;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Camera::setupRenderTexture(RenderWindow *window)
    {
        String name = getGameObject()->getName() + "##RT";
        const RenderWindowDesc &desc = window->getDescriptor();
        PixelFormat format;
        switch (desc.ColorDepth)
        {
        case 8:
            format = PixelFormat::E_PF_PALETTE8;
            break;
        case 16:
            format = PixelFormat::E_PF_B5G6R5;
            break;
        case 24:
            format = PixelFormat::E_PF_B8G8R8;
            break;
        case 32:
            format = PixelFormat::E_PF_B8G8R8A8;
            break;
        default:
            format = PixelFormat::E_PF_B8G8R8;
            break;
        }
        mRenderTexture = T3D_TEXTURE_MGR.createRenderTexture(name, desc.Width, desc.Height, format, 1, desc.MSAA.Count, desc.MSAA.Quality);
    }

    //--------------------------------------------------------------------------

    void Camera::setupQuad(RenderWindow *window)
    {
        // if (mVB == nullptr)
        // {
        //     // 没有创建过才创建
        //     struct Vertex
        //     {
        //         Vector3 position;
        //         Vector2 uv;
        //     };
        //
        //     Vertex vertices[4] =
        //     {
        //         { Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f) },
        //         { Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f) },
        //         { Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f) },
        //         { Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f) }
        //     };
        //     
        //     // vertex input layout
        //     VertexDeclarationPtr decl = VertexDeclaration::create();
        //     decl->addAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3, VertexAttribute::Semantic::E_VAS_POSITION, 0);
        //     decl->addAttribute(0, 12, VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0);
        //     T3D_RENDER_BUFFER_MGR.addVertexDeclaration(decl);
        //
        //     // vertex buffer
        //     Buffer buffer;
        //     buffer.setData(&vertices, sizeof(vertices));
        //     T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), 4, buffer, MemoryType::kVRAM, Usage::kImmutable, kCPUNone);
        //
        //     
        // }
    }

    //--------------------------------------------------------------------------
}
