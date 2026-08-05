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


#include "Component/T3DSkybox.h"
#include "Kernel/T3DArchive.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkyboxPtr Skybox::create()
    {
        return T3D_NEW Skybox(UUID::generate());
    }

    //--------------------------------------------------------------------------

    Skybox::Skybox(const UUID &uuid)
        : Component(uuid)
    {
        
    }

    //--------------------------------------------------------------------------

    Skybox::~Skybox()
    {
        
    }

    //--------------------------------------------------------------------------

    ComponentPtr Skybox::clone() const
    {
        SkyboxPtr skybox = T3D_NEW Skybox();
        TResult ret = skybox->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            skybox = nullptr;
        }
        return skybox;
    }

    //--------------------------------------------------------------------------

    TResult Skybox::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Component::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const Skybox * const other = static_cast<const Skybox * const>(src);
            mMaterialUUID = other->mMaterialUUID;
            mMaterial = other->mMaterial;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void Skybox::setMaterialUUID(const UUID &uuid)
    {
        if (mMaterialUUID != uuid)
        {
            mMaterialUUID = uuid;
            // 换 UUID 后已缓存的材质对象就失效了，等下一次 onLoadResource 再取
            mMaterial = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void Skybox::setMaterial(Material *material)
    {
        mMaterial = material;
        mMaterialUUID = (material != nullptr ? material->getUUID() : UUID::INVALID);
    }

    //--------------------------------------------------------------------------

    void Skybox::onLoadResource(Archive *archive)
    {
        Component::onLoadResource(archive);

        if (mMaterial != nullptr && mMaterial->getUUID() != mMaterialUUID)
        {
            mMaterial = nullptr;
        }

        if (mMaterial == nullptr && mMaterialUUID != UUID::INVALID)
        {
            mMaterial = T3D_MATERIAL_MGR.loadMaterial(archive, mMaterialUUID);

            if (mMaterial == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_COMPONENT, "Failed to load skybox material (%s) !",
                    mMaterialUUID.toString().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void Skybox::onDestroy()
    {
        mMaterial = nullptr;

        Component::onDestroy();
    }

    //--------------------------------------------------------------------------
}
