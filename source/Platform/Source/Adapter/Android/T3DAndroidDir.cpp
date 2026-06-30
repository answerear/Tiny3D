/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "Adapter/Android/T3DAndroidDir.h"
#include <sys/stat.h>
#include <unistd.h>
#include <android/asset_manager.h>

#include <SDL.h>
#include "Adapter/Android/T3DJniApi.h"
#include "Asset/T3DZipAssetManager.h"
#include "IO/T3DMemoryDataStream.h"

namespace Tiny3D
{
    AndroidDir::AndroidDir()
        : mApkPathInitialized(false)
    {
    }

    void AndroidDir::ensureApkPath() const
    {
        if (mApkPathInitialized)
            return;

        mApkPathInitialized = true;

        JNIEnv *pEnv = (JNIEnv *)SDL_AndroidGetJNIEnv();
        if (pEnv != nullptr)
        {
            JNICallParam param;

            if (GetClassStaticMethodID(pEnv, param, "com/tiny3d/lib/Tiny3DGlobal",
                                       "GetApkPath", "()Ljava/lang/String;"))
            {
                jstring jstr = (jstring) pEnv->CallStaticObjectMethod(param.classID,
                                                                      param.methodID);
                if (pEnv->ExceptionCheck())
                {
                    pEnv->ExceptionClear();
                    jstr = nullptr;
                }

                if (jstr != nullptr)
                {
                    jboolean isCopy;
                    const char *apkPath = pEnv->GetStringUTFChars(jstr, &isCopy);

                    if (apkPath != nullptr)
                    {
                        mApkPath = apkPath;
                        pEnv->ReleaseStringUTFChars(jstr, apkPath);
                    }

                    pEnv->DeleteLocalRef(jstr);
                }
            }
            DeleteLocalRef(pEnv, param);
        }
    }

    AndroidDir::~AndroidDir()
    {
        
    }

    long_t AndroidDir::getCreationTime() const
    {
        long_t timestamp = 0;
        
        do
        {
            struct stat s;
            String strPath = m_strRoot + String(m_pDirent->d_name);
            int result = stat(strPath.c_str(), &s);
            if (result != 0)
                break;
            timestamp = (long_t)s.st_ctime;
        } while (0);
        
        return timestamp;
    }
    
    long_t AndroidDir::getLastAccessTime() const
    {
        long_t timestamp = 0;
        
        do
        {
            struct stat s;
            String strPath = m_strRoot + String(m_pDirent->d_name);
            int result = stat(strPath.c_str(), &s);
            if (result != 0)
                break;
            timestamp = (long_t)s.st_atime;
        } while (0);
        
        
        return timestamp;
    }
    
    long_t AndroidDir::getLastWriteTime() const
    {
        long_t timestamp = 0;
        
        do
        {
            struct stat s;
            String strPath = m_strRoot + String(m_pDirent->d_name);
            int result = stat(strPath.c_str(), &s);
            if (result != 0)
                break;
            timestamp = (long_t)s.st_mtime;
        } while (0);
        
        return timestamp;
    }
    
    String AndroidDir::getCachePath() const
    {
        String internalPath = SDL_AndroidGetInternalStoragePath();
        size_t pos = internalPath.rfind("/");
        String appPath = internalPath.substr(0, pos);
        return appPath + "/cache";
    }
    
    String AndroidDir::getAppPath() const
    {
        ensureApkPath();
        return mApkPath;
    }
    
    String AndroidDir::getWritablePath() const
    {
        String externalPath = SDL_AndroidGetInternalStoragePath();
        return externalPath;
    }

    String AndroidDir::getLibraryPath() const
    {
        String internalPath = SDL_AndroidGetInternalStoragePath();
        size_t pos = internalPath.rfind("/");
        String appPath = internalPath.substr(0, pos);
        return appPath + "/lib";
    }

    String AndroidDir::getResourcePath(const String &logicalPath) const
    {
        // Android：资源位于 APK assets 根下，逻辑路径即 assets 相对路径
        String result = logicalPath;

        // 统一分隔符为 '/'
        for (auto &ch : result)
        {
            if (ch == '\\')
                ch = '/';
        }

        // 去掉前导 "./"
        while (result.size() >= 2 && result[0] == '.' && result[1] == '/')
            result = result.substr(2);

        // 去掉前导 '/'
        while (!result.empty() && result[0] == '/')
            result = result.substr(1);

        return result;
    }

    DataStream *AndroidDir::openAsset(const String &path) const
    {
        AAssetManager *mgr = static_cast<AAssetManager *>(
            T3D_ZIP_ASSET_MGR.getNativeHandle());
        if (mgr == nullptr)
        {
            return nullptr;
        }

        AAsset *asset = AAssetManager_open(mgr, path.c_str(), AASSET_MODE_BUFFER);
        if (asset == nullptr)
        {
            return nullptr;
        }

        off_t length = AAsset_getLength(asset);
        const void *buffer = AAsset_getBuffer(asset);
        if (buffer == nullptr)
        {
            AAsset_close(asset);
            return nullptr;
        }

        // MemoryDataStream 默认拷贝一份数据，因此可安全关闭 AAsset
        MemoryDataStream *stream = T3D_NEW MemoryDataStream(
            const_cast<uchar_t *>(static_cast<const uchar_t *>(buffer)),
            static_cast<size_t>(length), true);

        AAsset_close(asset);
        return stream;
    }
}

