

#ifndef __T3D_RECT_H__
#define __T3D_RECT_H__


#include "T3DPrerequisites.h"
#include "Math/T3DSize.h"


namespace Tiny3D
{
    template <typename T>
    class T3D_ENGINE_API Rect
    {
    public:
        Rect()
            : left(0)
            , top(0)
            , right(0)
            , bottom(0)
        {

        }

        Rect(const T &l, const T &t, const T &r, const T &b)
            : left(l)
            , top(t)
            , right(r)
            , bottom(b)
        {

        }

        Rect(const T &x, const T &y, const Size<T> &size)
            : left(x)
            , top(y)
        {
            right = x + size.width - 1;
            bottom = y + size.height - 1;
        }

        template <typename T1>
        explicit Rect(const Rect<T1> &other)
        {
            left = (T)other.left;
            top = (T)other.top;
            right = (T)other.right;
            bottom = (T)other.bottom;
        }

        template <typename T1>
        Rect &operator =(const Rect<T1> &other)
        {
            left = (T)other.left;
            top = (T)other.top;
            right = (T)other.right;
            bottom = (T)other.bottom;
            return *this;
        }

        bool empty() const
        {
            return (left == right && top == bottom);
        }

        bool operator ==(const Rect &other) const
        {
            return (left == other.left && top == other.top && right == other.right && bottom == other.bottom);
        }

        bool operator !=(const Rect &other) const
        {
            return !(*this == other);
        }

        T   left;
        T   top;
        T   right;
        T   bottom;
    };
}


#endif  /*__T3D_RECT_H__*/
