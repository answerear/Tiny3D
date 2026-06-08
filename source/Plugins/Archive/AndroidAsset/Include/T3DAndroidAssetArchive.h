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


#ifndef __T3D_ANDROID_ASSET_ARCHIVE_H__
#define __T3D_ANDROID_ASSET_ARCHIVE_H__


#include "T3DAndroidAssetArchivePrerequisites.h"
#include <android/asset_manager.h>


namespace Tiny3D
{
    class AndroidAssetArchive : public Archive
    {
    public:
        static const char * const ARCHIVE_TYPE;

        static AndroidAssetArchivePtr create(const String &name, AccessMode mode);

        ~AndroidAssetArchive() override;

        String getArchiveType() const override;

        String getPath() const override;

        ArchivePtr clone() const override;

        bool exists(const String &name) const override;

        TResult read(const String &name,
                     const ArchiveReadCallback &callback, void *userData) override;

        TResult write(const String &name,
                      const ArchiveWriteCallback &callback, void *userData) override;

        TResult read(const UUID &uuid,
                     const ArchiveReadCallback &callback, void *userData) override;

        TResult write(const UUID &uuid,
                      const ArchiveWriteCallback &callback, void *userData) override;

    protected:
        AndroidAssetArchive(const String &name, AccessMode mode);

    private:
        AAssetManager *mAssetMgr {nullptr};
    };
}


#endif  /*__T3D_ANDROID_ASSET_ARCHIVE_H__*/
