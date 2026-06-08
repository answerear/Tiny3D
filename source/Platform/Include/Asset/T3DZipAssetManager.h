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

#ifndef __T3D_ZIP_ASSET_MANAGER_H__
#define __T3D_ZIP_ASSET_MANAGER_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class IZipAssetManager;

    class T3D_PLATFORM_API ZipAssetManager : public Allocator, public Singleton<ZipAssetManager>
    {
    public:
        ZipAssetManager();
        virtual ~ZipAssetManager();

        /**
         * @brief 获取平台原生资源管理器句柄
         */
        void* getNativeHandle() const;

        /**
         * @brief 将打包资源中的文件提取到目标路径
         */
        bool extractToPath(const String &assetSubDir, const String &destDir);

    protected:
        IZipAssetManager *mImpl {nullptr};
    };

    #define T3D_ZIP_ASSET_MGR   ZipAssetManager::getInstance()

    T3D_EXTERN_SINGLETON(ZipAssetManager)
}


#endif  /*__T3D_ZIP_ASSET_MANAGER_H__*/
