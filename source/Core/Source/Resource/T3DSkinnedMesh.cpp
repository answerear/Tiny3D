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

#include "Resource/T3DSkinnedMesh.h"
#include "T3DErrorDef.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Resource/T3DAnimationManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name)
    {
        return new SkinnedMesh(name);
    }

    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name,
        VertexAttributes &&attributes, Vertices &&vertices,
        VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
        SkeletalAnimation *skeletalAni, Bones &&bones)
    {
        return new SkinnedMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes), skeletalAni, std::move(bones));
    }

    //--------------------------------------------------------------------------

    SkinnedMesh::~SkinnedMesh()
    {

    }
    
    //--------------------------------------------------------------------------

    SkinnedMesh::SkinnedMesh(const String &name)
        : Mesh(name)
    {
        
    }

    //--------------------------------------------------------------------------

    SkinnedMesh::SkinnedMesh(const String &name, VertexAttributes &&attributes,
        Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets,
        SubMeshes &&submeshes, SkeletalAnimation *skeletalAni, Bones &&bones)
        : Mesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes))
        , mSkeletalAniUUID(skeletalAni->getUUID())
        , mBones(std::move(bones))
        , mSkeletalAni(skeletalAni)
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type SkinnedMesh::getType() const
    {
        return Type::kSkinnedMesh;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr SkinnedMesh::clone() const
    {
        SkinnedMeshPtr mesh = create(getName());
        mesh->cloneProperties(this);
        return mesh;
    }
    
    //--------------------------------------------------------------------------

    void SkinnedMesh::cloneProperties(const Resource *const src)
    {
        Mesh::cloneProperties(src);
        
        const SkinnedMesh *mesh = static_cast<const SkinnedMesh*>(src);
    }

    //--------------------------------------------------------------------------

    TResult SkinnedMesh::onCreate()
    {
        TResult ret = Mesh::onCreate();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SkinnedMesh::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Mesh::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = generateRenderResource(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult SkinnedMesh::generateRenderResource(Archive *archive)
    {
        TResult ret = Mesh::generateRenderResource(archive);

        do
        {
            mSkeletalAni = T3D_ANIMATION_MGR.loadSkeletalAnimation(archive, mSkeletalAniUUID);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
