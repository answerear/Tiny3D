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


#include "T3DAndroidAssetArchive.h"
#include "T3DAssetDataStream.h"
#include <Asset/T3DZipAssetManager.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const AndroidAssetArchive::ARCHIVE_TYPE = "AndroidAsset";

    //--------------------------------------------------------------------------

    AndroidAssetArchivePtr AndroidAssetArchive::create(const String &name, AccessMode mode)
    {
        AndroidAssetArchivePtr archive = T3D_NEW AndroidAssetArchive(name, mode);
        return archive;
    }

    //--------------------------------------------------------------------------

    AndroidAssetArchive::AndroidAssetArchive(const String &name, AccessMode mode)
        : Archive(name, mode)
        , mAssetMgr(static_cast<AAssetManager*>(T3D_ZIP_ASSET_MGR.getNativeHandle()))
    {
    }

    //--------------------------------------------------------------------------

    AndroidAssetArchive::~AndroidAssetArchive()
    {
    }

    //--------------------------------------------------------------------------

    String AndroidAssetArchive::getArchiveType() const
    {
        return ARCHIVE_TYPE;
    }

    //--------------------------------------------------------------------------

    String AndroidAssetArchive::getPath() const
    {
        return getName();
    }

    //--------------------------------------------------------------------------

    ArchivePtr AndroidAssetArchive::clone() const
    {
        ArchivePtr archive = create(getName(), getAccessMode());
        return archive;
    }

    //--------------------------------------------------------------------------

    bool AndroidAssetArchive::exists(const String &name) const
    {
        String fullPath = getPath().empty() ? name : (getPath() + "/" + name);
        AAsset *asset = AAssetManager_open(mAssetMgr, fullPath.c_str(), AASSET_MODE_UNKNOWN);
        if (asset)
        {
            AAsset_close(asset);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    TResult AndroidAssetArchive::read(const String &name,
                                       const ArchiveReadCallback &callback,
                                       void *userData)
    {
        TResult ret = T3D_OK;

        do
        {
            if (callback == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_ANDROID_ASSET,
                    "Read callback is nullptr when reading archive [%s] !",
                    name.c_str());
                break;
            }

            String fullPath = getPath().empty() ? name : (getPath() + "/" + name);
            AssetDataStream stream;
            if (!stream.open(mAssetMgr, fullPath.c_str(), AASSET_MODE_STREAMING))
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                T3D_LOG_ERROR(LOG_TAG_ANDROID_ASSET,
                    "Open asset [%s] failed !", fullPath.c_str());
                break;
            }

            ret = callback(stream, fullPath, userData);

            stream.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult AndroidAssetArchive::write(const String &name,
                                        const ArchiveWriteCallback &callback,
                                        void *userData)
    {
        T3D_ASSERT(false && "AndroidAssetArchive does not support write operations!");
        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    TResult AndroidAssetArchive::read(const UUID &uuid,
                                       const ArchiveReadCallback &callback,
                                       void *userData)
    {
        T3D_ASSERT(false && "AndroidAssetArchive does not support read by UUID!");
        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    TResult AndroidAssetArchive::write(const UUID &uuid,
                                        const ArchiveWriteCallback &callback,
                                        void *userData)
    {
        T3D_ASSERT(false && "AndroidAssetArchive does not support write operations!");
        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------
}
