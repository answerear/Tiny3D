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

#ifndef __T3D_SHARED_LIBRARY_H__
#define __T3D_SHARED_LIBRARY_H__


#include "T3DMacro.h"
#include "T3DNoncopyable.h"
#include "T3DPlatformPrerequisites.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class ISharedLibrary;

    /**
     * @brief   动态库加载标志
     * @remarks Now 与 Lazy 互斥，Local 与 Global 互斥，非法组合 open 直接失败。
     *          Windows 下这些位全部忽略：LoadLibrary 的行为已经等价于 Now|Local。
     *          两个互斥对都不给时，使用平台默认（POSIX 缺省为 RTLD_NOW，
     *          可见性按系统缺省：Linux/Android 为 LOCAL，macOS/iOS 为 GLOBAL）。
     */
    enum SharedLibraryFlags : uint32_t
    {
        /// 未定义符号在加载时立即解析（POSIX RTLD_NOW）
        kSharedLibraryNow       = 1u << 0,
        /// 符号用到再解析（POSIX RTLD_LAZY）
        kSharedLibraryLazy      = 1u << 1,
        /// 符号不进入全局符号表（POSIX RTLD_LOCAL）
        kSharedLibraryLocal     = 1u << 2,
        /// 符号进入全局符号表（POSIX RTLD_GLOBAL）
        kSharedLibraryGlobal    = 1u << 3,
    };

    /// 引擎插件默认：立刻解析
    constexpr uint32_t kSharedLibraryPlugin = kSharedLibraryNow;

    /// 第三方编译器 / 带私有 LLVM 的库：惰性 + 本地，避免符号泄漏到全局
    constexpr uint32_t kSharedLibraryIsolated = kSharedLibraryLazy | kSharedLibraryLocal;

    /**
     * @class   SharedLibrary
     * @brief   平台无关的动态库加载门面
     * @remarks open 只做系统调用，不拼文件名、不搜目录。拼文件名用 makeFileName，
     *          拼目录用 Dir。候选路径策略属于调用方。
     */
    class T3D_PLATFORM_API SharedLibrary : public Allocator, public Noncopyable
    {
    public:
        SharedLibrary();

        /// 若仍处于打开状态则 close()
        ~SharedLibrary() override;

        /**
         * @brief   打开动态库
         * @param [in] path : 原样交给系统，可以是绝对路径、相对路径或裸文件名
         * @param [in] flags : SharedLibraryFlags 组合
         * @return  成功返回 T3D_OK；flags 非法返回 T3D_ERR_SHAREDLIB_FLAGS；
         *          系统加载失败返回 T3D_ERR_SHAREDLIB_OPEN，原因见 getLastError()
         * @remarks 已打开时会先关闭之前的库
         */
        TResult open(const String &path, uint32_t flags = kSharedLibraryPlugin);

        void close();

        bool isOpen() const;

        /**
         * @brief   按导出符号名取地址
         * @return  未打开或找不到返回 nullptr，找不到时原因见 getLastError()
         */
        void *getSymbol(const String &name) const;

        /**
         * @brief   最近一次失败的平台原文（Windows FormatMessage / POSIX dlerror）
         */
        String getLastError() const;

        THandle getNativeHandle() const;

        /**
         * @brief   逻辑名转平台文件名，不含目录
         * @remarks "dxcompiler" → "dxcompiler.dll" / "libdxcompiler.so" / "libdxcompiler.dylib"
         */
        static String makeFileName(const String &logicalName);

        /**
         * @brief   反查 address 所属模块的绝对路径
         * @remarks 传入本模块内任意静态函数地址即可得到本 .dll / .so / .dylib 的路径；
         *          静态链进 exe 时得到的是 exe 路径。
         * @return  失败返回空串
         */
        static String getModulePath(const void *address);

        /**
         * @brief   getModulePath + Dir::parsePath，只返回目录，末尾不带分隔符
         */
        static String getModuleDir(const void *address);

        /**
         * @brief   address 是否仍映射在某个已加载模块里
         * @remarks Platform 未创建时保守返回 false
         */
        static bool isAddressMapped(const void *address);

    protected:
        ISharedLibrary  *mLib {nullptr};
        String          mLastError {};
    };
}


#endif  /*__T3D_SHARED_LIBRARY_H__*/
