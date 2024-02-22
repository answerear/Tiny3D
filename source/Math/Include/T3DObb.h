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

#ifndef __T3D_OBB_H__
#define __T3D_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    TENUM()
    enum class ObbBuildOption : uint32_t
    {
        kByAABB = 0,       /**< 通过AABB直接构造 */
        kByCovariance = 1, /**< 利用协方差矩阵构造 */
    };
    
    /**
     * @brief 有向包围盒类（Oriented Bounding Box）
     */
    TSTRUCT()
    template <typename T>
    struct TObb
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TObb();
        TObb(const TVector3<T> &rkCenter, const TVector3<T> *akAxis,
            const T *afExtent);
        TObb(const TVector3<T> &rkCenter, const TVector3<T> &rkAxis0,
            const TVector3<T> &rkAxis1, const TVector3<T> &rkAxis2,
            T fExtent0, T fExtent1, T fExtent2);

        TFUNCTION()
        void build(const TVector3<T> points[], size_t count, 
            ObbBuildOption option = ObbBuildOption::kByAABB);

        TFUNCTION()
        void computeVertices(TVector3<T> *akVertex) const;

        TPROPERTY(RTTRFuncName="center", RTTRFuncType="getter")
        const TVector3<T> &getCenter() const;

        TFUNCTION()
        const TVector3<T> &getAxis(int32_t idx) const;

        const TVector3<T> *getAxis() const;

        TFUNCTION()
        T getExtent(int32_t idx) const;

        const T *getExtent() const;

        TPROPERTY(RTTRFuncName="center", RTTRFuncType="setter")
        void setCenter(const TVector3<T> &rkCenter);

        TFUNCTION()
        void setAxis(const TVector3<T> &rkAxis0, const TVector3<T> &rkAxis1,
            const TVector3<T> &rkAxis2);

        TFUNCTION()
        void setExtent(int32_t idx, T fExtent);

    protected:
        void buildByAABB(const TVector3<T> points[], size_t count);

        void buildByCovariance(const TVector3<T> points[], size_t count);

        TPROPERTY(RTTRFuncName="axis", RTTRFuncType="getter")
        TArray<TVector3<T>> getAxes() const;
        
        TPROPERTY(RTTRFuncName="axis", RTTRFuncTYpe="setter")
        void setAxes(TArray<TVector3<T>> axis);

        TPROPERTY(RTTRFuncName="extents", RTTRFuncType="getter")
        TArray<T> getExtents() const;

        TPROPERTY(RTTRFuncName="extents", RTTRFuncType="setter")
        void setExtents(TArray<T> extents);
        
    private:
        TVector3<T> mCenter;
        TVector3<T> mAxis[3];
        T           mExtent[3];
    };
}

#include "T3DObb.inl"

#endif    /*__T3D_OBB_H__*/
