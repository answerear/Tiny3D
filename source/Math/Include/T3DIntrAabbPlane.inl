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
    inline TIntrAabbPlane<T>::TIntrAabbPlane()
        : mBox(nullptr)
        , mPlane(nullptr)
    {

    }

    template <typename T>
    inline TIntrAabbPlane<T>::TIntrAabbPlane(
        const TAabb<T> &box, 
        const TPlane<T> &plane)
        : mBox(&box)
        , mPlane(&plane)
    {

    }

    template <typename T>
    inline TIntrAabbPlane<T>::TIntrAabbPlane(
        const TAabb<T> *box,
        const TPlane<T> *plane)
        : mBox(box)
        , mPlane(plane)
    {

    }

    template <typename T>
    inline int32_t TIntrAabbPlane<T>::test()
    {
        if (mBox == nullptr || mPlane == nullptr)
            return -1;

        T minD, maxD;
        const TVector3<T> &n = mPlane->getNormal();

        if (n.x() > TReal<T>::ZERO)
        {
            minD = n.x() * mBox->getMinX();
            maxD = n.x() * mBox->getMaxX();
        }
        else
        {
            minD = n.x() * mBox->getMaxX();
            maxD = n.x() * mBox->getMinX();
        }

        if (n.y() > TReal<T>::ZERO)
        {
            minD += n.y() * mBox->getMinY();
            maxD += n.y() * mBox->getMaxY();
        }
        else
        {
            minD += n.y() * mBox->getMaxY();
            maxD += n.y() * mBox->getMinY();
        }

        if (n.z() > TReal<T>::ZERO)
        {
            minD += n.z() * mBox->getMinZ();
            maxD += n.z() * mBox->getMaxZ();
        }
        else
        {
            minD += n.z() * mBox->getMaxZ();
            maxD += n.z() * mBox->getMinZ();
        }

        if (minD >= mPlane->getDistance())
            return 1;

        if (maxD <= mPlane->getDistance())
            return -1;

        return 0;
    }
}

