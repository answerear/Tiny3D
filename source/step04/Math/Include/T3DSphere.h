/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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

#ifndef __T3D_SPHERE_H__
#define __T3D_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    class T3D_MATH_API Sphere
    {
    public:
        Sphere();
        Sphere(const Vector3 &rkCenter, Real fRadius);
        Sphere(const Sphere &rkOther);

        Sphere &operator =(const Sphere &rkOther);

        const Vector3 &getCenter() const;
        Vector3 &getCenter();

        Real getRadius() const;
        Real &getRadius();

        void setCenter(const Vector3 &rkCenter);
        void setRadius(Real fRadius);

        bool contains(const Vector3 &rkPoint) const;

    private:
        Vector3 mCenter;
        Real    mRadius;
    };
}


#include "T3DSphere.inl"


#endif  /*__T3D_SPHERE_H__*/
