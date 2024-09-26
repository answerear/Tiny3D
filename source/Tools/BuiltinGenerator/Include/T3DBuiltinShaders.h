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


#pragma once


#include "T3DBuiltinGenPrerequisites.h"


namespace Tiny3D
{
    class BuiltinShaders : public Singleton<BuiltinShaders>
    {
    public:
        struct ShaderData
        {
            ShaderData() = default;

            ShaderData(const UUID &uuid, const ShaderPtr &s)
                : shaderLabUUID(uuid)
                , shader(s)
            {}
            
            UUID shaderLabUUID {UUID::INVALID};
            ShaderPtr shader {nullptr};
        };
        
        BuiltinShaders() = default;

        ~BuiltinShaders() = default;

        TResult generate(const String &rootPath, bool reservedTemp);

        // Shader *getShader(const String &name) const
        // {
        //     auto it = mShaders.find(name);
        //     return (it == mShaders.end() ? nullptr : it->second);
        // }

        const ShaderData &getShaderData(const String &name) const
        {
            static ShaderData dummy;
            auto it = mShaders.find(name);
            return (it == mShaders.end() ? dummy : it->second);
        }
        
    protected:
        TResult generateShader(const String &path, const String &outputPath);

        TResult generateShaderFile(const String &path, const String &outputPath);
        
        // using Shaders = TMap<String, ShaderPtr>;

        using Shaders = TUnorderedMap<String, ShaderData>;
        
        Shaders mShaders {};
    };

    #define T3D_BUILTIN_SHADERS (BuiltinShaders::getInstance())
}


