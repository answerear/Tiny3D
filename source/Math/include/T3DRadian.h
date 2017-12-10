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

#ifndef __T3D_RADIAN_H__
#define __T3D_RADIAN_H__

#include "T3DMathPrerequisites.h"

namespace Tiny3D
{
    class T3D_MATH_API Radian
    {
    public:
        /// Create a radian object with float value.
        explicit Radian(Real fRadian = 0.0);
        /// Create a degree object with float value.
        Radian(const Degree &degree);
        /// Copy constructor.
        Radian(const Radian &other);

        /// Assignment
        Radian &operator =(Real fRadian);
        Radian &operator =(const Radian &other);
        Radian &operator =(const Degree &degree);

        const Radian &operator +() const;
        Radian operator -() const;

        /// Comparison (equal to).
        bool operator ==(const Radian &other) const;
        /// Comparison (not equal to).
        bool operator !=(const Radian &other) const;
        ///
        bool operator <(const Radian &other) const;
        bool operator <=(const Radian &other) const;
        bool operator >(const Radian &other) const;
        bool operator >=(const Radian &other) const;

        Radian operator +(const Radian &other) const;
        Radian operator -(const Radian &other) const;

        Radian operator +(const Degree &degree) const;
        Radian operator -(const Degree &degree) const;

        Radian &operator +=(const Radian &other);
        Radian &operator -=(const Radian &other);

        Radian &operator +=(const Degree &degree);
        Radian &operator -=(const Degree &degree);

        Radian operator *(Real fScalar) const;
        Radian operator /(Real fScalar) const;

        Radian &operator *=(Real fScalar);
        Radian &operator /=(Real fScalar);

        Real valueDegrees() const;
        Real valueRadians() const;

    private:
        Real    m_fRadian;
    };

    #include "T3DRadian.inl"

    inline Radian operator *(Real fScalar, const Radian &radians)
    {
        return Radian(fScalar * radians.valueRadians());
    }
}

#endif  /*__T3D_RADIAN_H__*/