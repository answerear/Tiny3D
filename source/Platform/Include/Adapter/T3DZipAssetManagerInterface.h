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

#ifndef __T3D_ZIP_ASSET_MANAGER_INTERFACE_H__
#define __T3D_ZIP_ASSET_MANAGER_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    /**
     * @class IZipAssetManager
     * @brief 平台打包资源管理器接口
     * @note 不同操作系统平台需要继承该接口实现具体平台相关功能
     */
    class IZipAssetManager : public Allocator
    {
        T3D_DECLARE_INTERFACE(IZipAssetManager);

    public:
        /**
         * @brief 获取平台原生资源管理器句柄
         * @return Android 返回 AAssetManager*，其他平台返回 nullptr
         */
        virtual void* getNativeHandle() const = 0;

        /**
         * @brief 将打包资源中指定子目录下的文件提取到目标文件系统路径
         * @param[in] assetSubDir  包内资源子目录（如 "plugins"）
         * @param[in] destDir      目标绝对路径
         * @return 成功返回 true
         */
        virtual bool extractToPath(const String &assetSubDir, const String &destDir) = 0;
    };
}


#endif  /*__T3D_ZIP_ASSET_MANAGER_INTERFACE_H__*/
