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


#include "T3DBundleFSArchive.h"

#if defined (T3D_OS_ANDROID)
    #include <android/asset_manager.h>
    #include <Asset/T3DZipAssetManager.h>
#endif


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const BundleFSArchive::ARCHIVE_TYPE = "BundleFileSystem";

    const char * const BundleFSArchive::MANIFEST_NAME = "bundle.manifest";

    //--------------------------------------------------------------------------

    BundleFSArchivePtr BundleFSArchive::create(const String &name, AccessMode mode)
    {
        BundleFSArchivePtr archive = T3D_NEW BundleFSArchive(name, mode);
        if (archive != nullptr)
        {
            // 加载 manifest 失败不影响按 UUID 直读，仅记录告警
            archive->init();
        }
        return archive;
    }

    //--------------------------------------------------------------------------

    BundleFSArchive::BundleFSArchive(const String &name, AccessMode mode)
        : Archive(name, mode)
    {
    }

    //--------------------------------------------------------------------------

    BundleFSArchive::~BundleFSArchive()
    {
    }

    //--------------------------------------------------------------------------

    bool BundleFSArchive::init()
    {
        TResult ret = readRaw(MANIFEST_NAME,
            [this](DataStream &stream, const String &name, void *userData)
            {
                return mManifest.parse(stream) ? T3D_OK : T3D_ERR_FAIL;
            },
            nullptr);

        mManifestLoaded = T3D_SUCCEEDED(ret);
        if (!mManifestLoaded)
        {
            BFS_LOG_WARNING("Load bundle manifest [%s] under [%s] failed, "
                "only read-by-uuid will be available !",
                MANIFEST_NAME, getPath().c_str());
        }
        else
        {
            BFS_LOG_INFO("Bundle manifest loaded, %zu entries under [%s].",
                mManifest.size(), getPath().c_str());
        }

        return mManifestLoaded;
    }

    //--------------------------------------------------------------------------

    String BundleFSArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    //--------------------------------------------------------------------------

    String BundleFSArchive::getPath() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    ArchivePtr BundleFSArchive::clone() const
    {
        ArchivePtr archive = create(getName(), getAccessMode());
        return archive;
    }

    //--------------------------------------------------------------------------

    bool BundleFSArchive::exists(const String &name) const
    {
        // 仅查 manifest，不做原生探测
        return mManifest.exists(name);
    }

    //--------------------------------------------------------------------------

    TResult BundleFSArchive::readRaw(const String &relativeName,
        const ArchiveReadCallback &callback, void *userData)
    {
        if (callback == nullptr)
        {
            BFS_LOG_ERROR("Read callback is nullptr when reading bundle [%s] !",
                relativeName.c_str());
            return T3D_ERR_INVALID_PARAM;
        }

        if (relativeName.empty())
        {
            BFS_LOG_ERROR("Empty relative name when reading bundle !");
            return T3D_ERR_INVALID_PARAM;
        }

#if defined (T3D_OS_ANDROID)
        // Android：直接使用 AAssetManager 原生接口读取 APK 内的散列文件
        AAssetManager *mgr = static_cast<AAssetManager *>(
            T3D_ZIP_ASSET_MGR.getNativeHandle());
        if (mgr == nullptr)
        {
            BFS_LOG_ERROR("Native AAssetManager is null !");
            return T3D_ERR_FAIL;
        }

        String fullPath = getPath().empty()
            ? relativeName : (getPath() + "/" + relativeName);

        AAsset *asset = AAssetManager_open(mgr, fullPath.c_str(), AASSET_MODE_BUFFER);
        if (asset == nullptr)
        {
            return T3D_ERR_FILE_NOT_EXIST;
        }

        off_t length = AAsset_getLength(asset);
        const void *buffer = AAsset_getBuffer(asset);
        if (buffer == nullptr)
        {
            AAsset_close(asset);
            BFS_LOG_ERROR("Get asset buffer failed for [%s] !", fullPath.c_str());
            return T3D_ERR_FAIL;
        }

        // MemoryDataStream 默认拷贝一份数据，因此回调结束后可安全关闭 AAsset
        MemoryDataStream stream(
            const_cast<uchar_t *>(static_cast<const uchar_t *>(buffer)),
            static_cast<size_t>(length), true);

        TResult ret = callback(stream, relativeName, userData);

        AAsset_close(asset);
        return ret;
#elif defined (T3D_OS_DESKTOP)
        // 桌面：直接使用 FileDataStream 原生读取
        FileDataStream fs;
        String path = getPath() + Dir::getNativeSeparator() + relativeName;
        if (!fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            return T3D_ERR_FILE_NOT_EXIST;
        }

        TResult ret = callback(fs, relativeName, userData);

        fs.close();
        return ret;
#else
        // 其它平台：预留扩展点
        BFS_LOG_ERROR("BundleFSArchive native I/O is not implemented on this platform !");
        return T3D_ERR_NOT_IMPLEMENT;
#endif
    }

    //--------------------------------------------------------------------------

    TResult BundleFSArchive::read(const String &name,
        const ArchiveReadCallback &callback, void *userData)
    {
        if (name.empty())
        {
            BFS_LOG_ERROR("Empty filename when reading bundle !");
            return T3D_ERR_INVALID_PARAM;
        }

        UUID uuid;
        if (!mManifest.getUUIDByPath(name, uuid))
        {
            // manifest 未命中，直接返回错误，不回退原生按名读取
            BFS_LOG_ERROR("File [%s] not found in bundle manifest !", name.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        return readRaw(uuid.toString(), callback, userData);
    }

    //--------------------------------------------------------------------------

    TResult BundleFSArchive::write(const String &name,
        const ArchiveWriteCallback &callback, void *userData)
    {
        BFS_LOG_ERROR("BundleFSArchive is read-only, write [%s] is not supported !",
            name.c_str());
        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    TResult BundleFSArchive::read(const UUID &uuid,
        const ArchiveReadCallback &callback, void *userData)
    {
        return readRaw(uuid.toString(), callback, userData);
    }

    //--------------------------------------------------------------------------

    TResult BundleFSArchive::write(const UUID &uuid,
        const ArchiveWriteCallback &callback, void *userData)
    {
        BFS_LOG_ERROR("BundleFSArchive is read-only, write (uuid: %s) is not supported !",
            uuid.toString().c_str());
        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------
}
