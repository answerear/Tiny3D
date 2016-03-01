

#ifndef __T3D_RECT_H__
#define __T3D_RECT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    template <typename T>
    struct TSize
    {
        T width, height;

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

        TSize(const Vector2 &other)
        {
            width = (T)other.x();
            height = (T)other.y();
        }

        TSize &operator =(const Vector2 &other)
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
    };


    template <typename T>
    struct TPoint
    {
        T x, y;

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
            y = (T)other.y:
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
            return (x == othre.x && y == other.y);
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
    };


    template <typename T>
    struct TRect
    {
        T left, top, right, bottom;

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

        TSize size() const
        {
            return TSize(width(), height());
        }

        TPoint position() const
        {
            return TPoint(left, top);
        }

        bool isEmpty() const
        {
            return (width() == 0 && height() == 0);
        }

        void setEmpty()
        {
            left = top = right = bottom = 0;
        }

        TRect operator +(const TPoint &offset) const
        {
            return TRect(left + offset.x, top + offset.y, right + offset.x, bottom + offset.y);
        }

        TRect &operator +=(const TPoint &offset)
        {
            left += offset.x;
            right += offset.x;
            top += offset.y;
            bottom += offset.y;
            return *this;
        }

        TRect operator -(const TPoint &offset) const
        {
            return TRect(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y);
        }

        TRect &operator -=(const TPoint &offset)
        {
            left -= offset.x;
            right -= offset.x;
            top -= offset.y;
            bottom -= offset.y;
            return *this;
        }
    };

    typedef TSize<long_t>       Size;
    typedef TSize<Real>         RealSize;

    typedef TPoint<long_t>      Point;
    typedef TPoint<Real>        RealPoint;

    typedef TRect<long_t>       Rect;
    typedef TRect<Real>         RealRect;
}


#endif  /*__T3D_RECT_H__*/
