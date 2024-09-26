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


#include "Resource/T3DShaderManager.h"
#include "Resource/T3DShader.h"
#include "Kernel/T3DAgent.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderManagerPtr ShaderManager::create()
    {
        return new ShaderManager();
    }

    //--------------------------------------------------------------------------
    
    ShaderPtr ShaderManager::createShader(const String &name)
    {
        return smart_pointer_cast<Shader>(createResource(name, 0));
    }

    //--------------------------------------------------------------------------

#if defined (T3D_EDITOR)
    ShaderPtr ShaderManager::createShader(const String &name, const UUID &uuid)
    {
        return smart_pointer_cast<Shader>(createResource(name, 1, &uuid));
    }
#endif
    
    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(Archive *archive, const String &filename)
    {
        return smart_pointer_cast<Shader>(load(archive, filename));
    }

    //--------------------------------------------------------------------------

    ShaderPtr ShaderManager::loadShader(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<Shader>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult ShaderManager::saveShader(Archive *archive, const String &filename, Shader *shader)
    {
        return save(archive, filename, shader);
    }

    //--------------------------------------------------------------------------

    TResult ShaderManager::saveShader(Archive *archive, Shader *shader)
    {
        return save(archive, shader); 
    }

    //--------------------------------------------------------------------------

    ResourcePtr ShaderManager::newResource(const String &name, int32_t argc, va_list args)
    {
        ResourcePtr shader;
        shader = Shader::create(name);

#if defined (T3D_EDITOR)
        if (argc == 1)
        {
            UUID *uuid = va_arg(args, UUID*);
            rttr::instance inst(*shader);
            rttr::type t = inst.get_type();
            bool rval = t.set_property_value("UUID", inst, *uuid);
            T3D_ASSERT(rval);
        }
#endif

        return shader;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ShaderManager::loadResource(const String &name, DataStream &stream)
    {
        return loadResource(stream);
    }
    
    //--------------------------------------------------------------------------

    ResourcePtr ShaderManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Shader>(stream);
    }

    //--------------------------------------------------------------------------

    TResult ShaderManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kShader);
        Shader *shader = static_cast<Shader*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, shader);
    }

    //--------------------------------------------------------------------------
}

