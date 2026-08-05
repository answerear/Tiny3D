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


#include "Resource/T3DAssetManager.h"
#include "Kernel/T3DCompositeArchive.h"
#include "Resource/T3DMeshManager.h"
#include "Resource/T3DTextureManager.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DShaderManager.h"
#include "Resource/T3DPrefabManager.h"
#include "Resource/T3DSceneManager.h"
#include "Resource/T3DImageManager.h"
#include "Resource/T3DScene.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AssetManagerPtr AssetManager::create()
    {
        return T3D_NEW AssetManager();
    }

    //--------------------------------------------------------------------------

    AssetManager::~AssetManager()
    {
        if (mArchive != nullptr)
        {
            mArchive->unmountAll();
            mArchive = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void AssetManager::ensureArchive()
    {
        if (mArchive == nullptr)
        {
            // 编辑器允许写回工程资源，运行时是只读搜索链
            Archive::AccessMode mode = (mMode == Mode::kEditor)
                ? Archive::AccessMode::kReadTruncate
                : Archive::AccessMode::kReadOnly;
            mArchive = CompositeArchive::create("AssetManager", mode);
        }
    }

    //--------------------------------------------------------------------------

    void AssetManager::init(Mode mode)
    {
        mMode = mode;
        ensureArchive();
    }

    //--------------------------------------------------------------------------

    void AssetManager::mount(Archive *archive, int32_t priority)
    {
        ensureArchive();
        mArchive->mount(archive, priority);
    }

    //--------------------------------------------------------------------------

    void AssetManager::unmount(Archive *archive)
    {
        if (mArchive != nullptr)
        {
            mArchive->unmount(archive);
        }
    }

    //--------------------------------------------------------------------------

    void AssetManager::unmountAll()
    {
        if (mArchive != nullptr)
        {
            mArchive->unmountAll();
        }
    }

    //--------------------------------------------------------------------------

    Archive *AssetManager::getArchive() const
    {
        return mArchive.get();
    }

    //--------------------------------------------------------------------------

    MeshPtr AssetManager::loadMesh(const String &filename)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_MESH_MGR.loadMesh(archive, filename);
    }

    //--------------------------------------------------------------------------

    MeshPtr AssetManager::loadMesh(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_MESH_MGR.loadMesh(archive, uuid);
    }

    //--------------------------------------------------------------------------

    TexturePtr AssetManager::loadTexture(const String &filename)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_TEXTURE_MGR.loadTexture(archive, filename);
    }

    //--------------------------------------------------------------------------

    TexturePtr AssetManager::loadTexture(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_TEXTURE_MGR.loadTexture(archive, uuid);
    }

    //--------------------------------------------------------------------------

    MaterialPtr AssetManager::loadMaterial(const String &filename)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_MATERIAL_MGR.loadMaterial(archive, filename);
    }

    //--------------------------------------------------------------------------

    MaterialPtr AssetManager::loadMaterial(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_MATERIAL_MGR.loadMaterial(archive, uuid);
    }

    //--------------------------------------------------------------------------

    ShaderPtr AssetManager::loadShader(const String &filename)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_SHADER_MGR.loadShader(archive, filename);
    }

    //--------------------------------------------------------------------------

    ShaderPtr AssetManager::loadShader(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_SHADER_MGR.loadShader(archive, uuid);
    }

    //--------------------------------------------------------------------------

    ImagePtr AssetManager::loadImage(const String &filename)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_IMAGE_MGR.loadImage(archive, filename);
    }

    //--------------------------------------------------------------------------

    ImagePtr AssetManager::loadImage(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_IMAGE_MGR.loadImage(archive, uuid);
    }

    //--------------------------------------------------------------------------

    PrefabPtr AssetManager::createPrefab(const String &name, GameObjectPtr root)
    {
        return T3D_PREFAB_MGR.createPrefab(name, root);
    }

    //--------------------------------------------------------------------------

    PrefabPtr AssetManager::loadPrefab(const String &name)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_PREFAB_MGR.loadPrefab(archive, name);
    }

    //--------------------------------------------------------------------------

    PrefabPtr AssetManager::loadPrefab(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_PREFAB_MGR.loadPrefab(archive, uuid);
    }

    //--------------------------------------------------------------------------

    TResult AssetManager::savePrefab(const String &name, Prefab *prefab)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_PREFAB_MGR.savePrefab(archive, name, prefab);
    }

    //--------------------------------------------------------------------------

    ScenePtr AssetManager::loadScene(const String &name)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_SCENE_MGR.loadScene(archive, name);
    }

    //--------------------------------------------------------------------------

    ScenePtr AssetManager::loadScene(const UUID &uuid)
    {
        ensureArchive();
        Archive *archive = mArchive.get();
        return T3D_SCENE_MGR.loadScene(archive, uuid);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(AssetManager)
}
