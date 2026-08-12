/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, publish, distribute, sublicense, and/or sell
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

#ifndef __T3D_CAMERA_H__
#define __T3D_CAMERA_H__


#include "Component/T3DComponent.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DViewport.h"


namespace Tiny3D
{
    /**
     * \brief 场景相机组件，管理视图/投影矩阵、渲染目标与清屏参数
     * \remarks onStart 注册到当前 Scene 并监听 Transform3D 变换回调以标记视图矩阵脏；
     *          onDestroy 反注册并移除变换回调。
     */
    TCLASS()
    class T3D_ENGINE_API Camera : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 投影矩阵类型
         */
        TENUM()
        enum class Projection : uint32_t
        {
            /// 正交投影
            kOrthographic = 0,
            /// 透视投影
            kPerspective
        };

        /**
         * \brief 相机背景清屏方式
         */
        TENUM()
        enum class ClearFlags : uint32_t
        {
            /// 画天空盒；找不到天空盒材质时自动回退为 kSolidColor
            kSkybox = 0,
            /// 用 ClearColor 填充背景
            kSolidColor,
            /// 只清深度模板，保留上一次的颜色内容
            kDepthOnly,
            /// 什么都不清
            kNothing
        };
        
        /**
         * \brief 创建带新生成 UUID 的 Camera 实例
         * \return 新建的 Camera 智能指针
         */
        static CameraPtr create();

        /// 析构并释放 mSrcRenderTarget 资源
        ~Camera() override;

        /**
         * \brief enabled 开关对本组件有效
         * \return 恒返回 true；关闭后不再渲染且不出现在管线相机列表
         */
        bool supportsEnabled() const override { return true; }

        /**
         * \brief 克隆当前 Camera
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 设置相机 Transform3D 的位置、朝向与单位缩放，实现 look-at
         * \param [in] eye : 相机世界位置
         * \param [in] obj : 观察目标点
         * \param [in] up : 上方向
         * \return 成功返回 T3D_OK；GameObject 或 Transform3D 为空返回 T3D_ERR_INVALID_POINTER
         */
        TResult lookAt(const Vector3 &eye, const Vector3 &obj, const Vector3 &up);

        /// 返回最终渲染目标
        RenderTarget *getRenderTarget() const { return mRenderTarget; }

        /**
         * \brief 设置渲染目标；窗口目标时会自动创建中间 RenderTexture
         * \param [in] target : 新的渲染目标，可为 nullptr
         */
        void setRenderTarget(RenderTargetPtr target);

        /// 返回实际绘制用的源渲染目标（窗口相机时为中间 RenderTexture 目标）
        RenderTarget *getSrcRenderTarget() const { return mSrcRenderTarget;}

        /// 返回源渲染目标的颜色 RenderTexture
        RenderTexturePtr getRenderTexture() const { return mSrcRenderTarget->getRenderTexture(); }

        /// 返回相机渲染排序值
        TPROPERTY(RTTRFuncName="Order", RTTRFuncType="getter")
        uint32_t getOrder() const { return mOrder; }

        /**
         * \brief 设置渲染排序并在当前 Scene 中重新注册
         * \param [in] order : 新的排序值
         */
        TPROPERTY(RTTRFuncName="Order", RTTRFuncType="setter")
        void setOrder(uint32_t order) { updateOrder(order); }

        // 反射注册按属性名的字母序排列，Viewport 排在最后，因此这里的分组标题只会
        // 覆盖它自己。分组标题只能在字母序上划边界，无法像 Unity 那样任意归组。
        // 标签还必须写成一行，跨行的标签会被预处理器漏掉（见 CHECK_TAG_RET_FILE_SPEC）
        /// 返回渲染目标上的视口子区域
        TPROPERTY(RTTRFuncName="Viewport", RTTRFuncType="getter", "HEADER"="Output", "TOOLTIP"="Sub-region of the render target to draw into.")
        const Viewport &getViewport() const { return mViewport; }

        /**
         * \brief 设置视口
         * \param [in] vp : 视口参数
         */
        TPROPERTY(RTTRFuncName="Viewport", RTTRFuncType="setter")
        void setViewport(const Viewport &vp) { mViewport = vp; }

        /// 返回背景清屏方式
        TPROPERTY(RTTRFuncName="ClearFlags", RTTRFuncType="getter")
        ClearFlags getClearFlags() const { return mClearFlags; }

