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


namespace Tiny3D
{
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
        T3D_ASSERT(getState() == State::kUnloaded, "Resource has not unloaded !");
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
    }

    //--------------------------------------------------------------------------
}
