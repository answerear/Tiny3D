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


#include "Resource/T3DDylib.h"
#include "Kernel/T3DAgent.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DylibPtr Dylib::create(const String &name, const String &searchPath)
    {
        DylibPtr dylib = T3D_NEW Dylib(name, searchPath);
        // dylib->release();
        return dylib;
    }

    //--------------------------------------------------------------------------

    Dylib::Dylib(const String &name, const String &searchPath)
        : Resource(name)
        , mSearchPath(searchPath)
    {
        mUUID = UUID::generate();
    }

    //--------------------------------------------------------------------------

    Dylib::~Dylib()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Dylib::getType() const
    {
        return Type::kDylib;
    }

    //--------------------------------------------------------------------------

    void *Dylib::getSymbol(const String &name) const
    {
        return mLib.getSymbol(name);
    }

    //--------------------------------------------------------------------------

    TResult Dylib::onLoad(Archive *archive)
    {
        TResult ret = T3D_OK;

        do 
        {
            mState = State::kLoading;
            
            const String fileName = SharedLibrary::makeFileName(mName);

#if defined (T3D_OS_ANDROID)
            // Android 的插件随 APK 部署在 JNI 库目录，只给文件名交给系统 loader 搜索
            const String &path = fileName;
#else
            const String &pluginsPath = mSearchPath.empty()
                ? Agent::getInstance().getPluginsPath() : mSearchPath;
            const String path = pluginsPath + Dir::getNativeSeparator() + fileName;
#endif

            if (T3D_FAILED(mLib.open(path, kSharedLibraryPlugin)))
            {
                ret = T3D_ERR_PLG_LOAD_FAILED;
                T3D_LOG_ERROR(LOG_TAG_PLUGIN, "Load plugin failed ! Desc : %s",
                    mLib.getLastError().c_str());
                mState = State::kUnloaded;
                break;
            }

            ret = Resource::onLoad(archive);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Dylib::onUnload()
    {
        if (getState() == State::kLoaded)
        {
            mLib.close();
        }

        return Resource::onUnload();
    }

    //--------------------------------------------------------------------------

    ResourcePtr Dylib::clone() const
    {
        DylibPtr dylib = create(getName(), mSearchPath);
        dylib->cloneProperties(this);
        return dylib;
    }
}

