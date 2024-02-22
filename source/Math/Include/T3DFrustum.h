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

#ifndef __T3D_FRUSTUM_H__
#define __T3D_FRUSTUM_H__


#include "T3DMathPrerequisites.h"
#include "T3DPlane.h"


namespace Tiny3D
{
    TENUM()
    enum Face
    {
        E_FACE_TOP = 0,
        E_FACE_BOTTOM,
        E_FACE_LEFT,
        E_FACE_RIGHT,
        E_FACE_NEAR,
        E_FACE_FAR,
        E_MAX_FACE
    };

    TSTRUCT()
    template <typename T>
    struct TFrustum
    {
        TRTTI_FRIEND

    public:
        TFrustum()
        {
        }

        TFUNCTION()
        void setFace(Face face, const TPlane<T> &plane)
        {
            T3D_ASSERT(face >= 0 && face < E_MAX_FACE, "The number of faces is out of bound !");
            mFaces[face] = plane;
        }

        TFUNCTION()
        const TPlane<T> &getFace(Face face) const
        {
            T3D_ASSERT(face >= 0 && face < E_MAX_FACE, "The number of faces is out of bound !");
            return mFaces[face];
        }

    protected:
        TPROPERTY(RTTRFuncName="faces", RTTRFuncType="getter", "Desciption"="Six faces in frustum")
        TArray<TPlane<T>> getFaces() const { return TArray<TPlane<T>>(mFaces, mFaces+E_MAX_FACE); }

        TPROPERTY(RTTRFuncName="faces", RTTRFuncType="setter", "Desciption"="Six faces in frustum")
        void setFaces(TArray<TPlane<T>> faces)
        {
            size_t i = 0;
            for (const auto &face : faces)
            {
                mFaces[i++] = face;
            }
        }
        
        TPlane<T>   mFaces[E_MAX_FACE];
    };
}


#endif  /*__T3D_FRUSTUM_H__*/
