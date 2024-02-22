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

template <typename T>
inline TDegree<T>::TDegree()
    : mDegree(TReal<T>::ZERO)
{
    
}


template <typename T>
inline TDegree<T>::TDegree(T degree /* = TReal<T>::ZERO */)
    : mDegree(degree)
{

}

template <typename T>
inline TDegree<T>::TDegree(const TDegree &other)
{
    mDegree = other.mDegree;
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator =(T degree)
{
    mDegree = degree;
    return *this;
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator =(const TDegree &other)
{
    mDegree = other.mDegree;
    return *this;
}

template <typename T>
inline const TDegree<T> &TDegree<T>::operator +() const
{
    return *this;
}

template <typename T>
inline TDegree<T> TDegree<T>::operator -() const
{
    return TDegree(-mDegree);
}

template <typename T>
inline bool TDegree<T>::operator ==(const TDegree &other) const
{
    return (mDegree == other.mDegree);
}

template <typename T>
inline bool TDegree<T>::operator !=(const TDegree &other) const
{
    return (mDegree != other.mDegree);
}

template <typename T>
inline bool TDegree<T>::operator <(const TDegree &other) const
{
    return (mDegree < other.mDegree);
}

template <typename T>
inline bool TDegree<T>::operator <=(const TDegree &other) const
{
    return (mDegree <= other.mDegree);
}

template <typename T>
inline bool TDegree<T>::operator >(const TDegree &other) const
{
    return (mDegree > other.mDegree);
}

template <typename T>
inline bool TDegree<T>::operator >=(const TDegree &other) const
{
    return (mDegree >= other.mDegree);
}

template <typename T>
inline TDegree<T> TDegree<T>::operator +(const TDegree &other) const
{
    return TDegree(mDegree + other.mDegree);
}

template <typename T>
inline TDegree<T> TDegree<T>::operator -(const TDegree &other) const
{
    return TDegree(mDegree - other.mDegree);
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator +=(const TDegree &other)
{
    mDegree += other.mDegree;
    return *this;
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator -=(const TDegree &other)
{
    mDegree -= other.mDegree;
    return *this;
}

template <typename T>
inline TDegree<T> TDegree<T>::operator *(T scalar) const
{
    return TDegree(mDegree * scalar);
}

template <typename T>
inline TDegree<T> TDegree<T>::operator /(T scalar) const
{
    if (scalar != TReal<T>::ZERO)
        return TDegree(mDegree / scalar);
    return TDegree();
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator *=(T scalar)
{
    mDegree *= scalar;
    return *this;
}

template <typename T>
inline TDegree<T> &TDegree<T>::operator /=(T scalar)
{
    if (scalar != TReal<T>::ZERO)
    {
        mDegree /= scalar;
    }
    else
    {
        mDegree = TReal<T>::ZERO;
    }
    return *this;
}

template <typename T>
inline T TDegree<T>::valueDegrees() const
{
    return mDegree;
}

