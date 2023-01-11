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

#ifndef __T3D_AABB_H__
#define __T3D_AABB_H__


#include "T3DMathPrerequisites.h"
#include "T3DSphere.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    /// 轴对齐包围盒类（Axis Aligned Bounding Box）
    TCLASS("Description"="Class represents an axis aligned bonding box.")
    template <typename T>
    class TAabb
    {
        TRTTI_ENABLE()
        TRTTI_FRIEND
        
    public:
        /// 默认构造函数
        TAabb();
        TAabb(T fXMin, T fXMax, T fYMin, T fYMax, T fZMin, T fZMax);

        void build(const TVector3<T> points[], size_t count);

        TFUNCTION("Description"="The width of the aligned axis box.")
        T getWidth() const;
        
        TFUNCTION("Description"="The height of the aligned axis box.")
        T getHeight() const;
        
        TFUNCTION("Description"="The depth of the aligned axis box.")
        T getDepth() const;

        TPROPERTY(RTTRFuncName="minX", RTTRFuncType="getter", "Description"="The minimum in x-axis.")
        T getMinX() const;
        
        TPROPERTY(RTTRFuncName="maxX", RTTRFuncType="getter", "Description"="The maximum in x-axis.")
        T getMaxX() const;

        TPROPERTY(RTTRFuncName="minY", RTTRFuncType="getter", "Description"="The minimum in y-axis.")
        T getMinY() const;
        
        TPROPERTY(RTTRFuncName="maxY", RTTRFuncType="getter", "Description"="The maximum in y-axis.")
        T getMaxY() const;

        TPROPERTY(RTTRFuncName="minZ", RTTRFuncType="getter", "Description"="The minimum in z-axis.")
        T getMinZ() const;
        
        TPROPERTY(RTTRFuncName="maxZ", RTTRFuncType="getter", "Description"="The maximum in z-axis.")
        T getMaxZ() const;

        TFUNCTION("Description"="The center in the aligned axis box.")
        const TVector3<T> &getCenter() const;

        TFUNCTION("Description"="The radius of the sphere surrounding the aligned axis box.")
        T getRadius() const;

        TFUNCTION("Description"="The sphere surrounding the aligned axis box.")
        const TSphere<T> &getSphere() const;

        TFUNCTION("Description"="The width of the aligned axis box.")
        void setWidth(T fWidth);

        TFUNCTION("Description"="The height of the aligned axis box.")
        void setHeight(T fHeight);

        TFUNCTION("Description"="The depth of the aligned axis box.")
        void setDepth(T fDepth);

        TPROPERTY(RTTRFuncName="minX", RTTRFuncType="setter", "Description"="The minimum in x-axis.")
        void setMinX(T x);

        TPROPERTY(RTTRFuncName="maxX", RTTRFuncType="setter", "Description"="The maximum in x-axis.")
        void setMaxX(T x);

        TPROPERTY(RTTRFuncName="minY", RTTRFuncType="setter", "Description"="The minimum in y-axis.")
        void setMinY(T y);

        TPROPERTY(RTTRFuncName="maxY", RTTRFuncType="setter", "Description"="The maximum in y-axis.")
        void setMaxY(T y);

        TPROPERTY(RTTRFuncName="minZ", RTTRFuncType="setter", "Description"="The minimum in z-axis.")
        void setMinZ(T z);

        TPROPERTY(RTTRFuncName="maxZ", RTTRFuncType="setter", "Description"="The maximum in z-axis.")
        void setMaxZ(T z);

        TFUNCTION()
        void setParam(const TVector3<T> &vMin, const TVector3<T> &vMax);

    private:
        T   mMinX;
        T   mMaxX;

        T   mMinY;
        T   mMaxY;

        T   mMinZ;
        T   mMaxZ;

        TSphere<T>  mSphere;
    };
}

#include "T3DAabb.inl"

#endif    /*__T3D_AABB_H__*/
