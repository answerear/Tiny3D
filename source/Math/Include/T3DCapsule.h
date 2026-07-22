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

#ifndef __T3D_CAPSULE_H__
#define __T3D_CAPSULE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"
#include "T3DReal.h"


namespace Tiny3D
{
    /// 胶囊体（线段两端点 + 半径）
    TSTRUCT()
    template <typename T>
    struct TCapsule
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TCapsule();
        /// 通过两端点与半径构造胶囊体
        TCapsule(const TVector3<T> &p0, const TVector3<T> &p1, const T &radius);
        /// 通过中心、圆柱半高（不含半球）、半径与单位轴向构造胶囊体
        TCapsule(const TVector3<T> &center, const T &cylinderHalfHeight,
            const T &radius, const TVector3<T> &axis);

        /// 拷贝构造函数
        TCapsule(const TCapsule &other);

        /// 重载赋值运算符
        TCapsule &operator =(const TCapsule &other);

        /**
         * \brief 通过多个点拟合胶囊体（PCA 主轴 + 半径 + 端点闭式解）
         * \param [in] points : 点数组
         * \param [in] count : 点数量
         * \remarks 结果保证包含所有点；点集近似球状时退化为 p0==p1 的球。
         */
        void build(const TVector3<T> points[], size_t count);

        /// 获取端点 0
        TPROPERTY(RTTRFuncName="point0", RTTRFuncType="getter")
        const TVector3<T> &getPoint0() const;
        TVector3<T> &getPoint0();

        /// 获取端点 1
        TPROPERTY(RTTRFuncName="point1", RTTRFuncType="getter")
        const TVector3<T> &getPoint1() const;
        TVector3<T> &getPoint1();

        /// 获取半径
        TPROPERTY(RTTRFuncName="radius", RTTRFuncType="getter")
        const T &getRadius() const;
        T &getRadius();

        /// 设置端点 0
        TPROPERTY(RTTRFuncName="point0", RTTRFuncType="setter")
        void setPoint0(const TVector3<T> &p0);

        /// 设置端点 1
        TPROPERTY(RTTRFuncName="point1", RTTRFuncType="setter")
        void setPoint1(const TVector3<T> &p1);

        /// 设置半径
        TPROPERTY(RTTRFuncName="radius", RTTRFuncType="setter")
        void setRadius(const T &radius);

        /// 获取胶囊体中心 (p0 + p1) / 2
        TFUNCTION()
        TVector3<T> getCenter() const;

        /// 获取轴向 normalize(p1 - p0)，退化时返回 UNIT_Y
        TFUNCTION()
        TVector3<T> getAxis() const;

        /// 获取线段长度 |p1 - p0|
        TFUNCTION()
        T getSegmentLength() const;

        /// 获取总高度（含两端半球）segmentLength + 2 * radius
        TFUNCTION()
        T getHeight() const;

        /// 检测指定点是否在胶囊体内部
        TFUNCTION()
        bool contains(const TVector3<T> &point) const;

        String getDebugString() const;

    private:
        TVector3<T> mPoint0;    /// 端点 0（半球中心）
        TVector3<T> mPoint1;    /// 端点 1（半球中心）
        T           mRadius;    /// 半径
    };
}

#include "T3DCapsule.inl"

#endif    /*__T3D_CAPSULE_H__*/