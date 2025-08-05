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

#ifndef __T3D_PLANE_H__
#define __T3D_PLANE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"
#include "T3DReal.h"


namespace Tiny3D
{
    TENUM()
    enum class Side : uint32_t
    {
        kIntersect = 0,
        kPositive,
        kNegative,
    };
    
    /**
     * @brief 平面类
     */
    TSTRUCT()
    template <typename T>
    struct TPlane
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TPlane();
        TPlane(const TVector3<T> &rkNormal, T fDistance);
        TPlane(const TVector3<T> &rkNormal, const TVector3<T> &rkPos);
        TPlane(const TVector3<T> &rkPos1, const TVector3<T> &rkPos2, const TVector3<T> &rkPos3);
        TPlane(const TPlane<T> &rkOther);

        TPlane<T> &operator =(const TPlane<T> &rkOther);

        bool operator ==(const TPlane<T> &rkOther) const;
        bool operator !=(const TPlane<T> &rkOther) const;

        const T operator [](int32_t idx) const;
        T &operator [](int32_t idx);

        TFUNCTION()
        T distanceToPoint(const TVector3<T> &rkPos) const;

        // 快速求点到平面距离，因为这里假设平面法线已经是单位向量
        TFUNCTION()
        T fastDistanceToPoint(const TVector3<T> &rkPos) const;

        TFUNCTION()
        Side sideForPoint(const TVector3<T> &rkPos) const;

        // 快速求点在平面的正面和反面
        TFUNCTION()
        Side fastSideForPoint(const TVector3<T> &rkPos) const;

        TFUNCTION()
        TVector3<T> getNormal() const;

        TFUNCTION()
        T getDistance() const;

        TFUNCTION()
        void normalize();

        String getDebugString() const;
        
    protected:
        TPROPERTY(RTTRFuncName="A", RTTRFuncType="setter", "Description"="Coefficient A in plane euqation.")
        void setACoeff(const T &coeff) { mCoeff[0] = coeff; }

        TPROPERTY(RTTRFuncName="A", RTTRFuncType="getter", "Description"="Coefficient A in plane euqation.")
        const T &getACoeff() const { return mCoeff[0]; }

        TPROPERTY(RTTRFuncName="B", RTTRFuncType="setter", "Description"="Coefficient B in plane euqation.")
        void setBCoeff(const T &coeff) { mCoeff[1] = coeff; }

        TPROPERTY(RTTRFuncName="B", RTTRFuncType="getter", "Description"="Coefficient B in plane euqation.")
        const T &getBCoeff() const { return mCoeff[1]; }

        TPROPERTY(RTTRFuncName="C", RTTRFuncType="setter", "Description"="Coefficient C in plane euqation.")
        void setCCoeff(const T &coeff) { mCoeff[2] = coeff; }

        TPROPERTY(RTTRFuncName="C", RTTRFuncType="getter", "Description"="Coefficient C in plane euqation.")
        const T &getCCoeff() const { return mCoeff[2]; }

        TPROPERTY(RTTRFuncName="D", RTTRFuncType="setter", "Description"="Coefficient D in plane euqation.")
        void setDCoeff(const T &coeff) { mCoeff[3] = coeff; }

        TPROPERTY(RTTRFuncName="D", RTTRFuncType="getter", "Description"="Coefficient D in plane euqation.")
        const T &getDCoeff() const { return mCoeff[3]; }
        
        T    mCoeff[4];
    };
}

#include "T3DPlane.inl"


#endif    /*__T3D_PLANE_H__*/
