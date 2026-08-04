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


#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 视锥体碰撞体组件，通常挂载于带 Camera 的 GameObject 用于视锥剔除
     * \remarks 每帧从同对象 Camera 的 projection × view 矩阵推导六个裁剪平面并重算 mFrustum
     */
    TCLASS()
    class T3D_ENGINE_API FrustumBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建带新生成 UUID 的 FrustumBound 实例
         * \return 新建的 FrustumBound 智能指针
         */
        static FrustumBoundPtr create();

        ~FrustumBound() override;

        /**
         * \brief 返回碰撞体类型
         * \return Type::FRUSTUM
         */
        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        /**
         * \brief 克隆本组件
         * \return 复制属性成功返回新 FrustumBound；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /// 世界空间视锥体，由 update() 每帧从 Camera 的 view-projection 矩阵重算，不参与反射与序列化
        const Frustum &getFrustum() const { return mFrustum; }

        /**
         * \brief 返回局部空间原始视锥体
         * \return 与 mFrustum 同步维护的原始副本
         * \remarks 六个裁剪面每帧会被 update() 覆盖，局部视锥体不参与反射与序列化
         */
        const Frustum& getOriginalFrustum() const { return mOriginalFrustum; }

        /**
         * \brief 设置指定面的裁剪平面，同时更新世界与原始视锥体
         * \param [in] face : 视锥体面索引
         * \param [in] plane : 该面的裁剪平面
         */
        TFUNCTION()
        void setFrustumFace(Face face, const Plane &plane);

        /**
         * \brief 批量设置视锥体裁剪平面，同时更新世界与原始视锥体
         * \param [in] plane : 平面数组首地址
         * \param [in] planeCount : 平面数量，须不大于 E_MAX_FACE
         * \remarks planeCount 超过 E_MAX_FACE 时触发断言
         */
        TFUNCTION()
        void setFrustumFaces(Plane *plane, size_t planeCount);

        /**
         * \brief 获取关联 Renderable
         * \return 当前实现恒为 nullptr
         */
        Renderable *getRenderable() override;

    protected:
        FrustumBound() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件实例 UUID
         */
        FrustumBound(const UUID &uuid);

        /**
         * \brief 检测世界空间视锥体与球体是否相交
         * \param [in] sphere : 世界空间球体
         * \return 使用 IntrFrustumSphere 判定，相交返回 true
         */
        bool testSphere(const Sphere &sphere) const override;

        /**
         * \brief 检测世界空间视锥体与 AABB 是否相交
         * \param [in] aabb : 世界空间 AABB
         * \return 使用 IntrFrustumAabb 判定，相交返回 true
         */
        bool testAabb(const Aabb &aabb) const override;

        /**
         * \brief 检测世界空间视锥体与 OBB 是否相交
         * \param [in] obb : 世界空间 OBB
         * \return 使用 IntrFrustumObb 判定，相交返回 true
         */
        bool testObb(const Obb &obb) const override;

        /**
         * \brief 检测世界空间视锥体与胶囊体是否相交
         * \param [in] capsule : 世界空间胶囊体
         * \return 使用 IntrFrustumCapsule 判定，相交返回 true
         */
        bool testCapsule(const Capsule &capsule) const override;

        /**
         * \brief 检测两个视锥体是否相交
         * \param [in] frustum : 对方视锥体
         * \return 当前未实现视锥-视锥检测，恒返回 false
         */
        bool testFrustum(const Frustum &frustum) const override;

        /**
         * \brief 复制 FrustumBound 特有属性
         * \param [in] src : 源 Component，须为 FrustumBound
         * \return 基类成功后复制 mFrustum 与 mOriginalFrustum
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 从同 GameObject 上 Camera 的 projection × view 矩阵推导六个裁剪平面并重设视锥体
         * \remarks 采用齐次裁剪空间平面组合公式，近远平面按 OpenGL 约定（row3 ± row2）
         */
        void update();

        /**
         * \brief 启动时缓存所属 GameObject 的 Transform3D 节点
         */
        void onStart() override;

        /**
         * \brief 每帧更新回调，先调用基类再执行 update()
         */
        void onUpdate() override;

        /**
         * \brief 销毁时释放 Transform3D 缓存并调用基类 onDestroy()
         */
        void onDestroy() override;

    private:
        /**
         * \brief 设置局部空间原始视锥体并同步世界空间副本
         * \param [in] frustum : 新的视锥体
         * \remarks 在下一帧 update() 运行前，先将 mFrustum 设为相同值，避免剔除逻辑读到未初始化的世界视锥体
         */
        void setOriginalFrustum(const Frustum &frustum)
        {
            mOriginalFrustum = frustum;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界视锥体
            mFrustum = frustum;
        }

    private:
        /// 世界空间视锥体，由 update() 每帧重算
        Frustum mFrustum {};
        /// 局部空间原始视锥体，与 mFrustum 同步写入
        Frustum mOriginalFrustum {};

        /// 所属 GameObject 的 Transform3D 节点缓存，onStart 赋值、onDestroy 清空
        Transform3DPtr  mXformNode {nullptr};
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
