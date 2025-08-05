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

#ifndef __T3D_RAY_H__
#define __T3D_RAY_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    /**
     * @brief 射线类
     * @remarks 这里使用 f(t) = P + t * D 的形式来表示。 其中P表示射线原点，D表示
     *      射线方向向量，|D|是射线长度，而 0 <= t <= 1。
     */
    TSTRUCT()
    template <typename T>
    struct TRay
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TRay()
            : mOrigin(T(0), T(0), T(0))
            , mDirection(T(0), T(0), T(0))
        {
        }

        /// 初始化射线起点和方向的构造函数
        TRay(const TVector3<T> &rkOrigin, const TVector3<T> rkDirection)
            : mOrigin(rkOrigin)
            , mDirection(rkDirection)
        {
        }

        /// 获取射线原点位置
        TPROPERTY(RTTRFuncName="origin", RTTRFuncType="getter")
        const TVector3<T> &getOrigin() const
        {
            return mOrigin;
        }
        
        TVector3<T> &getOrigin()
        {
            return mOrigin;
        }

        /// 获取射线方向
        TPROPERTY(RTTRFuncName="direction", RTTRFuncType="setter")
        const TVector3<T> &getDirection() const
        {
            return mDirection;
        }

        TVector3<T> &getDirection()
        {
            return mDirection;
        }

        /// 设置射线原点
        TPROPERTY(RTTRFuncName="origin", RTTRFuncTYpe="setter")
        void setOrigin(const TVector3<T> &origin)
        {
            mOrigin = origin;
        }

        /// 设置射线方向
        TPROPERTY(RTTRFuncName="direction", RTTRFuncTYpe="setter")
        void setDirection(const TVector3<T> &direction)
        {
            mDirection = direction;
        }

        String getDebugString() const
        {
            std::stringstream ss;
            ss << "Origin : " << mOrigin.getDebugString() << " Direction : " << mDirection.getDebugString();
            return ss.str();
        }

    private:
        TVector3<T> mOrigin;        /// 原点
        TVector3<T> mDirection;     /// 方向
    };
}


#endif    /*__T3D_RAY_H__*/
