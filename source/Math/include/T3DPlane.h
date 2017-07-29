/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


namespace Tiny3D
{
    class T3D_MATH_API Plane
    {
    public:
        enum Side
        {
            E_SIDE_NONE,
            E_SIDE_POSITIVE,
            E_SIDE_NEGATIVE,
        };

        Plane();
        Plane(const Vector3 &rkNormal, Real fDistance);
        Plane(const Vector3 &rkNormal, const Vector3 &rkPos);
        Plane(const Vector3 &rkPos1, const Vector3 &rkPos2, const Vector3 &rkPos3);
        Plane(const Plane &rkOther);

        Plane &operator =(const Plane &rkOther);

        bool operator ==(const Plane &rkOther) const;
        bool operator !=(const Plane &rkOther) const;

        const Real operator [](int32_t idx) const;
        Real &operator [](int32_t idx);

        Real distanceToPoint(const Vector3 &rkPos) const;

        Side sideForPoint(const Vector3 &rkPos) const;

        Vector3 getNormal() const;
        Real getDistance() const;

        void normalize();

    protected:
        Real    mCoeff[4];
    };
}


#include "T3DPlane.inl"


#endif  /*__T3D_PLANE_H__*/