        /**
         * \brief 设置背景清屏方式
         * \param [in] flags : 清屏标志
         */
        TPROPERTY(RTTRFuncName="ClearFlags", RTTRFuncType="setter")
        void setClearFlags(ClearFlags flags) { mClearFlags = flags; }

        /// 返回背景填充颜色
        TPROPERTY(RTTRFuncName="ClearColor", RTTRFuncType="getter")
        const ColorRGB &getClearColor() const { return mClearColor; }

        /**
         * \brief 设置背景填充颜色
         * \param [in] color : RGB 颜色
         */
        TPROPERTY(RTTRFuncName="ClearColor", RTTRFuncType="setter")
        void setClearColor(const ColorRGB &color) { mClearColor = color; }

        /// 返回深度缓冲填充值
        TPROPERTY(RTTRFuncName="ClearDepth", RTTRFuncType="getter", "RANGE_MIN"=0, "RANGE_MAX"=1)
        Real getClearDepth() const { return mClearDepth; }

        /**
         * \brief 设置深度缓冲填充值
         * \param [in] depth : 深度值，范围 0~1
         */
        TPROPERTY(RTTRFuncName="ClearDepth", RTTRFuncType="setter")
        void setClearDepth(Real depth) { mClearDepth = depth; }

        /// 返回模板缓冲填充值
        TPROPERTY(RTTRFuncName="ClearStencil", RTTRFuncType="getter")
        uint8_t getClearStencil() const { return mClearStencil; }

        /**
         * \brief 设置模板缓冲填充值
         * \param [in] stencil : 模板值
         */
        TPROPERTY(RTTRFuncName="ClearStencil", RTTRFuncType="setter")
        void setClearStencil(uint8_t stencil) { mClearStencil = stencil; }

        /// 返回投影类型
        TPROPERTY(RTTRFuncName="ProjectionType", RTTRFuncType="getter")
        Projection getProjectionType() const { return mProjectionType; }

        /**
         * \brief 设置投影类型并标记投影矩阵脏
         * \param [in] type : 透视或正交
         */
        TPROPERTY(RTTRFuncName="ProjectionType", RTTRFuncType="setter")
        void setProjectionType(Projection type)
        {
            if (mProjectionType != type)
            {
                mProjectionType = type; mIsProjDirty = true;
            }
        }

        /// 返回垂直视场角（透视投影）
        TPROPERTY(RTTRFuncName="FovY", RTTRFuncType="getter", "DISPLAY_NAME"="Field of View", "RANGE_MIN"=1, "RANGE_MAX"=179)
        const Radian &getFovY() const { return mFovY; }

        /**
         * \brief 设置垂直视场角并标记投影矩阵脏
         * \param [in] fovY : 视场角
         */
        TPROPERTY(RTTRFuncName="FovY", RTTRFuncType="setter")
        void setFovY(const Radian &fovY) { mFovY = fovY; mIsProjDirty = true; }

        /// 返回宽高比
        TPROPERTY(RTTRFuncName="AspectRatio", RTTRFuncType="getter")
        const Real &getAspectRatio() const { return mAspectRatio; }

        /**
         * \brief 设置宽高比并标记投影矩阵脏
         * \param [in] ratio : 宽高比
         */
        TPROPERTY(RTTRFuncName="AspectRatio", RTTRFuncType="setter")
        void setAspectRatio(const Real &ratio) { mAspectRatio = ratio; mIsProjDirty = true; }

        /// 返回正交相机半高度
        TPROPERTY(RTTRFuncName="OrthographicSize", RTTRFuncType="getter")
        const Real &getOrthographicSize() const { return mOrthographicSize; }

        /**
         * \brief 设置正交相机半高度并标记投影矩阵脏
         * \param [in] size : 半高度
         */
        TPROPERTY(RTTRFuncName="OrthographicSize", RTTRFuncType="setter")
        void setOrthographicSize(const Real &size) { mOrthographicSize = size; mIsProjDirty = true; }

        /// 返回远裁剪面距离
        TPROPERTY(RTTRFuncName="FarPlane", RTTRFuncType="getter")
        const Real &getFarPlaneDistance() const { return mFar; }

        /**
         * \brief 设置远裁剪面距离并标记投影矩阵脏
         * \param [in] f : 远平面距离
         */
        TPROPERTY(RTTRFuncName="FarPlane", RTTRFuncType="setter")
        void setFarPlaneDistance(const Real &f) { mFar = f; mIsProjDirty = true; }

        /// 返回近裁剪面距离
        TPROPERTY(RTTRFuncName="NearPlane", RTTRFuncType="getter")
        const Real &getNearPlaneDistance() const { return mNear; }

