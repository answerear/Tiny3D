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


#include "T3DPlane.h"


namespace Tiny3D
{
    template <typename T>
    inline TIntrPointFrustum<T>::TIntrPointFrustum()
        : mPoint(nullptr)
        , mFrustum(nullptr)
    {

    }

    template <typename T>
    inline TIntrPointFrustum<T>::TIntrPointFrustum(
        const TVector3<T> &point,
        const TFrustum<T> &frustum)
        : mPoint(&point)
        , mFrustum(&frustum)
    {

    }

    template <typename T>
    inline TIntrPointFrustum<T>::TIntrPointFrustum(
        const TVector3<T> *point,
        const TFrustum<T> *frustum)
        : mPoint(point)
        , mFrustum(frustum)
    {

    }

    template <typename T>
    inline bool TIntrPointFrustum<T>::test()
    {
        if (mPoint == nullptr || mFrustum == nullptr)
            return false;

        const TVector3<T> &point = *mPoint;

        // 近平面
        const TPlane<T> &nearPlane = mFrustum->getFace(E_FACE_NEAR);
        typename TPlane<T>::Side side = nearPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        // 远平面
        const TPlane<T> &farPlane = mFrustum->getFace(E_FACE_FAR);
        side = farPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        // 左平面
        const TPlane<T> &leftPlane = mFrustum->getFace(E_FACE_LEFT);
        side = farPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        // 右平面
        const TPlane<T> &rightPlane = mFrustum->getFace(E_FACE_RIGHT);
        side = farPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        // 上平面
        const TPlane<T> &topPlane = mFrustum->getFace(E_FACE_TOP);
        side = topPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        // 下平面
        const TPlane<T> &bottomPlane = mFrustum->getFace(E_FACE_BOTTOM);
        side = bottomPlane.fastSideForPoint(point);
        if (side == TPlane<T>::E_SIDE_NEGATIVE)
            return false;

        return true;
    }
}

