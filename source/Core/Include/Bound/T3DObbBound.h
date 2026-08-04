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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 有向包围盒（OBB）碰撞体组件
     * \remarks 局部空间原始 OBB 经 Transform 每帧重算为世界空间 mObb，相交检测使用世界空间数据
     */
    TCLASS()
    class T3D_ENGINE_API ObbBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建带新生成 UUID 的 ObbBound 实例
         * \return 新建的 ObbBound 智能指针
         */
        static ObbBoundPtr create();

        ~ObbBound() override;

        /**
         * \brief 返回碰撞体类型
         * \return Type::OBB
         */
        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        /**
         * \brief 克隆本组件
         * \return 复制属性成功返回新 ObbBound；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /// 世界空间 OBB，由 update() 每帧从 mOriginalObb 与变换矩阵重算，不参与反射与序列化
        const Obb &getObb() const { return mObb; }

        /// 局部空间原始 OBB，作为编辑与序列化数据源
        const Obb& getOriginalObb() const { return mOriginalObb; }

        /**
         * \brief 返回局部空间 OBB 中心
         * \return 只读引用，数据来自 mOriginalObb
         * \remarks 拆分为 Center、三轴方向与 Extents 以匹配 Inspector 平铺展示，避免整体反射 Obb 产生可折叠子结构
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Box center in local space")
        const Vector3 &getCenter() const { return mOriginalObb.getCenter(); }

        /// 返回局部空间第一轴方向（单位向量）
        TPROPERTY(RTTRFuncName="XAxis", RTTRFuncType="getter", "Description"="First axis direction in local space")
        const Vector3 &getXAxis() const { return mOriginalObb.getAxis(0); }

        /// 返回局部空间第二轴方向（单位向量）
        TPROPERTY(RTTRFuncName="YAxis", RTTRFuncType="getter", "Description"="Second axis direction in local space")
        const Vector3 &getYAxis() const { return mOriginalObb.getAxis(1); }

        /// 返回局部空间第三轴方向（单位向量）
        TPROPERTY(RTTRFuncName="ZAxis", RTTRFuncType="getter", "Description"="Third axis direction in local space")
        const Vector3 &getZAxis() const { return mOriginalObb.getAxis(2); }

        /**
         * \brief 返回局部空间三轴半长
         * \return 由 mOriginalObb 各轴 extent 组成的 Vector3
         */
        TPROPERTY(RTTRFuncName="Extents", RTTRFuncType="getter", "Description"="Half extents along the three axes in local space")
        Vector3 getExtents() const
        {
            return Vector3(mOriginalObb.getExtent(0), mOriginalObb.getExtent(1),
                mOriginalObb.getExtent(2));
        }

        /**
         * \brief 设置局部空间 OBB 中心，同时写入 mObb 与 mOriginalObb
         * \param [in] center : 新的中心点
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Box center in local space")
        void setCenter(const Vector3 &center)
        {
            mObb.setCenter(center);
            mOriginalObb.setCenter(center);
        }

        /**
         * \brief 设置局部空间 OBB 三轴方向，同时写入 mObb 与 mOriginalObb
         * \param [in] axis0 : 第一轴方向
         * \param [in] axis1 : 第二轴方向
         * \param [in] axis2 : 第三轴方向
         */
        TFUNCTION()
        void setAxis(const Vector3 &axis0, const Vector3 &axis1, const Vector3 &axis2)
        {
            mObb.setAxis(axis0, axis1, axis2);
            mOriginalObb.setAxis(axis0, axis1, axis2);
        }

        /**
         * \brief 设置局部空间 OBB 三轴半长，同时写入 mObb 与 mOriginalObb
         * \param [in] extent0 : 第一轴半长
         * \param [in] extent1 : 第二轴半长
         * \param [in] extent2 : 第三轴半长
         */
        TFUNCTION()
        void setExtent(Real extent0, Real extent1, Real extent2)
        {
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);

            mOriginalObb.setExtent(0, extent0);
            mOriginalObb.setExtent(1, extent1);
            mOriginalObb.setExtent(2, extent2);
        }

        /**
         * \brief 获取关联 Renderable
         * \return 当前实现恒为 nullptr
         */
        Renderable *getRenderable() override;

    protected:
        ObbBound() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件实例 UUID
         */
        ObbBound(const UUID &uuid);

        /**
         * \brief 检测世界空间 OBB 与球体是否相交
         * \param [in] sphere : 世界空间球体
         * \return 使用 IntrSphereObb 判定，相交返回 true
         */
        bool testSphere(const Sphere &sphere) const override;

        /**
         * \brief 检测世界空间 OBB 与 AABB 是否相交
         * \param [in] aabb : 世界空间 AABB
         * \return 使用 IntrAabbObb 判定，相交返回 true
         */
        bool testAabb(const Aabb &aabb) const override;

        /**
         * \brief 检测两个世界空间 OBB 是否相交
         * \param [in] obb : 对方世界空间 OBB
         * \return 使用 IntrObbObb 判定，相交返回 true
         */
        bool testObb(const Obb &obb) const override;

        /**
         * \brief 检测世界空间 OBB 与胶囊体是否相交
         * \param [in] capsule : 世界空间胶囊体
         * \return 使用 IntrCapsuleObb 判定，相交返回 true
         */
        bool testCapsule(const Capsule &capsule) const override;

        /**
         * \brief 检测世界空间 OBB 与视锥体是否相交
         * \param [in] frustum : 世界空间视锥体
         * \return 使用 IntrFrustumObb 判定，相交返回 true
         */
        bool testFrustum(const Frustum &frustum) const override;

        /**
         * \brief 复制 ObbBound 特有属性
         * \param [in] src : 源 Component，须为 ObbBound
         * \return 基类成功后复制 mObb 与 mOriginalObb
         */
        TResult cloneProperties(const Component * const src) override;
        
        /**
         * \brief 根据局部到世界变换重算世界空间 mObb
         * \remarks 中心经仿射矩阵变换；各轴方向与半长由变换后的轴向量归一化并取长度得到
         */
        void update();

        /**
         * \brief 每帧更新回调，调用 update()
         */
        void onUpdate() override;

    private:
        /**
         * \brief 设置局部空间原始 OBB 并同步世界空间副本
         * \param [in] obb : 新的局部空间 OBB
         * \remarks 在下一帧 update() 运行前，先将 mObb 设为相同值，避免剔除逻辑读到未初始化的世界 OBB
         */
        void setOriginalObb(const Obb &obb)
        {
            mOriginalObb = obb;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界 OBB
            mObb = obb;
        }

        /**
         * \brief 单独设置第一轴方向，保留其余两轴
         * \param [in] axis : 新的第一轴方向
         * \remarks 须先拷贝 Y/Z 轴再调用 setAxis，不可将 getXAxis() 等引用直接传回 setAxis
         */
        TPROPERTY(RTTRFuncName="XAxis", RTTRFuncType="setter", "Description"="First axis direction in local space")
        void setXAxis(const Vector3 &axis)
        {
            const Vector3 y = getYAxis();
            const Vector3 z = getZAxis();
            setAxis(axis, y, z);
        }

        /**
         * \brief 单独设置第二轴方向，保留其余两轴
         * \param [in] axis : 新的第二轴方向
         */
        TPROPERTY(RTTRFuncName="YAxis", RTTRFuncType="setter", "Description"="Second axis direction in local space")
        void setYAxis(const Vector3 &axis)
        {
            const Vector3 x = getXAxis();
            const Vector3 z = getZAxis();
            setAxis(x, axis, z);
        }

        /**
         * \brief 单独设置第三轴方向，保留其余两轴
         * \param [in] axis : 新的第三轴方向
         */
        TPROPERTY(RTTRFuncName="ZAxis", RTTRFuncType="setter", "Description"="Third axis direction in local space")
        void setZAxis(const Vector3 &axis)
        {
            const Vector3 x = getXAxis();
            const Vector3 y = getYAxis();
            setAxis(x, y, axis);
        }

        /**
         * \brief 以 Vector3 同时设置三轴半长
         * \param [in] extents : 三轴半长
         * \remarks Extents getter 按值返回，setter 亦按值收参以满足 RTTR 类型一致要求
         */
        TPROPERTY(RTTRFuncName="Extents", RTTRFuncType="setter", "Description"="Half extents along the three axes in local space")
        void setExtents(Vector3 extents)
        {
            setExtent(extents.x(), extents.y(), extents.z());
        }
        
    private:
        /// 世界空间 OBB，由 update() 每帧重算
        Obb mObb {};
        /// 局部空间原始 OBB，作为编辑与序列化数据源
        Obb mOriginalObb {};
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
