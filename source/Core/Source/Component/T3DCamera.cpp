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


#include "T3DConfig.h"
#include "Component/T3DCamera.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderWindow.h"
#include "Component/T3DTransform3D.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"
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

    TResult Camera::lookAt(const Vector3 &eye, const Vector3 &obj, const Vector3 &up)
    {
        TResult ret = T3D_OK;

        do
        {
            if (getGameObject() == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_COMPONENT, "Game object is null when camera look at !");
                break;
            }

            Transform3D *xform = getGameObject()->getComponent<Transform3D>();
            if (xform == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_COMPONENT, "Transform3D is nullptr when camera look at !");
                break;
            }

            // 构造相机的三个坐标轴，U(x)，V(y)，N(z)
#if (T3D_COORDINATION_RH)
            // 这里使用右手系，所以相机空间里相机是看向 -z 轴方向
            Vector3 N = eye - obj;
            N.normalize();
            Vector3 V = up;
            V.normalize();
            Vector3 U = V.cross(N);
            U.normalize();
            V = N.cross(U);
            V.normalize();
#else
            // 这里使用左手系，所以相机空间里相机是看向 z 轴方向
            Vector3 N = obj - eye;
            N.normalize();
            Vector3 V = up;
            V.normalize();
            Vector3 U = V.cross(N);
            U.normalize();
            V = N.cross(U);
            V.normalize();
