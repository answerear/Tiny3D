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

#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API MaterialManager
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        static MaterialManagerPtr create();

        MaterialPtr createMaterial(const String &name, Shader *shader);

        MaterialPtr loadMaterial(Archive *archive, const String &name);

        TResult saveMaterial(Archive *archive, Material *material);

    protected:

        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        ResourcePtr loadResource(const String &name, DataStream &stream) override;
        
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_MATERIAL_MGR    (MaterialManager::getInstance()) 
}


#endif    /*__T3D_MATERIAL_MANAGER_H__*/
