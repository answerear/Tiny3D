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

#ifndef __T3D_EVENT_PARAM_H__
#define __T3D_EVENT_PARAM_H__


#include "T3DEventPrerequisites.h"


namespace Tiny3D
{
    class T3D_FRAMEWORK_API EventParam
    {
        T3D_DECLARE_INTERFACE(EventParam);

    public:
        /**
         * @brief 克隆接口
         * @note 所有子类都要实现以克隆一个深拷贝的子类对象出来。 主要用于异步
         *      事件派发时候能保存一个深拷贝的副本
         */
        virtual EventParam *clone() = 0;
    };

    template <typename T1>
    class EventParamT1 : public EventParam
    {
    public:
        EventParamT1() = default;
        
        EventParamT1(T1 a1) : arg1(a1) {}

        EventParam *clone() override { return new EventParamT1(arg1); }
        
        T1 arg1 {};
    };

    template <typename T1, typename T2>
    class EventParamT2 : public EventParam
    {
    public:
        EventParamT2() = default;
        
        EventParamT2(T1 a1, T2 a2) : arg1(a1), arg2(a2) {}

        EventParam *clone() override { return new EventParamT2(arg1, arg2); }
        
        T1 arg1 {};
        T2 arg2 {};
    };


    template <typename T1, typename T2, typename T3>
    class EventParamT3 : public EventParam
    {
    public:
        EventParamT3() = default;
        
        EventParamT3(T1 a1, T2 a2, T3 a3) : arg1(a1), arg2(a2), arg3(a3) {}

        EventParam *clone() override { return new EventParamT3(arg1, arg2, arg3); }
        
        T1 arg1 {};
        T2 arg2 {};
        T3 arg3 {};
    };

    template <typename T1, typename T2, typename T3, typename T4>
    class EventParamT4 : public EventParam
    {
    public:
        EventParamT4() = default;
        
        EventParamT4(T1 a1, T2 a2, T3 a3, T4 a4) : arg1(a1), arg2(a2), arg3(a3), arg4(a4) {}

        EventParam *clone() override { return new EventParamT4(arg1, arg2, arg3, arg4); }
        
        T1 arg1 {};
        T2 arg2 {};
        T3 arg3 {};
        T4 arg4 {};
    };
}


#endif  /*__T3D_EVENT_PARAM_H__*/
