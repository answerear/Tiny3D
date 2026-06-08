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


#ifndef __T3D_ASSET_DATA_STREAM_H__
#define __T3D_ASSET_DATA_STREAM_H__


#include "T3DAndroidAssetArchivePrerequisites.h"
#include <android/asset_manager.h>


namespace Tiny3D
{
    class AssetDataStream : public DataStream
    {
    public:
        AssetDataStream();
        ~AssetDataStream() override;

        bool open(AAssetManager *mgr, const char *path,
                  int mode = AASSET_MODE_STREAMING);
        void close();

        bool isOpen() const;

        size_t read(void *pBuffer, size_t nSize) override;
        size_t write(void *pBuffer, size_t nSize) override;
        bool seek(long_t lPos, bool relative) override;
        long_t tell() const override;
        long_t size() const override;
        bool eof() const override;
        size_t read(uint8_t *&pData) override;

    private:
        AAsset *mAsset {nullptr};
        long_t  mSize  {0};
    };
}


#endif  /*__T3D_ASSET_DATA_STREAM_H__*/
