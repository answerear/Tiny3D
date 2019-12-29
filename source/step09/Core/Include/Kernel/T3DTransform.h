/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#ifndef __T3D_TRANSFORM_H__
#define __T3D_TRANSFORM_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 变换类
     */
    class T3D_ENGINE_API Transform
    {
    public:
        /**
         * @brief 默认构造函数
         */
        Transform();

        /**
         * @brief 构造函数
         * @param [in] translation : 平移
         * @param [in] scaling : 缩放
         * @param [in] orientation : 朝向
         */
        Transform(const Vector3 &translation, const Vector3 &scaling,
            const Quaternion &orientation);

        /**
         * @brief 拷贝构造函数
         */
        Transform(const Transform &other);

        /**
         * @brief 重载赋值运算符
         */
        Transform &operator =(const Transform &other);

        /**
         * @brief 设置平移量
         * @param [in] translate : 平移量
         */
        void setTranslation(const Vector3 &translation);

        /**
         * @brief 获取平移量
         */
        const Vector3 &getTranslation() const;

        /**
         * @brief 获取平移量
         */
        Vector3 &getTranslation();

        /**
         * @brief 设置缩放量
         * @param [in] scale : 缩放量
         */
        void setScaling(const Vector3 &scaling);

        /**
         * @brief 获取缩放量
         */
        const Vector3 &getScaling() const;

        /**
         * @brief 获取缩放量
         */
        Vector3 &getScaling();

        /**
         * @brief 设置朝向量
         */
        void setOrientation(const Quaternion &orientation);

        /**
         * @brief 获取朝向量
         */
        const Quaternion &getOrientation() const;

        /**
         * @brief 获取朝向量
         */
        Quaternion &getOrientation();

        /**
         * @brief 把一个变换应用到另外一个变换上
         * @param [in] xform1 : 源变换
         * @param [in] xform2 : 应用到源变换上的变换
         */
        void applyTransform(const Transform &xform1, const Transform &xform2);

        /**
         * @brief 把一个RTS变换应用到另外一个变换上
         * @param [in] xform : 源变换
         * @param [in] translation : 应用到源变换上的平移量
         * @param [in] orientation : 应用到源变换上的朝向量
         * @param [in] scaling : 应用到源变换上的缩放量
         */
        void applyTransform(const Transform &xform, const Vector3 &translation,
            const Quaternion &orientation, const Vector3 &scaling);

        /**
         * @brief 获取仿射变换矩阵
         */
        const Matrix4 &getAffineMatrix() const;

        /**
         * @brief 更新变换
         */
        void update();

    private:
        /**
         * @brief 构造仿射变换
         */
        void makeAffineMatrix();

        Vector3     mTranslation;       /**< 平移量 */
        Vector3     mScaling;           /**< 缩放量 */
        Quaternion  mOrientation;       /**< 朝向量 */

        Matrix4     mAffineMatrix;      /**< 根据TRS构造的仿射变换 */
    };
}


#include "T3DTransform.inl"


#endif  /*__T3D_TRANSFORM_H__*/
