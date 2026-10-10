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

#ifndef __T3D_RECT_H__
#define __T3D_RECT_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector2.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TSTRUCT()
    template <typename T>
    struct TSize
    {
        TPROPERTY()
        T width;
        TPROPERTY()
        T height;

        TSize()
            : width(0)
            , height(0)
        {

        }

        TSize(const T &w, const T &h)
            : width(w)
            , height(h)
        {

        }

        TSize(const TVector2<T> &other)
        {
            width = (T)other.x();
            height = (T)other.y();
        }

        bool isEmpty() const
        {
            return (width == 0 && height == 0);
        }

        TSize &operator =(const TVector2<T> &other)
        {
            width = (T)other.x();
            height = (T)other.y();
            return *this;
        }

        template <typename T1>
        explicit TSize(const TSize<T1> &other)
        {
            width = (T)other.width;
            height = (T)other.height;
        }

        template <typename T1>
        TSize<T> &operator =(const TSize<T1> &other)
        {
            width = (T)other.width;
            height = (T)other.height;
            return *this;
        }

        bool operator ==(const TSize &other) const
        {
            return (width == other.width && height == other.height);
        }

        bool operator !=(const TSize &other) const
        {
            return !(this == other);
        }

        TSize operator +(const TSize &other) const
        {
            return TSize(width + other.width, height + other.height);
        }

        TSize &operator +=(const TSize &other)
        {
            width += other.width;
            height += other.height;
            return *this;
        }

        TSize operator -(const TSize &other) const
        {
            return TSize(width - other.width, height - other.height);
        }

        TSize &operator -=(const TSize &other)
        {
            width -= other.width;
            height -= other.height;
            return *this;
        }

        TSize operator *(const T &scale) const
        {
            return TSize(scale * width, scale * height);
        }

        TSize &operator *=(const T &scale)
        {
            width *= scale;
            height *= scale;
            return *this;
        }

        TSize operator /(const T &scale) const
        {
            return Size(width / scale, height / scale);
        }

        TSize &operator /=(const T &scale)
        {
            width /= scale;
            height /= scale;
            return *this;
        }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "(" << width << ", " << height << ")";
            return ss.str();
        }
    };

    //--------------------------------------------------------------------------

    TSTRUCT()
    template <typename T>
    struct TPoint
    {
        TPROPERTY()
        T x;
        
        TPROPERTY()
        T y;

        TPoint()
            : x(0)
            , y(0)
        {

        }

        TPoint(const T &_x, const T &_y)
            : x(_x)
            , y(_y)
        {

        }

        template <typename T1>
        explicit TPoint(const TPoint<T1> &other)
        {
            x = (T)other.x;
            y = (T)other.y;
        }

        template <typename T1>
        TPoint<T> &operator =(const TPoint<T1> &other)
        {
            x = (T)other.x;
            y = (T)other.y;
            return *this;
        }

        bool isZero() const
        {
            return (x == 0 && y == 0);
        }

        void setZero()
        {
            x = 0, y = 0;
        }

        bool operator ==(const TPoint &other) const
        {
            return (x == other.x && y == other.y);
        }

        bool operator !=(const TPoint &other) const
        {
            return !(*this == other);
        }

        TPoint operator -() const
        {
            return TPoint(-x, -y);
        }

        TPoint operator +(const TPoint &other) const
        {
            return TPoint(x + other.x, y + other.y);
        }

        TPoint &operator +=(const TPoint &other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        TPoint operator -(const TPoint &other) const
        {
            return TPoint(x - other.x, y - other.y);
        }

        TPoint &operator -=(const TPoint &other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        TPoint operator *(const T &scale) const
        {
            return TPoint(x * scale, y * scale);
        }

        TPoint &operator *=(const T &scale)
        {
            x *= scale;
            y *= scale;
            return *this;
        }

        TPoint operator /(const T &scale) const
        {
            return TPoint(x / scale, y / scale);
        }

        TPoint &operator /=(const T &scale)
        {
            x /= scale;
            y /= scale;
            return *this;
        }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "(" << x << ", " << y << ")";
            return ss.str();
        }
    };

    //--------------------------------------------------------------------------

    TSTRUCT()
    template <typename T>
    struct TRect
    {
        TPROPERTY()
        T left;

        TPROPERTY()
        T top;

        TPROPERTY()
        T right;

        TPROPERTY()
        T bottom;

        TRect()
            : left(0)
            , top(0)
            , right(0)
            , bottom(0)
        {

        }

        TRect(const T &l, const T &t, const T &r, const T &b)
            : left(l)
            , top(t)
            , right(r)
            , bottom(b)
        {

        }

        TRect(const TPoint<T> &pos, const TSize<T> &size)
        {
            left = pos.x;
            top = pos.y;
            right = pos.x + size.width - 1;
            bottom = pos.y + size.height - 1;
        }

        template <typename T1>
        explicit TRect(const TRect<T1> &other)
        {
            left = (T)other.left;
            top = (T)other.top;
            right = (T)other.right;
            bottom = (T)other.bottom;
        }

        template <typename T1>
        TRect &operator =(const TRect<T1> &other)
        {
            left = (T)other.left;
            top = (T)other.top;
            right = (T)other.right;
            bottom = (T)other.bottom;
            return *this;
        }

        bool operator ==(const TRect &other) const
        {
            return (left == other.left && top == other.top && right == other.right && bottom == other.bottom);
        }

        bool operator !=(const TRect &other) const
        {
            return !(*this == other);
        }

        T x() const
        {
            return left;
        }

        T y() const
        {
            return top;
        }

        T width() const
        {
            return right - left + 1;
        }

        T height() const
        {
            return bottom - top + 1;
        }

        TSize<T> size() const
        {
            return TSize<T>(width(), height());
        }

        TPoint<T> position() const
        {
            return TPoint<T>(left, top);
        }

        bool isEmpty() const
        {
            return (width() == 0 && height() == 0);
        }

        void setEmpty()
        {
            left = top = right = bottom = 0;
        }

        TRect operator +(const TPoint<T> &offset) const
        {
            return TRect(left + offset.x, top + offset.y, right + offset.x, bottom + offset.y);
        }

        TRect &operator +=(const TPoint<T> &offset)
        {
            left += offset.x;
            right += offset.x;
            top += offset.y;
            bottom += offset.y;
            return *this;
        }

        TRect operator -(const TPoint<T> &offset) const
        {
            return TRect(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y);
        }

        TRect &operator -=(const TPoint<T> &offset)
        {
            left -= offset.x;
            right -= offset.x;
            top -= offset.y;
            bottom -= offset.y;
            return *this;
        }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "(" << left << ", " << top << "), (" << right << ", " << bottom << ")";
            return ss.str();
        }
    };
}


#endif  /*__T3D_RECT_H__*/
