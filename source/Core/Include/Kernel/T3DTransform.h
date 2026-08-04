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

#ifndef __T3D_TRANSFORM_H__
#define __T3D_TRANSFORM_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief TRS 变换数据：平移、缩放、朝向，以及由其合成的仿射矩阵
     * \remarks setTranslation / setScaling / setOrientation 只改分量，不自动刷新矩阵；
     *          需调用 update() 或 applyTransform()（后者内部会 makeAffineMatrix）。
     */
    TSTRUCT()
    struct T3D_ENGINE_API Transform
    {
    public:
        /**
         * \brief 默认构造：平移为零、缩放为单位、朝向为单位四元数；仿射矩阵未按 TRS 重建
         */
        Transform();

        /**
         * \brief 用指定 TRS 构造，并立即合成仿射矩阵
         * \param [in] translation : 平移
         * \param [in] scaling : 缩放
         * \param [in] orientation : 朝向
         */
        Transform(const Vector3 &translation, const Vector3 &scaling, const Quaternion &orientation);

        /**
         * \brief 拷贝构造，复制 TRS 与仿射矩阵
         * \param [in] other : 源变换
         */
        Transform(const Transform &other);

        /**
         * \brief 赋值，复制 TRS 与仿射矩阵
         * \param [in] other : 源变换
         * \return 本对象引用
         */
        Transform &operator =(const Transform &other);

        /**
         * \brief 设置平移分量（不刷新仿射矩阵）
         * \param [in] translation : 平移
         */
        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &translation);

        /**
         * \brief 获取平移（常量）
         * \return 平移分量引用
         */
        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const;

        /// 获取可写平移引用（不刷新仿射矩阵）
        Vector3 &getTranslation();

        /**
         * \brief 设置缩放分量（不刷新仿射矩阵）
         * \param [in] scaling : 缩放
         */
        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling);

        /**
         * \brief 获取缩放（常量）
         * \return 缩放分量引用
         */
        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const;

        /// 获取可写缩放引用（不刷新仿射矩阵）
        Vector3 &getScaling();

        /**
         * \brief 设置朝向（不刷新仿射矩阵）
         * \param [in] orientation : 朝向四元数
         */
        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation);

        /**
         * \brief 获取朝向（常量）
         * \return 朝向四元数引用
         */
        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="getter")
        const Quaternion &getOrientation() const;

        /// 获取可写朝向引用（不刷新仿射矩阵）
        Quaternion &getOrientation();

        /**
         * \brief 将本对象设为 xform1 ∘ xform2（先局部 xform2，再父级 xform1），并刷新仿射矩阵
         * \param [in] xform1 : 父级 / 左侧变换
         * \param [in] xform2 : 子级 / 右侧变换
         */
        void applyTransform(const Transform &xform1, const Transform &xform2);

        /**
         * \brief 将本对象设为 xform ∘ (translation, orientation, scaling)，并刷新仿射矩阵
         * \param [in] xform : 父级变换
         * \param [in] translation : 局部平移
         * \param [in] orientation : 局部朝向
         * \param [in] scaling : 局部缩放
         * \remarks 朝向 = xform.orientation * orientation；
         *          缩放 = xform.scaling * scaling；
         *          平移 = xform.translation + xform.orientation * translation * xform.scaling。
         */
        void applyTransform(const Transform &xform, const Vector3 &translation, const Quaternion &orientation, const Vector3 &scaling);

        /**
         * \brief 获取当前缓存的仿射矩阵
         * \return 仿射矩阵常量引用（可能与 TRS 不同步，除非已 update / applyTransform）
         */
        const Matrix4 &getAffineMatrix() const;

        /// 根据当前 TRS 重新合成仿射矩阵（R * S，再 setTranslate）
        void update();

    private:
        /// 由 mOrientation / mScaling / mTranslation 写入 mAffineMatrix
        void makeAffineMatrix();

    private:
        Vector3     mTranslation {};    ///< 平移
        Vector3     mScaling {};        ///< 缩放
        Quaternion  mOrientation {};    ///< 朝向

        Matrix4     mAffineMatrix {};   ///< 由 TRS 合成的仿射矩阵
    };
}


#include "T3DTransform.inl"


#endif    /*__T3D_TRANSFORM_H__*/