        /**
         * \brief 设置近裁剪面距离并标记投影矩阵脏
         * \param [in] n : 近平面距离
         */
        TPROPERTY(RTTRFuncName="NearPlane", RTTRFuncType="setter")
        void setNearPlaneDistance(const Real &n) { mNear = n; mIsProjDirty = true; }

        /// 返回层剔除掩码
        TPROPERTY(RTTRFuncName="CullingMask", RTTRFuncType="getter")
        uint32_t getCullingMask() const { return mCullingMask; }

        /**
         * \brief 设置层剔除掩码
         * \param [in] mask : 位为 1 的层参与剔除判断，为 0 的层本相机不渲染
         */
        TPROPERTY(RTTRFuncName="CullingMask", RTTRFuncType="setter")
        void setCullingMask(uint32_t mask) { mCullingMask = mask; }
        
        /**
         * \brief 返回视图矩阵（世界到相机空间）
         * \return 脏时从 Transform3D 世界变换求逆并缓存
         */
        const Matrix4 &getViewMatrix() const;

        /**
         * \brief 返回投影矩阵
         * \return 脏时按投影类型与坐标系宏重新计算并缓存
         */
        const Matrix4 &getProjectionMatrix() const;
        
    protected:
        /// 默认构造
        Camera() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        Camera(const UUID &uuid);

        /**
         * \brief 注册到 Scene 并监听 Transform3D 变换变化
         */
        void onStart() override;

        /**
         * \brief 从 Scene 移除并解除 Transform3D 回调
         */
        void onDestroy() override;

        /**
         * \brief 从源 Camera 拷贝渲染参数
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK
         * \note cloneProperties 中 mCullingMask 当前从 mOrder 赋值，疑似实现笔误
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 为窗口渲染目标创建颜色与深度 RenderTexture 作为中间绘制目标
         * \param [in] window : 渲染窗口
         */
        void setupRenderTexture(RenderWindow *window);

        /**
         * \brief 更新渲染排序并在 Scene 中重新注册
         * \param [in] newOrder : 新的排序值
         */
        void updateOrder(uint32_t newOrder);

        /**
         * \brief 解除对源渲染目标的引用，只在本相机拥有它时才释放其附件
         */
        void releaseSrcRenderTarget();
        
    protected:
        /// 实际绘制用的源渲染目标（窗口相机时为中间 RenderTexture）
        RenderTargetPtr     mSrcRenderTarget {nullptr};
        /// 最终渲染目标
        RenderTargetPtr     mRenderTarget {nullptr};
        /// mSrcRenderTarget 是否由本相机创建。绑定纹理目标时它只是外部对象的
        /// 引用，附件的所有权在持有者那边，而且往往被多台相机共用，误释放会把
        /// 别人正在用的纹理一起卸载掉
        bool    mOwnsSrcRenderTarget {false};
        
        /// 渲染顺序
        uint32_t    mOrder {0};
        /// 层剔除掩码
        uint32_t    mCullingMask {0xFFFFFFFF};
        /// 相机视口
        Viewport    mViewport {};
        /// 背景清屏方式
        ClearFlags  mClearFlags {ClearFlags::kSkybox};
        /// 背景填充颜色
        ColorRGB    mClearColor {ColorRGB::BLACK};
        /// 深度缓冲填充值
        Real    mClearDepth {1.0f};
        /// 模板缓冲填充值
        uint8_t mClearStencil {0};
        
        /// 缓存的视图矩阵
        mutable Matrix4 mViewMatrix {}; 
        /// 缓存的投影矩阵
        mutable Matrix4 mProjectMatrix {};

        /// 视图矩阵是否需要重新计算
        mutable bool mIsViewDirty {true};
        /// 投影矩阵是否需要重新计算
        mutable bool mIsProjDirty {false};

        /// 投影类型
        Projection  mProjectionType {Projection::kPerspective};

        /// 垂直视场角（透视投影）
        Radian  mFovY {Math::PI / REAL_HALF};
        /// 宽高比（透视投影）
        Real    mAspectRatio {16.0f / 9.0f};
        /// 正交相机半高度
        Real    mOrthographicSize {1.0f};
        /// 远裁剪面距离
        Real    mFar {REAL_ZERO};
        /// 近裁剪面距离
        Real    mNear {REAL_ZERO};

        /// 宿主 Transform3D 指针（onStart 缓存）
        Transform3D *mXformNode {nullptr};
    };
}


#endif    /*__T3D_CAMERA_H__*/
