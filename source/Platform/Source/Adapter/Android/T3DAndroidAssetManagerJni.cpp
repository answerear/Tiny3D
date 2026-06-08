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

#include "Adapter/Android/T3DAndroidAssetManager.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <sys/stat.h>
#include <cstdio>
#include <string>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void *AndroidAssetManager::sNativeAssetManager = nullptr;

    //--------------------------------------------------------------------------

    AndroidAssetManager::AndroidAssetManager()
    {
    }

    //--------------------------------------------------------------------------

    AndroidAssetManager::~AndroidAssetManager()
    {
    }

    //--------------------------------------------------------------------------

    void AndroidAssetManager::setNativeAssetManager(void *mgr)
    {
        sNativeAssetManager = mgr;
    }

    //--------------------------------------------------------------------------

    void* AndroidAssetManager::getNativeHandle() const
    {
        return sNativeAssetManager;
    }

    //--------------------------------------------------------------------------

    bool AndroidAssetManager::extractToPath(const String &assetSubDir, const String &destDir)
    {
        AAssetManager *mgr = static_cast<AAssetManager*>(sNativeAssetManager);
        if (mgr == nullptr)
            return false;

        mkdir(destDir.c_str(), 0755);

        AAssetDir *dir = AAssetManager_openDir(mgr, assetSubDir.c_str());
        if (dir == nullptr)
            return false;

        const char *filename = nullptr;
        while ((filename = AAssetDir_getNextFileName(dir)) != nullptr)
        {
            std::string assetPath = assetSubDir + "/" + filename;
            std::string destPath = destDir + "/" + filename;

            AAsset *asset = AAssetManager_open(mgr, assetPath.c_str(),
                                               AASSET_MODE_STREAMING);
            if (asset == nullptr)
                continue;

            off_t assetSize = AAsset_getLength(asset);

            struct stat st;
            if (stat(destPath.c_str(), &st) == 0 && st.st_size == assetSize)
            {
                AAsset_close(asset);
                continue;
            }

            FILE *fp = fopen(destPath.c_str(), "wb");
            if (fp == nullptr)
            {
                AAsset_close(asset);
                continue;
            }

            const int kBufSize = 64 * 1024;
            char buf[kBufSize];
            int bytesRead = 0;
            while ((bytesRead = AAsset_read(asset, buf, kBufSize)) > 0)
            {
                fwrite(buf, 1, bytesRead, fp);
            }

            fclose(fp);
            AAsset_close(asset);
        }

        AAssetDir_close(dir);
        return true;
    }
}


//------------------------------------------------------------------------------

extern "C" JNIEXPORT void JNICALL
Java_com_tiny3d_lib_Tiny3DGlobal_nativeSetAssetManager(
    JNIEnv *env, jclass /*clazz*/, jobject assetManager)
{
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    Tiny3D::AndroidAssetManager::setNativeAssetManager(mgr);
}
