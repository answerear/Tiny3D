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
    inline TIntrSphereSphere<T>::TIntrSphereSphere()
        : mSphere0(nullptr)
        , mSphere1(nullptr)
    {

    }

    template <typename T>
    inline TIntrSphereSphere<T>::TIntrSphereSphere(
        const TSphere<T> &s0,
        const TSphere<T> &s1)
        : mSphere0(&s0)
        , mSphere1(&s1)
    {

    }

    template <typename T>
    inline TIntrSphereSphere<T>::TIntrSphereSphere(
        const TSphere<T> *s0,
        const TSphere<T> *s1)
        : mSphere0(s0)
        , mSphere1(s1)
    {

    }

    template <typename T>
    bool TIntrSphereSphere<T>::test()
    {
        if (mSphere0 == nullptr || mSphere1 == nullptr)
            return false;

        T r = mSphere0->getRadius() + mSphere1->getRadius();
        T r2 = r * r;

        return ((mSphere0->getCenter() - mSphere1->getCenter()).length2() < r2); 
    }
}

