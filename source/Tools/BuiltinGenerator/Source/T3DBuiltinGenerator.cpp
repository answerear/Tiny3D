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


#include "T3DBuiltinGenerator.h"
#include "T3DBuiltinTextures.h"
#include "T3DBuiltinShaders.h"
#include "T3DBuiltinMaterials.h"
#include "T3DBuiltinCube.h"
#include "T3DBuiltinCylinder.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    BuiltinGenerator::~BuiltinGenerator()
    {
        T3D_SAFE_DELETE(mBuiltinShaders);
        T3D_SAFE_DELETE(mBuiltinMaterials);
        T3D_SAFE_DELETE(mBuiltinTextures);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinGenerator::run(const String &rootPath, bool reservedTemp)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = generateTextures(rootPath, reservedTemp);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate all builtin textures ! ERROR [%d]", ret);
            }
            
            ret = generateShaders(rootPath, reservedTemp);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate all builtin shaders ! ERROR [%d]", ret);
            }
            
            ret = generateMaterials(rootPath, reservedTemp);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate all builtin materials ! ERROR [%d]", ret);
            }

            ret = generateMeshes(rootPath, reservedTemp);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Failed to generate all builtin meshes ! ERROR [%d]", ret);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinGenerator::generateMeshes(const String &rootPath, bool reservedTemp)
    {
        TResult ret = T3D_OK;

        String meshPath = rootPath + Dir::getNativeSeparator() + "meshes";
        Dir::makeDir(meshPath);

        // Cube
        BuiltinMesh *mesh = new BuiltinCube();
        ret = mesh->build();
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Build cube mesh failed ! ERROR [%d]", ret);
        }
        else
        {
            BGEN_LOG_INFO("Build cube mesh ok !");

            ret = mesh->save(meshPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Generated cube mesh failed ! ERROR [%d]", ret);
            }
            else
            {
                BGEN_LOG_INFO("Generated cube mesh ok !");
            }
        }
        T3D_SAFE_DELETE(mesh);

        // Cylinder
        mesh = new BuiltinCylinder();
        ret = mesh->build();
        if (T3D_FAILED(ret))
        {
            BGEN_LOG_ERROR("Build cylinder mesh failed ! ERROR [%d]", ret);
        }
        else
        {
            BGEN_LOG_INFO("Build cylinder mesh ok !");

            ret = mesh->save(meshPath);
            if (T3D_FAILED(ret))
            {
                BGEN_LOG_ERROR("Generated cylinder mesh failed ! ERROR [%d]", ret);
            }
            else
            {
                BGEN_LOG_INFO("Generated cylinder mesh ok !");
            }
        }
        T3D_SAFE_DELETE(mesh);

        // Sphere
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BuiltinGenerator::generateShaders(const String &rootPath, bool reservedTemp)
    {
        T3D_SAFE_DELETE(mBuiltinShaders);
        mBuiltinShaders = new BuiltinShaders();
        return mBuiltinShaders->generate(rootPath, reservedTemp);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinGenerator::generateMaterials(const String &rootPath, bool reservedTemp)
    {
        if (mBuiltinMaterials == nullptr)
        {
            mBuiltinMaterials = new BuiltinMaterials();
        }
        
        return mBuiltinMaterials->generate(rootPath);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinGenerator::generateTextures(const String &rootPath, bool reservedTemp)
    {
        if (mBuiltinTextures == nullptr)
        {
            mBuiltinTextures = new BuiltinTextures();
        }

        return mBuiltinTextures->generate(rootPath);
    }

    //--------------------------------------------------------------------------
}
