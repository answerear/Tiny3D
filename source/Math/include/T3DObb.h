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

#ifndef __T3D_OBB_H__
#define __T3D_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    class T3D_MATH_API Obb
    {
    public:
        Obb();
        Obb(const Vector3 &rkCenter, const Vector3 *akAxis, 
            const Real *afExtent);
        Obb(const Vector3 &rkCenter, const Vector3 &rkAxis0, 
            const Vector3 &rkAxis1, const Vector3 &rkAxis2,
            Real fExtent0, Real fExtent1, Real fExtent2);

        void computeVertices(Vector3 *akVertex) const;

        const Vector3 &getCenter() const;

        const Vector3 &getAxis(int32_t idx) const;

        const Vector3 *getAxis() const;

        Real getExtent(int32_t idx) const;

        const Real *getExtent() const;

        void setCenter(const Vector3 &rkCenter);

        void setAxis(const Vector3 &rkAxis0, const Vector3 &rkAxis1,
            const Vector3 &rkAxis2);

        void setExtent(int32_t idx, Real fExtent);

    private:
        Vector3 mCenter;
        Vector3 mAxis[3];
        Real    mExtent[3];
    };
}


#include "T3DObb.inl"


#endif  /*__T3D_OBB_H__*/
