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


namespace Tiny3D
{
    template <typename T>
    inline TTriangle<T>::TTriangle()
    {
        mVertices[0] = TReal<T>::ZERO;
        mVertices[1] = TReal<T>::ZERO;
        mVertices[2] = TReal<T>::ZERO;
    }

    template <typename T>
    inline TTriangle<T>::TTriangle(const TVector3<T> *vertices)
    {
        mVertices[0] = vertices[0];
        mVertices[1] = vertices[1];
        mVertices[2] = vertices[2];
    }

    template <typename T>
    inline TTriangle<T>::TTriangle(const T *vertices)
    {
        mVertices[0] = TVector3<T>(vertices[0], vertices[1], vertices[2]);
        mVertices[1] = TVector3<T>(vertices[3], vertices[4], vertices[5]);
        mVertices[2] = TVector3<T>(vertices[6], vertices[7], vertices[8]);
    }

    template <typename T>
    inline void TTriangle<T>::setVertices(const TVector3<T> *vertices)
    {
        mVertices[0] = vertices[0];
        mVertices[1] = vertices[1];
        mVertices[2] = vertices[2];
    }

    template <typename T>
    inline void TTriangle<T>::setVertices(const T *vertices)
    {
        mVertices[0] = TVector3<T>(vertices[0], vertices[1], vertices[2]);
        mVertices[1] = TVector3<T>(vertices[3], vertices[4], vertices[5]);
        mVertices[2] = TVector3<T>(vertices[6], vertices[7], vertices[8]);
    }

    template <typename T>
    inline const TVector3<T> &TTriangle<T>::operator [](size_t idx) const
    {
        T3D_ASSERT(idx < 3, "The idx is out of bound !");
        return mVertices[idx];
    }

    template <typename T>
    inline TVector3<T> &TTriangle<T>::operator [](size_t idx)
    {
        T3D_ASSERT(idx < 3, "The idx is out of bound !");
        return mVertices[idx];
    }
}

