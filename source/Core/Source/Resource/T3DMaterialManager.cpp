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
        return new MaterialManager();
    }
    
    //--------------------------------------------------------------------------

    MaterialPtr MaterialManager::loadMaterial(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Material>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveMaterial(Archive *archive, Material *material)
    {
        return save(material, archive);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Material::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
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
}

