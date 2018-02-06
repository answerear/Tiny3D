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

#ifndef __T3D_DEGREE_H__
#define __T3D_DEGREE_H__


namespace Tiny3D
{
    class T3D_MATH_API Degree
    {
    public:
        /// Create a degree with float value.
        explicit Degree(Real fDegree = 0.0);
        /// Create a degree with radian.
        Degree(const Radian &radian);
        /// Copy constructor.
        Degree(const Degree &other);

        /// Assignment
        Degree &operator =(Real fDegree);
        Degree &operator =(const Degree &other);
        Degree &operator =(const Radian &radian);

        const Degree &operator +() const;
        Degree operator -() const;

        /// Comparison (equal to).
        bool operator ==(const Degree &other) const;
        /// Comparison (not equal to).
        bool operator !=(const Degree &other) const;
        ///
        bool operator <(const Degree &other) const;
        bool operator <=(const Degree &other) const;
        bool operator >(const Degree &other) const;
        bool operator >=(const Degree &other) const;

        Degree operator +(const Degree &other) const;
        Degree operator -(const Degree &other) const;

        Degree operator +(const Radian &radian) const;
        Degree operator -(const Radian &radian) const;

        Degree &operator +=(const Degree &other);
        Degree &operator -=(const Degree &other);

        Degree &operator +=(const Radian &radian);
        Degree &operator -=(const Radian &radian);

        Degree operator *(Real fScalar) const;
        Degree operator /(Real fScalar) const;

        Degree &operator *=(Real fScalar);
        Degree &operator /=(Real fScalar);

        Real valueDegrees() const;
        Real valueRadians() const;

    private:
        Real    m_fDegree;
    };

    #include "T3DDegree.inl"

    inline Degree operator *(Real fScalar, const Degree &degree)
    {
        return Degree(fScalar * degree.valueDegrees());
    }
}

#endif  /*__T3D_DEGREE_H__*/
