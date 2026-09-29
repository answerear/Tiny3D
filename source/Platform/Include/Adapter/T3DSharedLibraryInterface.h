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

#ifndef __T3D_SHARED_LIBRARY_INTERFACE_H__
#define __T3D_SHARED_LIBRARY_INTERFACE_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    /**
     * @class   ISharedLibrary
     * @brief   动态库加载适配器接口
     * @remarks 只做系统调用，不拼 lib 前缀 / 扩展名，也不做目录搜索。
     *          flags 的合法性由门面 SharedLibrary 校验后才传到这里。
     */
    class ISharedLibrary : public Allocator
    {
        T3D_DECLARE_INTERFACE(ISharedLibrary);

    public:
        /**
         * @brief   打开动态库
         * @param [in] path : 原样交给系统，可以是绝对路径、相对路径或裸文件名
         * @param [in] flags : SharedLibraryFlags 组合
         * @return  成功返回 T3D_OK，失败返回 T3D_ERR_SHAREDLIB_OPEN，原因见 getLastError()
         */
        virtual TResult open(const String &path, uint32_t flags) = 0;

        /**
         * @brief   关闭动态库。未打开或已关闭则空操作
         */
        virtual void close() = 0;

        virtual bool isOpen() const = 0;

        /**
         * @brief   按导出符号名取地址
         * @return  未打开或找不到返回 nullptr
         */
        virtual void *getSymbol(const String &name) const = 0;

        /**
         * @brief   最近一次失败的平台原文（Windows FormatMessage / POSIX dlerror）
         */
        virtual String getLastError() const = 0;

        virtual THandle getNativeHandle() const = 0;
    };
}


#endif  /*__T3D_SHARED_LIBRARY_INTERFACE_H__*/
