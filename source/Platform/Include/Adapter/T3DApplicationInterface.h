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


#ifndef __T3D_APPLICATION_INTERFACE_H__
#define __T3D_APPLICATION_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class IApplication
    {
        T3D_DECLARE_INTERFACE(IApplication);

    public:
        /**
         * @brief 初始化应用程序
         * @return 调用成功返回T3D_OK
         */
        virtual TResult init() = 0;

        /**
         * @brief 轮询处理应用程序事件
         * @return 返回 false 时表示需要退出程序
         */
        virtual bool pollEvents() = 0;

        /**
         * @brief 处理应用程序事件
         * @param [in] evt : 事件，不同平台不一样定义，自己解析
         * @return 返回 false 时表示需要退出程序
         */
        virtual bool processEvents(void *evt) = 0;

        /**
         * @brief 释放应用程序对象资源
         */
        virtual void release() = 0;

        /**
         * @brief 获取平台原生应用对象
         */
        virtual void *getNativeAppObject() = 0;

        /**
         * @brief 设置是否后台运行
         * @param [in] enable : 开启或关闭后台运行
         */
        virtual void setRunInBackground(bool enable) = 0;

        /**
         * @brief 获取是否开启后台运行
         * @return 返回是否开启后台运行
         */
        virtual bool isRunInBackground() const = 0;
    };
}



#endif  /*__T3D_APPLICATION_INTERFACE_H__*/
