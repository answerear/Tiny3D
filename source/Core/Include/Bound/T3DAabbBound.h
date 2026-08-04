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


#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 轴对齐包围盒（AABB）碰撞体组件
     * \remarks 局部空间原始 AABB 经 Transform 每帧重算为世界空间 mAabb，相交检测使用世界空间数据
     */
    TCLASS()
    class T3D_ENGINE_API AabbBound : public Bound
    {
         TRTTI_ENABLE(Bound)
         TRTTI_FRIEND

    public:
        /**
         * \brief 创建带新生成 UUID 的 AabbBound 实例
         * \return 新建的 AabbBound 智能指针
         */
        static AabbBoundPtr create();

        ~AabbBound() override;

        /**
         * \brief 返回碰撞体类型
         * \return Type::AABB
         */
        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        /**
         * \brief 克隆本组件
         * \return 复制属性成功返回新 AabbBound；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /// 世界空间 AABB，由 update() 每帧从 mOriginalAabb 与变换矩阵重算，不参与反射与序列化
        const Aabb &getAabb() const { return mAabb; }

        /// 局部空间原始 AABB，作为编辑器与序列化数据源
        const Aabb& getOriginalAabb() const { return mOriginalAabb; }

        /**
         * \brief 返回局部空间包围盒中心
         * \return 只读引用，数据来自 mOriginalAabb
         * \remarks 拆分为 Center/Size 两个 Vector3 以匹配 Inspector 平铺展示，避免整体反射 Aabb 产生可折叠子结构
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Box center in local space")
        const Vector3 &getCenter() const { return mOriginalAabb.getCenter(); }

        /**
         * \brief 返回局部空间包围盒三轴尺寸（宽、高、深）
         * \return 由 mOriginalAabb 的 min/max 差值组成的 Vector3
         */
        TPROPERTY(RTTRFuncName="Size", RTTRFuncType="getter", "Description"="Box size along the three axes in local space")
        Vector3 getSize() const
        {
            return Vector3(mOriginalAabb.getWidth(), mOriginalAabb.getHeight(),
                mOriginalAabb.getDepth());
        }

        /**
         * \brief 以各轴最小/最大值设置局部空间 AABB
         * \param [in] minX : X 轴最小值
         * \param [in] maxX : X 轴最大值
         * \param [in] minY : Y 轴最小值
         * \param [in] maxY : Y 轴最大值
         * \param [in] minZ : Z 轴最小值
         * \param [in] maxZ : Z 轴最大值
         * \remarks 内部构造 Aabb 后调用 setOriginalAabb，同步更新世界空间副本
         */
        TFUNCTION()
        void setParams(Real minX, Real maxX, Real minY, Real maxY, Real minZ, Real maxZ)
        {
            Aabb aabb;
            aabb.setParam(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
            setOriginalAabb(aabb);
        }

        /**
         * \brief 获取关联 Renderable
         * \return 当前实现恒为 nullptr
         */
        Renderable *getRenderable() override;
        
    protected:
        AabbBound() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件实例 UUID
         */
        AabbBound(const UUID &uuid);

        /**
         * \brief 检测世界空间 AABB 与球体是否相交
         * \param [in] sphere : 世界空间球体
         * \return 使用 IntrSphereAabb 判定，相交返回 true
         */
        bool testSphere(const Sphere &sphere) const override;

        /**
         * \brief 检测两个世界空间 AABB 是否相交
         * \param [in] aabb : 对方世界空间 AABB
         * \return 使用 IntrAabbAabb 判定，相交返回 true
         */
        bool testAabb(const Aabb &aabb) const override;

        /**
         * \brief 检测世界空间 AABB 与 OBB 是否相交
         * \param [in] obb : 世界空间 OBB
         * \return 使用 IntrAabbObb 判定，相交返回 true
         */
        bool testObb(const Obb &obb) const override;

        /**
         * \brief 检测世界空间 AABB 与胶囊体是否相交
         * \param [in] capsule : 世界空间胶囊体
         * \return 使用 IntrCapsuleAabb 判定，相交返回 true
         */
        bool testCapsule(const Capsule &capsule) const override;

        /**
         * \brief 检测世界空间 AABB 与视锥体是否相交
         * \param [in] frustum : 世界空间视锥体
         * \return 使用 IntrFrustumAabb 判定，相交返回 true
         */
        bool testFrustum(const Frustum &frustum) const override;

        /**
         * \brief 复制 AabbBound 特有属性
         * \param [in] src : 源 Component，须为 AabbBound
         * \return 基类成功后复制 mAabb 与 mOriginalAabb
         */
        TResult cloneProperties(const Component * const src) override;
        
        /**
         * \brief 根据所属 GameObject 的局部到世界变换，重算世界空间 mAabb
         * \remarks 采用矩阵符号快速法变换 AABB 八个顶点对应的 min/max，避免逐顶点变换
         */
        void update();

        /**
         * \brief 每帧更新回调，调用 update()
         */
        void onUpdate() override;

    private:
        /**
         * \brief 设置局部空间原始 AABB 并同步世界空间副本
         * \param [in] aabb : 新的局部空间 AABB
         * \remarks 在下一帧 update() 运行前，先将 mAabb 设为相同值，避免剔除逻辑读到未初始化的世界包围盒
         */
        void setOriginalAabb(const Aabb &aabb)
        {
            mOriginalAabb = aabb;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界包围盒
            mAabb = aabb;
        }

        /**
         * \brief 设置局部空间包围盒中心并保持当前尺寸
         * \param [in] center : 新的中心点
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Box center in local space")
        void setCenter(const Vector3 &center)
        {
            resetOriginalAabb(center, getSize());
        }

        /**
         * \brief 设置局部空间包围盒三轴尺寸并保持当前中心
         * \param [in] size : 宽、高、深尺寸
         * \remarks Size getter 按值返回，setter 亦按值收参以满足 RTTR 类型一致要求
         */
        TPROPERTY(RTTRFuncName="Size", RTTRFuncType="setter", "Description"="Box size along the three axes in local space")
        void setSize(Vector3 size)
        {
            resetOriginalAabb(getCenter(), size);
        }

        /**
         * \brief 由中心与尺寸重建局部空间 AABB
         * \param [in] center : 包围盒中心
         * \param [in] size : 三轴全长（非半长）
         */
        void resetOriginalAabb(const Vector3 &center, const Vector3 &size)
        {
            const Vector3 half = size * REAL_HALF;
            Aabb aabb;
            aabb.setParam(center - half, center + half);
            setOriginalAabb(aabb);
        }

    private:
        /// 世界空间 AABB，由 update() 每帧重算，不参与序列化
        Aabb    mAabb {};
        /// 局部空间原始 AABB，作为编辑与序列化数据源
        Aabb    mOriginalAabb {};
    };
}

#endif  /*__T3D_AABB_BOUND_H__*/
