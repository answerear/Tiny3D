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

#ifndef __T3D_SPHERE_H__
#define __T3D_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"
#include "T3DMatrix3.h"
#include "T3DReal.h"


namespace Tiny3D
{
    TENUM()
    enum class SphereBuildOption : uint32_t
    {
        kByWelzl = 0,   /// Welzl最小包围球算法
        kByRitter,      /// Ritter包围球算法
        kByAverage,     /// 均值法
    };

    /// 球类
    TSTRUCT()
    template <typename T>
    struct TSphere
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TSphere();
        /// 通过空间一个点构造包围球
        TSphere(const TVector3<T> &p0);
        /// 通过空间两点构造包围球
        TSphere(const TVector3<T> &p0, const TVector3<T> &p1);
        /// 通过空间三点构造包围球
//         TSphere(const TVector3<T> &p0, const TVector3<T> &p1,
//             const TVector3<T> &p2);
//         /// 通过空间四点构造包围球
//         TSphere(const TVector3<T> &p0, const TVector3<T> &p1,
//             const TVector3<T> &p2, const TVector3<T> &p3);
        /// 初始化原点和半径的构造函数
        TSphere(const TVector3<T> &center, const T &radius);

        /// 拷贝构造函数
        TSphere(const TSphere &sphere);

        /// 重载赋值运算符
        TSphere &operator =(const TSphere &sphere);

        /// 通过多个点构造包围球
        TFUNCTION()
        void build(const TVector3<T> points[], size_t count, 
            SphereBuildOption option = SphereBuildOption::kByRitter);

        /// 通过空间三点构造包围球
        TFUNCTION()
        bool build(const TVector3<T> &p0, const TVector3<T> &p1,
            const TVector3<T> &p2);

        /// 通过空间四点构造包围球
        TFUNCTION()
        bool build(const TVector3<T> &p0, const TVector3<T> &p1,
            const TVector3<T> &p2, const TVector3<T> &p3);
        
        /// 获取球心
        TPROPERTY(RTTRFuncName="center", RTTRFuncType="getter")
        const TVector3<T> &getCenter() const;
        TVector3<T> &getCenter();

        /// 获取半径
        TPROPERTY(RTTRFuncName="radius", RTTRFuncType="getter")
        const T &getRadius() const;
        T &getRadius();

        /// 设置球心
        TPROPERTY(RTTRFuncName="center", RTTRFuncType="setter")
        void setCenter(const TVector3<T> &center);

        /// 设置半径
        TPROPERTY(RTTRFuncName="radius", RTTRFuncType="setter")
        void setRadius(const T &radius);

        /// 检测指定点是否在球内部
        TFUNCTION()
        bool contains(const TVector3<T> &point) const;

        String getDebugString() const;

    protected:
        /// Welzl最小包围球算法生成包围球
        void buildByWelzl(const TVector3<T> points[], size_t count);

        TSphere recurseMinSphere(TVector3<T> *points[], size_t count, size_t b = 0);

        /// Ritter逼近修正算法生成包围球
        void buildByRitter(const TVector3<T> points[], size_t count);

        /// 简单粗暴的均值法生成包围球
        void buildByAverage(const TVector3<T> points[], size_t count);

    private:
        TVector3<T> mCenter;    /// 球心
        T           mRadius;    /// 半径
    };
}

#include "T3DSphere.inl"

#endif    /*__T3D_SPHERE_H__*/
