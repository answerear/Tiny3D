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

#ifndef __T3D_ENVIRONMENT_H__
#define __T3D_ENVIRONMENT_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"


namespace Tiny3D
{
    /**
     * @class   Environment
     * @brief   进程环境变量读取，名字和值都是 UTF-8
     * @remarks Windows 走 GetEnvironmentVariableW，避免 getenv 按 ANSI 代码页截断非 ASCII 路径；
     *          POSIX 走 getenv，按 UTF-8 解释字节。不依赖 Platform 单例。
     */
    class T3D_PLATFORM_API Environment
    {
    public:
        /**
         * @brief   读取环境变量
         * @return  不存在返回空串；需要区分「未设置」与「设成空」时用 has()
         */
        static String get(const String &name);

        static bool has(const String &name);
    };
}


#endif  /*__T3D_ENVIRONMENT_H__*/
