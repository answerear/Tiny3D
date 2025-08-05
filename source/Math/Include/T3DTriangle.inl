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
        vertices[0] = TReal<T>::ZERO;
        vertices[1] = TReal<T>::ZERO;
        vertices[2] = TReal<T>::ZERO;
    }

    template <typename T>
    inline TTriangle<T>::TTriangle(const TVector3<T> *verts)
    {
        vertices[0] = verts[0];
        vertices[1] = verts[1];
        vertices[2] = verts[2];
    }

    template <typename T>
    inline TTriangle<T>::TTriangle(const T *verts)
    {
        vertices[0] = TVector3<T>(verts[0], verts[1], verts[2]);
        vertices[1] = TVector3<T>(verts[3], verts[4], verts[5]);
        vertices[2] = TVector3<T>(verts[6], verts[7], verts[8]);
    }

    template <typename T>
    inline void TTriangle<T>::setVertices(const TVector3<T> *verts)
    {
        vertices[0] = verts[0];
        vertices[1] = verts[1];
        vertices[2] = verts[2];
    }

    template <typename T>
    inline void TTriangle<T>::setVertices(const T *verts)
    {
        vertices[0] = TVector3<T>(verts[0], verts[1], verts[2]);
        vertices[1] = TVector3<T>(verts[3], verts[4], verts[5]);
        vertices[2] = TVector3<T>(verts[6], verts[7], verts[8]);
    }

    template <typename T>
    inline const TVector3<T> &TTriangle<T>::operator [](size_t idx) const
    {
        T3D_ASSERT(idx < 3);
        return vertices[idx];
    }

    template <typename T>
    inline TVector3<T> &TTriangle<T>::operator [](size_t idx)
    {
        T3D_ASSERT(idx < 3);
        return vertices[idx];
    }

    template <typename T>
    inline String TTriangle<T>::getDebugString() const
    {
        std::stringstream ss;
        ss << "Triangle -";
        ss << " V0 : " << vertices[0].getDebugString();
        ss << " V1 : " << vertices[1].getDebugString();
        ss << " V2 : " << vertices[2].getDebugString();
        return ss.str();
    }
}

