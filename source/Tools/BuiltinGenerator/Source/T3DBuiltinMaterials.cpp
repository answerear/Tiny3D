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

    const char *MATERIAL_TITLE = "Default-Material";
    const char *TEST_MATERIAL_TITLE = "Test-Material";
    
    //--------------------------------------------------------------------------

    TResult BuiltinMaterials::generate(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // default meterial
            ret = generateMaterial(MATERIAL_TITLE, rootPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate default material !");
            }

            // test material
            ret = generateMaterial(TEST_MATERIAL_TITLE, rootPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate test material !");
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinMaterials::generateMaterial(const String &title, const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            // 生成 material
            // String title = MATERIAL_TITLE;
            String shaderName = title + "." + Resource::EXT_SHADER;
            // Shader *shader = T3D_BUILTIN_SHADERS.getShader(shaderName);
            auto shaderData = T3D_BUILTIN_SHADERS.getShaderData(shaderName);
            T3D_ASSERT(shaderData.shader != nullptr);
            String materialName = title + "." + Resource::EXT_MATERIAL;
            MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(title, shaderData.shader);
            String path = rootPath + Dir::getNativeSeparator() + "materials";
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive);
            
            // 这里 hook ，用反射去修改引用的 shader uuid
            rttr::instance inst(*material);
            rttr::type t = inst.get_type();
            bool rval = t.set_property_value("Shader", inst, shaderData.shaderLabUUID);
            T3D_ASSERT(rval);
            // BGEN_LOG_DEBUG("Material name : %s, uuid : %s, material shader : %s, shader name : %s, shader lab uuid : %s, shader uuid : %s",
            //     materialName.c_str(), material->getUUID().toString().c_str(), material->getShaderUUID().toString().c_str(),
            //     shaderName.c_str(), shaderData.shaderLabUUID.toString().c_str(), shaderData.shader->getUUID().toString().c_str());
            
            // 保存材质
            ret = T3D_MATERIAL_MGR.saveMaterial(archive, materialName, material);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to save material [%s] ! ERROR [%d]", (path + Dir::getNativeSeparator() + materialName).c_str(), ret);
                break;
            }

            // 生成 material meta file
            MetaMaterialPtr meta = MetaMaterial::create(material->getUUID());
            String metaName = materialName + ".meta";
            ret = archive->write(metaName, [&meta](DataStream &stream, const String &filename)
                {
                    return T3D_SERIALIZER_MGR.serialize(stream, meta);
                });
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate material meta file (%s) ! ERROR [%d]", metaName.c_str(), ret);
                break;
            }
            
            mMaterials.emplace(materialName, material);
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    // TResult BuiltinMaterials::generateDefaultMaterial(const String &rootPath)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         // default material
    //         String title = MATERIAL_TITLE;
    //         String shaderName = title + "." + Resource::EXT_SHADER;
    //         Shader *shader = T3D_BUILTIN_SHADERS.getShader(shaderName);
    //         String materialName = title + "." + Resource::EXT_MATERIAL;
    //         MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(title, shader);
    //         String path = rootPath + Dir::getNativeSeparator() + "materials";
    //         ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
    //         T3D_ASSERT(archive);
    //         ret = T3D_MATERIAL_MGR.saveMaterial(archive, materialName, material);
    //         if (T3D_FAILED(ret))
    //         {
    //             BGEN_LOG_ERROR("Failed to save material [%s] ! ERROR [%d]", (path + Dir::getNativeSeparator() + materialName).c_str(), ret);
    //             break;
    //         }
    //
    //         // default material meta file
    //         MetaMaterialPtr meta = MetaMaterial::create(material->getUUID());
    //         String metaName = materialName + ".meta";
    //         ret = archive->write(metaName, [&meta](DataStream &stream)
    //             {
    //                 return T3D_SERIALIZER_MGR.serialize(stream, meta);
    //             });
    //         if (T3D_FAILED(ret))
    //         {
    //             BGEN_LOG_ERROR("Failed to generate default material meta file (%s) ! ERROR [%d]", metaName.c_str(), ret);
    //             break;
    //         }
    //         
    //         mMaterials.emplace(materialName, material);
    //     } while (false);
    //     
    //     return ret;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // TResult BuiltinMaterials::generateTestMaterial(const String &rootPath)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         // Testing material file
    //         String title = TEST_MATERIAL_TITLE;
    //         String shaderName = title + "." + Resource::EXT_SHADER;
    //         Shader *shader = T3D_BUILTIN_SHADERS.getShader(shaderName);
    //         String materialName = title + "." + Resource::EXT_MATERIAL;
    //         MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(title, shader);
    //         String path = rootPath + Dir::getNativeSeparator() + "materials";
    //         ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
    //         T3D_ASSERT(archive);
    //         ret = T3D_MATERIAL_MGR.saveMaterial(archive, materialName, material);
    //         if (T3D_FAILED(ret))
    //         {
    //             BGEN_LOG_ERROR("Failed to save material [%s] ! ERROR [%d]", (path + Dir::getNativeSeparator() + materialName).c_str(), ret);
    //             break;
    //         }
    //
    //         // Testing material meta file
    //         MetaMaterialPtr meta = MetaMaterial::create(material->getUUID());
    //         String metaName = materialName + ".meta";
    //         ret = archive->write(metaName, [&meta](DataStream &stream)
    //             {
    //                 return T3D_SERIALIZER_MGR.serialize(stream, meta);
    //             });
    //         if (T3D_FAILED(ret))
    //         {
    //             BGEN_LOG_ERROR("Failed to generate test material meta file (%s) ! ERROR [%d]", metaName.c_str(), ret);
    //             break;
    //         }
    //         
    //         mMaterials.emplace(materialName, material);
    //     } while (false);
    //
    //     return ret;
    // }
    
    //--------------------------------------------------------------------------
}
