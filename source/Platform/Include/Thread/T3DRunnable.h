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

#ifndef __T3D_RUNNABLE_H__
#define __T3D_RUNNABLE_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API Runnable : public Allocator
    {
        friend class RunnableThread;

    public:
        /**
         * 初始化回调，在线程中执行
         * @return 返回 true，初始化成功，线程继续执行，否则线程终止执行
         */
        virtual bool init();

        /**
         * 线程执行函数，子类负责具体实现
         * @return 返回线程执行返回值
         */
        virtual TResult run() = 0;

        /**
         * 其他线程中调用，用于停止线程执行，非暴力终止线程
         */
        virtual void stop();

        /**
         * 线程执行结束回调
         */
        virtual void exit();
    };
}

#endif  /*__T3D_RUNNABLE_H__*/
