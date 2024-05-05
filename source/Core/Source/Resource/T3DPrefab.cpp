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

#include "Resource/T3DPrefab.h"

#include "Component/T3DComponent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PrefabPtr Prefab::create(const String &name)
    {
        return new Prefab(name);
    }

    //--------------------------------------------------------------------------

    Prefab::~Prefab()
    {
        T3D_SAFE_DELETE(mObject);
    }

    //--------------------------------------------------------------------------

    Prefab::Prefab(const String &name)
        : Resource(name)
        , mObject(nullptr)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Prefab::getType() const
    {
        return Type::kPrefab;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Prefab::clone() const
    {
        PrefabPtr prefab = create(getName());
        prefab->cloneProperties(this);
        return prefab;
    }
    
    //--------------------------------------------------------------------------

    void Prefab::cloneProperties(const Resource *const src)
    {
        const Prefab *prefab = static_cast<const Prefab*>(src);
        mObject = new RTTRObject(*prefab->mObject);
    }

    //--------------------------------------------------------------------------

    TResult Prefab::onLoad(Archive *archive)
    {
        TResult ret = Resource::onLoad(archive);

        if (T3D_SUCCEEDED(ret))
        {
            for (auto component : mNeedToLoadResourceComponents)
            {
                component->onLoadResource(archive);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Prefab::addCompnentForLoadingResource(Component *component)
    {
        mNeedToLoadResourceComponents.emplace(component);
    }

    //--------------------------------------------------------------------------
}
