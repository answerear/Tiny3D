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


#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMaterial.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    MaterialManagerPtr MaterialManager::create()
    {
        return T3D_NEW MaterialManager();
    }
    
    //--------------------------------------------------------------------------

    MaterialPtr MaterialManager::createMaterial(const String &name,  Shader *shader, const UUID &uuid/* = UUID::INVALID*/)
    {
#if defined (T3D_EDITOR)
        // 编辑器/工具环境下允许指定 UUID（用于保留已有资源 guid），创建时即设好
        return smart_pointer_cast<Material>(createResource(name, 2, shader, &uuid));
#else
        return smart_pointer_cast<Material>(createResource(name, 1, shader));
#endif
    }

    //--------------------------------------------------------------------------

    MaterialPtr MaterialManager::loadMaterial(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Material>(load(archive, name));
    }

    //--------------------------------------------------------------------------

    MaterialPtr MaterialManager::loadMaterial(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<Material>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveMaterial(Archive *archive, const String &filename, Material *material)
    {
        return save(archive, filename, material);
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveMaterial(Archive *archive, Material *material)
    {
        return save(archive, material);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::newResource(const String &name, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 1 || argc == 2);
        Shader *shader = va_arg(args, Shader*);
        ResourcePtr material = Material::create(name, shader);
#if defined (T3D_EDITOR)
        if (argc == 2)
        {
            const UUID *uuid = va_arg(args, const UUID*);
            applyCreationUUID(material, uuid != nullptr ? *uuid : UUID::INVALID);
        }
#endif
        return material;
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::loadResource(const String &name, DataStream &stream)
    {
        return loadResource(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Material>(stream);
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveResource(DataStream &stream, Resource *res)
    {
        Material *material = static_cast<Material*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, material);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(MaterialManager)
}

