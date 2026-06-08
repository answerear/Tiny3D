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


#include "T3DAssetDataStream.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AssetDataStream::AssetDataStream()
    {
    }

    //--------------------------------------------------------------------------

    AssetDataStream::~AssetDataStream()
    {
    }

    //--------------------------------------------------------------------------

    bool AssetDataStream::open(AAssetManager *mgr, const char *path, int mode)
    {
        if (mAsset)
        {
            close();
        }

        mAsset = AAssetManager_open(mgr, path, mode);
        if (mAsset)
        {
            mSize = AAsset_getLength(mAsset);
            return true;
        }

        mSize = 0;
        return false;
    }

    //--------------------------------------------------------------------------

    void AssetDataStream::close()
    {
        if (mAsset)
        {
            AAsset_close(mAsset);
            mAsset = nullptr;
            mSize = 0;
        }
    }

    //--------------------------------------------------------------------------

    bool AssetDataStream::isOpen() const
    {
        return mAsset != nullptr;
    }

    //--------------------------------------------------------------------------

    size_t AssetDataStream::read(void *pBuffer, size_t nSize)
    {
        if (!mAsset) return 0;
        int bytesRead = AAsset_read(mAsset, pBuffer, nSize);
        return (bytesRead < 0) ? 0 : static_cast<size_t>(bytesRead);
    }

    //--------------------------------------------------------------------------

    size_t AssetDataStream::write(void *pBuffer, size_t nSize)
    {
        T3D_ASSERT(false && "AssetDataStream does not support write operations!");
        return 0;
    }

    //--------------------------------------------------------------------------

    bool AssetDataStream::seek(long_t lPos, bool relative)
    {
        if (!mAsset) return false;
        off_t result = AAsset_seek(mAsset, lPos, relative ? SEEK_CUR : SEEK_SET);
        return result != (off_t)-1;
    }

    //--------------------------------------------------------------------------

    long_t AssetDataStream::tell() const
    {
        if (!mAsset) return 0;
        return mSize - AAsset_getRemainingLength(mAsset);
    }

    //--------------------------------------------------------------------------

    long_t AssetDataStream::size() const
    {
        return mSize;
    }

    //--------------------------------------------------------------------------

    bool AssetDataStream::eof() const
    {
        if (!mAsset) return true;
        return AAsset_getRemainingLength(mAsset) <= 0;
    }

    //--------------------------------------------------------------------------

    size_t AssetDataStream::read(uint8_t *&pData)
    {
        if (!mAsset) return 0;

        // 尝试获取 mmap 缓冲区（对未压缩资源零拷贝）
        const void *buffer = AAsset_getBuffer(mAsset);
        if (buffer)
        {
            pData = const_cast<uint8_t *>(static_cast<const uint8_t *>(buffer));
            return static_cast<size_t>(mSize);
        }

        // 压缩资源回退：分配内存后全量读取
        pData = T3D_POD_NEW_ARRAY(uint8_t, mSize);
        AAsset_seek(mAsset, 0, SEEK_SET);
        int bytesRead = AAsset_read(mAsset, pData, static_cast<size_t>(mSize));
        return (bytesRead < 0) ? 0 : static_cast<size_t>(bytesRead);
    }

    //--------------------------------------------------------------------------
}
