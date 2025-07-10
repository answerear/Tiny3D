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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    BoneNodePtr BoneNode::create(const String &name, uint16_t parentIndex, const Vector3 &translation, const Quaternion &rotation, const Vector3 &scaling, const Matrix4 &offsetMatrix)
    {
        return new BoneNode(name, parentIndex, translation, rotation, scaling, offsetMatrix);
    }

    //--------------------------------------------------------------------------

    BoneNode::BoneNode(const String &name, uint16_t parentIndex, const Vector3 &translation, const Quaternion &rotation, const Vector3 &scaling, const Matrix4 &offsetMatrix)
        : mName(name)
        , mParentIndex(parentIndex)
        , mTranslation(translation)
        , mRotation(rotation)
        , mScaling(scaling)
        , mOffsetMatrix(offsetMatrix)
    {
        
    }

    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name)
    {
        return new SkinnedMesh(name);
    }

    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name, SkeletalAnimation *skeletalAni, const Bones &bones)
    {
        return new SkinnedMesh(name, skeletalAni, bones);
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

    SkinnedMesh::SkinnedMesh(const String &name, SkeletalAnimation *skeletalAni, const Bones &bones)
        : Mesh(name)
        , mSkeletalAniUUID(skeletalAni->getUUID())
        , mBones(bones)
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
            
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
