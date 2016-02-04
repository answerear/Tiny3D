

#ifndef __T3D_SIZE_H__
#define __T3D_SIZE_H__


#include "T3DPrerequisites.h"
#include "Math/T3DVector2.h"


namespace Tiny3D
{
    template <typename T>
    class T3D_ENGINE_API Size
    {
    public:
        Size()
            : width(0)
            , height(0)
        {

        }

        Size(const T &w, const T &h)
            : width(w)
            , height(h)
        {

        }

        Size(const Vector2 &other)
        {
            width = (T)other.x();
            height = (T)other.y();
        }

        Size &operator =(const Vector2 &other)
        {
            width = (T)other.x();
            height = (T)other.y();
            return *this;
        }

        template <typename T1>
        explicit Size(const Size<T1> &other)
        {
            width = (T)other.width;
            height = (T)other.height;
        }

        template <typename T1>
        Size<T> &operator =(const Size<T1> &other)
        {
            width = (T)other.width;
            height = (T)other.height;
            return *this;
        }

        bool operator ==(const Size &other) const
        {
            return (width == other.width && height == other.height);
        }

        bool operator !=(const Size &other) const
        {
            return !(this == other);
        }

        Size operator +(const Size &other) const
        {
            return Size(width + other.width, height + other.height);
        }

        Size &operator +=(const Size &other)
        {
            width += other.width;
            height += other.height;
            return *this;
        }

        Size operator -(const Size &other) const
        {
            return Size(width - other.width, height - other.height);
        }

        Size &operator -=(const Size &other)
        {
            width -= other.width;
            height -= other.height;
            return *this;
        }

        Size operator *(const T &scale) const
        {
            return Size(scale * width, scale * height);
        }

        Size &operator *=(const T &scale)
        {
            width *= scale;
            height *= scale;
            return *this;
        }

        Size operator /(const T &scale) const
        {
            return Size(width / scale, height / scale);
        }

        Size &operator /=(const T &scale)
        {
            width /= scale;
            height /= scale;
            return *this;
        }

    public:
        T  width;
        T  height;
    };

    typedef Size<int32_t>   Sizei;
    typedef Size<Real>      Sizer;
}


#endif  /*__T3D_SIZE_H__*/
