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
    inline TIntrPointObb<T>::TIntrPointObb()
        : mPoint(nullptr)
        , mBox(nullptr)
    {

    }

    template <typename T>
    inline TIntrPointObb<T>::TIntrPointObb(
        const TVector3<T> &point,
        const TObb<T> &box)
        : mPoint(&point)
        , mBox(&box)
    {

    }

    template <typename T>
    inline TIntrPointObb<T>::TIntrPointObb(
        const TVector3<T> *point,
        const TObb<T> *box)
        : mPoint(point)
        , mBox(box)
    {

    }

    template <typename T>
    inline bool TIntrPointObb<T>::test()
    {
        if (mPoint == nullptr || mBox == nullptr)
            return false;

        const TVector3<T> &point = *mPoint;

        // 相当于将点坐标从世界坐标系中转换到了OBB包围盒的物体坐标系中
        TVector3<T> pos = point - mBox->getCenter();

        // 求点在 X 轴上的投影
        float val = pos.dot(mBox->getAxis(0));

        // 判断投影是否大于x正方向的半长或小于x负方向半长
        if (val > mBox->getExtent(0) || val < -mBox->getExtent(0))
            return false;

        // 同理判断 Y 轴
        val = pos.dot(mBox->getAxis(1));
        if (val > mBox->getExtent(1) || val < -mBox->getExtent(1))
            return false;

        // 同理判断 Z 轴
        val = pos.dot(mBox->getAxis(2));
        if (val > mBox->getExtent(2) || val < -mBox->getExtent(2))
            return false;

        return true;
    }
}

