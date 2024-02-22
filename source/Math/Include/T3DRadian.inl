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
TRadian<T>::TRadian()
    : mRadian(TReal<T>::ZERO)
{
    
}


template <typename T>
inline TRadian<T>::TRadian(T radian)
    : mRadian(radian)
{

}

template <typename T>
inline TRadian<T>::TRadian(const TRadian &other)
    : mRadian(other.mRadian)
{

}

template <typename T>
inline TRadian<T> &TRadian<T>::operator =(T radian)
{
    mRadian = radian;
    return *this;
}

template <typename T>
inline TRadian<T> &TRadian<T>::operator =(const TRadian &other)
{
    mRadian = other.mRadian;
    return *this;
}

template <typename T>
inline const TRadian<T> &TRadian<T>::operator +() const
{
    return *this;
}

template <typename T>
inline TRadian<T> TRadian<T>::operator -() const
{
    return TRadian(-mRadian);
}

template <typename T>
inline bool TRadian<T>::operator ==(const TRadian &other) const
{
    return (mRadian == other.mRadian);
}

template <typename T>
inline bool TRadian<T>::operator !=(const TRadian &other) const
{
    return (mRadian != other.mRadian);
}

template <typename T>
inline bool TRadian<T>::operator <(const TRadian &other) const
{
    return (mRadian < other.mRadian);
}

template <typename T>
inline bool TRadian<T>::operator <=(const TRadian &other) const
{
    return (mRadian <= other.mRadian);
}

template <typename T>
inline bool TRadian<T>::operator >(const TRadian &other) const
{
    return (mRadian > other.mRadian);
}

template <typename T>
inline bool TRadian<T>::operator >=(const TRadian &other) const
{
    return (mRadian >= other.mRadian);
}

template <typename T>
inline TRadian<T> TRadian<T>::operator +(const TRadian &other) const
{
    return TRadian(mRadian + other.mRadian);
}

template <typename T>
inline TRadian<T> TRadian<T>::operator -(const TRadian &other) const
{
    return TRadian(mRadian - other.mRadian);
}

template <typename T>
inline TRadian<T> &TRadian<T>::operator +=(const TRadian &other)
{
    mRadian += other.mRadian;
    return *this;
}

template <typename T>
inline TRadian<T> &TRadian<T>::operator -=(const TRadian &other)
{
    mRadian -= other.mRadian;
    return *this;
}

template <typename T>
inline TRadian<T> TRadian<T>::operator *(T scalar) const
{
    return TRadian(mRadian * scalar);
}

template <typename T>
inline TRadian<T> TRadian<T>::operator /(T scalar) const
{
    if (scalar != TReal<T>::ZERO)
        return TRadian(mRadian / scalar);
    return TRadian();
}

template <typename T>
inline TRadian<T> &TRadian<T>::operator *=(T scalar)
{
    mRadian *= scalar;
    return *this;
}

template <typename T>
inline TRadian<T> &TRadian<T>::operator /=(T scalar)
{
    if (scalar != TReal<T>::ZERO)
    {
        mRadian /= scalar;
    }
    else
    {
        mRadian = TReal<T>::ZERO;
    }

    return *this;
}

template <typename T>
inline T TRadian<T>::valueRadians() const
{
    return mRadian;
}


