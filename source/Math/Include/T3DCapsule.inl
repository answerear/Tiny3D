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
    inline TCapsule<T>::TCapsule()
        : mPoint0(TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO)
        , mPoint1(TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO)
        , mRadius(TReal<T>::ZERO)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TVector3<T> &p0, const TVector3<T> &p1,
        const T &radius)
        : mPoint0(p0)
        , mPoint1(p1)
        , mRadius(radius)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TVector3<T> &center,
        const T &cylinderHalfHeight, const T &radius, const TVector3<T> &axis)
        : mPoint0(center - axis * cylinderHalfHeight)
        , mPoint1(center + axis * cylinderHalfHeight)
        , mRadius(radius)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TCapsule &other)
        : mPoint0(other.mPoint0)
        , mPoint1(other.mPoint1)
        , mRadius(other.mRadius)
    {
    }

    template <typename T>
    inline TCapsule<T> &TCapsule<T>::operator =(const TCapsule &other)
    {
        mPoint0 = other.mPoint0;
        mPoint1 = other.mPoint1;
        mRadius = other.mRadius;
        return *this;
    }

    template <typename T>
    inline const TVector3<T> &TCapsule<T>::getPoint0() const
    {
        return mPoint0;
    }

    template <typename T>
    inline TVector3<T> &TCapsule<T>::getPoint0()
    {
        return mPoint0;
    }

    template <typename T>
    inline const TVector3<T> &TCapsule<T>::getPoint1() const
    {
        return mPoint1;
    }

    template <typename T>
    inline TVector3<T> &TCapsule<T>::getPoint1()
    {
        return mPoint1;
    }

    template <typename T>
    inline const T &TCapsule<T>::getRadius() const
    {
        return mRadius;
    }

    template <typename T>
    inline T &TCapsule<T>::getRadius()
    {
        return mRadius;
    }

    template <typename T>
    inline void TCapsule<T>::setPoint0(const TVector3<T> &p0)
    {
        mPoint0 = p0;
    }

    template <typename T>
    inline void TCapsule<T>::setPoint1(const TVector3<T> &p1)
    {
        mPoint1 = p1;
    }

    template <typename T>
    inline void TCapsule<T>::setRadius(const T &radius)
    {
        mRadius = radius;
    }

    template <typename T>
    inline void TCapsule<T>::build(const TVector3<T> points[], size_t count)
    {
        if (points == nullptr || count == 0)
        {
            mPoint0 = TVector3<T>::ZERO;
            mPoint1 = TVector3<T>::ZERO;
            mRadius = TReal<T>::ZERO;
            return;
        }

        // 1. 质心
        TVector3<T> center(TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO);
        for (size_t i = 0; i < count; ++i)
        {
            center += points[i];
        }
        center /= (T)count;

        // 2. 协方差矩阵（3x3 对称），仅用普通数组，避免依赖 Matrix3
        T cov[3][3] = { { TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO },
                        { TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO },
                        { TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO } };
        for (size_t i = 0; i < count; ++i)
        {
            TVector3<T> d = points[i] - center;
            cov[0][0] += d.x() * d.x();
            cov[0][1] += d.x() * d.y();
            cov[0][2] += d.x() * d.z();
            cov[1][1] += d.y() * d.y();
            cov[1][2] += d.y() * d.z();
            cov[2][2] += d.z() * d.z();
        }
        cov[1][0] = cov[0][1];
        cov[2][0] = cov[0][2];
        cov[2][1] = cov[1][2];

        // 3. Jacobi 迭代求对称矩阵特征向量，取最大特征值对应向量作为主轴
        T v[3][3] = { { TReal<T>::ONE, TReal<T>::ZERO, TReal<T>::ZERO },
                      { TReal<T>::ZERO, TReal<T>::ONE, TReal<T>::ZERO },
                      { TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ONE } };
        for (int32_t sweep = 0; sweep < 32; ++sweep)
        {
            // 最大非对角元素
            T off = TMath<T>::abs(cov[0][1]) + TMath<T>::abs(cov[0][2])
                + TMath<T>::abs(cov[1][2]);
            if (off <= TReal<T>::EPSILON)
            {
                break;
            }

            for (int32_t p = 0; p < 2; ++p)
            {
                for (int32_t q = p + 1; q < 3; ++q)
                {
                    if (TMath<T>::abs(cov[p][q]) <= TReal<T>::EPSILON)
                    {
                        continue;
                    }

                    T theta = (cov[q][q] - cov[p][p])
                        / (((T)2) * cov[p][q]);
                    T sign = (theta >= TReal<T>::ZERO)
                        ? TReal<T>::ONE : TReal<T>::MINUS_ONE;
                    T t = sign / (TMath<T>::abs(theta)
                        + TMath<T>::sqrt(theta * theta + TReal<T>::ONE));
                    T c = TReal<T>::ONE
                        / TMath<T>::sqrt(t * t + TReal<T>::ONE);
                    T s = t * c;

                    // 旋转更新 cov
                    T app = cov[p][p];
                    T aqq = cov[q][q];
                    T apq = cov[p][q];
                    cov[p][p] = c * c * app - ((T)2) * s * c * apq + s * s * aqq;
                    cov[q][q] = s * s * app + ((T)2) * s * c * apq + c * c * aqq;
                    cov[p][q] = TReal<T>::ZERO;
                    cov[q][p] = TReal<T>::ZERO;

                    for (int32_t k = 0; k < 3; ++k)
                    {
                        if (k != p && k != q)
                        {
                            T akp = cov[k][p];
                            T akq = cov[k][q];
                            cov[k][p] = c * akp - s * akq;
                            cov[p][k] = cov[k][p];
                            cov[k][q] = s * akp + c * akq;
                            cov[q][k] = cov[k][q];
                        }
                    }

                    // 累积特征向量
                    for (int32_t k = 0; k < 3; ++k)
                    {
                        T vkp = v[k][p];
                        T vkq = v[k][q];
                        v[k][p] = c * vkp - s * vkq;
                        v[k][q] = s * vkp + c * vkq;
                    }
                }
            }
        }

        // 选最大特征值（对角元）对应的列向量为主轴
        int32_t maxIdx = 0;
        if (cov[1][1] > cov[maxIdx][maxIdx]) maxIdx = 1;
        if (cov[2][2] > cov[maxIdx][maxIdx]) maxIdx = 2;

        TVector3<T> axis(v[0][maxIdx], v[1][maxIdx], v[2][maxIdx]);
        T axisLen = axis.normalize();
        if (axisLen <= TReal<T>::EPSILON)
        {
            axis = TVector3<T>::UNIT_Y;
        }

        // 4. 半径 = 最大垂距；同时记录每个点的轴向坐标与垂距
        T radius = TReal<T>::ZERO;
        for (size_t i = 0; i < count; ++i)
        {
            TVector3<T> d = points[i] - center;
            T t = d.dot(axis);
            TVector3<T> perp = d - axis * t;
            T r = perp.length();
            if (r > radius)
            {
                radius = r;
            }
        }

        // 5. 端点闭式解：保证半径固定为 radius 时胶囊恰好包含所有点
        //    s0 = min(t_i + sqrt(R^2 - r_i^2)), s1 = max(t_i - sqrt(R^2 - r_i^2))
        T r2 = radius * radius;
        T s0 = TReal<T>::INF;
        T s1 = TReal<T>::MINUS_INF;
        for (size_t i = 0; i < count; ++i)
        {
            TVector3<T> d = points[i] - center;
            T t = d.dot(axis);
            TVector3<T> perp = d - axis * t;
            T ri2 = perp.length2();
            T diff = r2 - ri2;
            T off = (diff > TReal<T>::ZERO) ? TMath<T>::sqrt(diff) : TReal<T>::ZERO;
            T lo = t + off;
            T hi = t - off;
            if (lo < s0) s0 = lo;
            if (hi > s1) s1 = hi;
        }

        mRadius = radius;
        if (s1 < s0)
        {
            // 近似球状：线段退化为一点
            mPoint0 = center;
            mPoint1 = center;
        }
        else
        {
            mPoint0 = center + axis * s0;
            mPoint1 = center + axis * s1;
        }
    }

    template <typename T>
    inline TVector3<T> TCapsule<T>::getCenter() const
    {
        return TReal<T>::HALF * (mPoint0 + mPoint1);
    }

    template <typename T>
    inline TVector3<T> TCapsule<T>::getAxis() const
    {
        TVector3<T> axis = mPoint1 - mPoint0;
        T len = axis.normalize();
        if (len == TReal<T>::ZERO)
        {
            return TVector3<T>::UNIT_Y;
        }
        return axis;
    }

    template <typename T>
    inline T TCapsule<T>::getSegmentLength() const
    {
        return (mPoint1 - mPoint0).length();
    }

    template <typename T>
    inline T TCapsule<T>::getHeight() const
    {
        return getSegmentLength() + mRadius + mRadius;
    }

    template <typename T>
    inline bool TCapsule<T>::contains(const TVector3<T> &point) const
    {
        TVector3<T> d = mPoint1 - mPoint0;
        T denom = d.length2();
        T t = TReal<T>::ZERO;

        if (denom > TReal<T>::ZERO)
        {
            t = (point - mPoint0).dot(d) / denom;
            if (t < TReal<T>::ZERO)
            {
                t = TReal<T>::ZERO;
            }
            else if (t > TReal<T>::ONE)
            {
                t = TReal<T>::ONE;
            }
        }

        TVector3<T> closest = mPoint0 + d * t;
        return ((point - closest).length2() <= mRadius * mRadius);
    }

    template <typename T>
    inline String TCapsule<T>::getDebugString() const
    {
        std::stringstream ss;
        ss << "Point0 : " << mPoint0.getDebugString()
            << " Point1 : " << mPoint1.getDebugString()
            << " Radius : " << mRadius;
        return ss.str();
    }
}