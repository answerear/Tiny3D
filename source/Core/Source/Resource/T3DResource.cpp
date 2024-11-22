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


#include "Resource/T3DResource.h"
#include "Kernel/T3DArchive.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *Resource::EXT_MATERIAL = "tmat";
    const char *Resource::EXT_TEXTURE = "ttex";
    const char *Resource::EXT_IMAGE = "timg";
    const char *Resource::EXT_SHADER = "tshader";
    const char *Resource::EXT_MESH = "tmesh";
    const char *Resource::EXT_PREFAB = "tprefab";
    const char *Resource::EXT_SCENE = "tscene";
    const char *Resource::EXT_TXT = "txt";
    const char *Resource::EXT_BIN = "bin";
    const char *Resource::EXT_SHADERLAB = "shader";

#if defined (T3D_OS_WINDOWS)
    const char *Resource::EXT_DYLIB = "dll";
#elif defined (T3D_OS_OSX) || defined (T3D_OS_IOS)
    const char *Resource::EXT_DYLIB = "dylib";
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_ANDROID)
    const char *Resource::EXT_DYLIB = "so";
#endif
    
    //--------------------------------------------------------------------------

    Resource::Resource()
        : Resource("")
    {
    }
    
    //--------------------------------------------------------------------------

    Resource::Resource(const String &strName)
        : mState(State::kUnloaded)
        , mName(strName)
        , mCompletedCB(nullptr)
    {
        mUUID = UUID::generate();
    }

    //--------------------------------------------------------------------------

    Resource::~Resource()
    {
        T3D_ASSERT(getState() == State::kUnloaded);
    }

    //--------------------------------------------------------------------------

    TResult Resource::onCreate()
    {
        mState = State::kLoaded;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Resource::onSave(Archive *archive)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Resource::onLoad(Archive *archive)
    {
        mState = State::kLoaded;

        for (auto component : mNeedToLoadResourceComponents)
        {
            component->onLoadResource(archive);
        }

        mNeedToLoadResourceComponents.clear();
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Resource::onUnload()
    {
        mState = State::kUnloaded;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Resource::cloneProperties(const Resource *const src)
    {
        // 克隆就新生成 UUID
        mUUID = UUID::generate();
        mName = src->getName();
        mFilename = src->getFilename();
        mState = src->mState;
        mCompletedCB = src->mCompletedCB;
    }

    //--------------------------------------------------------------------------

    void Resource::onAddComponentForLoadingResource(Component *component)
    {
        
    }

    //--------------------------------------------------------------------------
}
