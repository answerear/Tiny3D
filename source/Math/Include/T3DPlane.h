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
    TCLASS()
    template <typename T>
    class TPlane
    {
        TRTTI_ENABLE()
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

    protected:
        TPROPERTY(RTTRFuncName="coeff", RTTRFuncType="setter", "Desciption"="Coefficient in plane euqation.")
        void setCoeff(TArray<T> coeff)
        {
            size_t i = 0;
            for (const auto &c : coeff)
            {
                mCoeff[i++] = c;
            }
        }

        TPROPERTY(RTTRFuncName="coeff", RTTRFuncType="getter", "Desciption"="Coefficient in plane euqation.")
        TArray<T> getCoeff() const
        {
            return TArray<T>(mCoeff, mCoeff+4);
        }
        
        T    mCoeff[4];
    };
}

#include "T3DPlane.inl"


#endif    /*__T3D_PLANE_H__*/
