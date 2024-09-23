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


#include "T3DBuiltinMaterials.h"
#include "T3DBuiltinShaders.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

#if !defined (GENERATE_TEST_MESH)
    const char *MATERIAL_TITLE = "Default-Material";
    const char *TEST_MATERIAL_TITLE = "Test-Material";
#else
    const char *MATERIAL_TITLE = "Test-Material";
#endif
    
    //--------------------------------------------------------------------------

    TResult BuiltinMaterials::generate(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // default meterial
            ret = generateDefaultMaterial(rootPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate default material !");
            }

            // test material
            ret = generateTestMaterial(rootPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate test material !");
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinMaterials::generateDefaultMaterial(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            String title = MATERIAL_TITLE;
            String shaderName = title + "." + Resource::EXT_SHADER;
            Shader *shader = T3D_BUILTIN_SHADERS.getShader(shaderName);
            String materialName = title + "." + Resource::EXT_MATERIAL;
            MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(title, shader);
            String path = rootPath + Dir::getNativeSeparator() + "materials";
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive);
            ret = T3D_MATERIAL_MGR.saveMaterial(archive, materialName, material);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save material [%s] ! ERROR [%d]", (path + Dir::getNativeSeparator() + materialName).c_str(), ret);
                break;
            }

            mMaterials.emplace(materialName, material);
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult BuiltinMaterials::generateTestMaterial(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            String title = TEST_MATERIAL_TITLE;
            String shaderName = title + "." + Resource::EXT_SHADER;
            Shader *shader = T3D_BUILTIN_SHADERS.getShader(shaderName);
            String materialName = title + "." + Resource::EXT_MATERIAL;
            MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(title, shader);
            String path = rootPath + Dir::getNativeSeparator() + "materials";
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive);
            ret = T3D_MATERIAL_MGR.saveMaterial(archive, materialName, material);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save material [%s] ! ERROR [%d]", (path + Dir::getNativeSeparator() + materialName).c_str(), ret);
                break;
            }

            mMaterials.emplace(materialName, material);
        } while (false);

        return ret;
    }
    //--------------------------------------------------------------------------
}