#endif
            
            // 设置相机位置
            xform->setPosition(eye);

            // 设置相机朝向
            Matrix3 mat;
            mat.setColumn(0, U);
            mat.setColumn(1, V);
            mat.setColumn(2, N);
            Quaternion orientation;
            orientation.fromRotationMatrix(mat);
            xform->setOrientation(orientation);

            // 设置相机缩放，UVN 相机是没有缩放的
            xform->setScaling(Vector3::UNIT_SCALE);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getViewMatrix() const
    {
        if (mIsViewDirty)
        {
            const Transform &xform = mXformNode->getLocalToWorldTransform();
        
            // 视图矩阵推导：
            // 其中C是相机进行世界变换的矩阵，
            //  T是平移变换
            //  R是旋转变换
            //  S是缩放变换
            //
            // 由 C = T * R * S
            // 得 C^(-1) = (T * R * S)^(-1) = S^(-1) * R^(-1) * T^(-1)
            // 
        
            // 旋转矩阵
            Matrix4 R = xform.getOrientation();
            // 旋转矩阵是正交矩阵，正交矩阵的逆矩阵是其转置矩阵
            Matrix4 invertR = R.transpose();
            // 平移矩阵
            Matrix4 invertT(false);
            invertT.setTranslate(-xform.getTranslation());
            // 缩放矩阵
            Matrix4 invertS(false);
            const Vector3 &scale = xform.getScaling();
            invertS[0][0] = REAL_ONE / scale.x();
            invertS[1][1] = REAL_ONE / scale.y();
            invertS[2][2] = REAL_ONE / scale.z();

#if (T3D_COORDINATION_RH)
            mViewMatrix = invertS * invertR * invertT;
#else
            Matrix4 matFlipZ(false);
            matFlipZ[2][2] = -1.0f;
            mViewMatrix = matFlipZ * invertS * invertR * invertT;
#endif
            mIsViewDirty = false;
        }
        
        return mViewMatrix;
    }

    //--------------------------------------------------------------------------

    const Matrix4 &Camera::getProjectionMatrix() const
    {
        if (mIsProjDirty)
        {
            // M_gl2dx = | 1 0   0   0 |
            //           | 0 1   0   0 |
            //           | 0 0 0.5 0.5 |
            //           | 0 0   0   1 |
            //
            // M_dx2gl = | 1 0 0  0 |
            //           | 0 1 0  0 |
            //           | 0 0 2 -1 |
            //           | 0 0 0  1 |
            //
            // 不管是左手坐标系还是右手坐标系，far 和 near 均是非负数
            
            if (mProjectionType == Projection::kPerspective)
            {
                // 透视投影

                // P_dx_lh = | 1/(aspectRatio*tan(fov*0.5)) 0                0              0                      |
                //           | 0                            1/tan(fov * 0.5) 0              0                      |
                //           | 0                            0                far/(far-near) -(far*near)/(far-near) |
                //           | 0                            0                1              0                      |
                //
                // P_dx_rh = | 1/(aspectRatio*tan(fov/0.5)) 0                0               0                      |
                //           | 0                            1/tan(fov / 0.5) 0               0                      |
                //           | 0                            0                -far/(far-near) -(far*near)/(far-near) |
                //           | 0                            0                -1              0                      |
                //
                // P_gl_lh = | 1/(aspectRatio*tan(fov*0.5)) 0              0                     0                        |
                //           | 0                            1/tan(fov*0.5) 0                     0                        |
                //           | 0                            0              (far+near)/(far-near) -(2*far*near)/(far-near) |
                //           | 0                            0              1                     0                        |
                //
                // P_gl_rh = | 1/(aspectRatio*tan(fov*0.5)) 0              0                      0                      |
                //           | 0                            1/tan(fov*0.5) 0                      0                      |
                //           | 0                            0              -(far+near)/(far-near) -2*far*near/(far-near) |
                //           | 0                            0              -1                     0                      |

                // P_dx_lh = M_gl2dx * P_gl_lh
                // P_dx_rh = M_gl2dx * P_gl_rh
                // P_gl_lh = M_dx2gl * P_dx_lh
                // p_gl_rh = M_dx2gl * P_dx_rh
                
                // 这里使用 OpenGL RH 作为透视投影矩阵
                const Radian radian = mFovY * REAL_HALF;
                const Real m11 = REAL_ONE / Math::tan(radian);
                const Real m00 = m11 / mAspectRatio;
                const Real m22 = -(mFar + mNear) / (mFar - mNear);
                const Real m23 = - 2 * mFar * mNear / (mFar - mNear);

                mProjectMatrix.make(
                    m00, REAL_ZERO, REAL_ZERO, REAL_ZERO,
                    REAL_ZERO, m11, REAL_ZERO, REAL_ZERO,
                    REAL_ZERO, REAL_ZERO, m22, m23,
                    REAL_ZERO, REAL_ZERO, -REAL_ONE, REAL_ZERO);
            }
            else
            {
                // 正交投影

                // O_dx_lh = | 2/width 0        0            0                |
                //           | 0       2/height 0            0                |
                //           | 0       0        1/(far-near) -near/(far-near) |
                //           | 0       0        0            1                |
                //
                // O_dx_rh = | 2/width 0        0             0                |
                //           | 0       2/height 0             0                |
                //           | 0       0        -1/(far-near) -near/(far-near) |
                //           | 0       0        0             1                |
                //
                // O_gl_lh = | 2/width 0        0            0                      |
                //           | 0       2/height 0            0                      |
                //           | 0       0        2/(far-near) -(far+near)/(far-near) |
                //           | 0       0        0            1                      |
                //
                // O_gl_rh = | 2/width 0        0             0                      |
                //           | 0       2/height 0             0                      |
                //           | 0       0        -2/(far-near) -(far+near)/(far-near) |
                //           | 0       0        0             1                      |

                // O_dx_lh = M_gl2dx * O_gl_lh
                // O_dx_rh = M_gl2dx * O_gl_rh
                // O_gl_lh = M_dx2gl * O_dx_lh
                // O_gl_rh = M_dx2gl * O_dx_rh
                
                // 这里使用 OpenGL RH 作为正交投影矩阵
                Real h = mOrthographicSize * 2.0f;
                Real w = mAspectRatio * h;
                const Real m00 = 2.0f / w;
                const Real m11 = 2.0f / h;
                const Real m22 = -2.0f / (mFar - mNear);
                const Real m23 = -(mFar + mNear) / (mFar - mNear);
        
                mProjectMatrix.make(
                    m00, REAL_ZERO, REAL_ZERO, REAL_ZERO,
                    REAL_ZERO, m11, REAL_ZERO, REAL_ZERO,
                    REAL_ZERO, REAL_ZERO, m22, m23,
                    REAL_ZERO, REAL_ZERO, REAL_ZERO, REAL_ONE);
            }
            
            mIsProjDirty = false;
        }
        
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
            mOrthographicSize = other->mOrthographicSize;
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

    void Camera::onStart()
    {
        GameObject *go = getGameObject();
        if (go != nullptr)
        {
            Transform3D *xform = go->getComponent<Transform3D>();
            xform->addPositionChangedCallback(this, [this](const Vector3&, const Vector3&)
                {
                    mIsViewDirty = true;
                });

            xform->addOrientationChangedCallback(this, [this](const Quaternion&, const Quaternion&)
                {
                    mIsViewDirty = true;
                });

            xform->addScalingChangedCallback(this, [this](const Vector3&, const Vector3&)
                {
                    mIsViewDirty = true;
                });

            mXformNode = getGameObject()->getComponent<Transform3D>();
        }

        T3D_SCENE_MGR.getCurrentScene()->addCamera(this);
    }

    //--------------------------------------------------------------------------

    void Camera::onDestroy()
    {
        GameObject *go = getGameObject();
        if (go != nullptr)
        {
            Transform3D *xform = go->getComponent<Transform3D>();
            if (xform != nullptr)
            {
                xform->removePositionChangedCallback(this);
                xform->removeOrientationChangedCallback(this);
                xform->removeScalingChangedCallback(this);
            }
        }

        T3D_SCENE_MGR.getCurrentScene()->removeCamera(this);
        
        Component::onDestroy();
    }

    //--------------------------------------------------------------------------
}
