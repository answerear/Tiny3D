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

    TResult BuiltinMaterials::generate(const String &rootPath)
    {
        TResult ret = T3D_OK;

        do
        {
            String name = "Default-Material";
            Shader *shader = T3D_BUILTIN_SHADERS.getShader(name);
            MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(name, shader);
            String path = rootPath + Dir::getNativeSeparator() + "materials";
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, "FileSystem", Archive::AccessMode::kTruncate);
            T3D_ASSERT(archive);
            ret = T3D_MATERIAL_MGR.saveMaterial(archive, material);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Save material %s to path %s failed ! ERROR [%d]", path.c_str(), name.c_str(), ret);
            }

            mMaterials.emplace(name, material);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
