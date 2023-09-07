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

#ifndef __T3D_INTR_AABB_PLANE_H__
#define __T3D_INTR_AABB_PLANE_H__


#include "T3DMathPrerequisites.h"
#include "T3DAabb.h"
#include "T3DPlane.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrAabbPlane
    {
        TIntrAabbPlane();
        TIntrAabbPlane(const TAabb<T> *box, const TPlane<T> *plane);
        TIntrAabbPlane(const TAabb<T> &box, const TPlane<T> &plane);

        TFUNCTION()
        int32_t test();

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="getter")
        const TAabb<T> *getBox() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="plane", RTTRFuncType="getter")
        const TAabb<T> *getPlane() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="setter")
        void setBox(const TAabb<T> *box)
        {
            mBox = box;
        }

        TPROPERTY(RTTRFuncName="plane", RTTRFuncType="setter")
        void setPlane(const TPlane<T> *plane)
        {
            mPlane = plane;
        }

    private:
        const TAabb<T>  *mBox;
        const TPlane<T> *mPlane;
    };
}


#include "T3DIntrAabbPlane.inl"


#endif  /*__T3D_INTR_AABB_PLANE_H__*/
