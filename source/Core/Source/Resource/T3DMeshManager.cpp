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


#include "Resource/T3DMeshManager.h"
#include "Resource/T3DMesh.h"
#include "Resource/T3DSkinnedMesh.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    MeshManagerPtr MeshManager::create()
    {
        return T3D_NEW MeshManager();
    }
    
    //--------------------------------------------------------------------------

    MeshPtr MeshManager::createMesh(const String &name, VertexAttributes &&attributes, Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes)
    {
        VertexAttributes attrs = std::move(attributes);
        Vertices verts = std::move(vertices);
        VertexStrides vstrides = std::move(strides);
        VertexOffsets voffsets = std::move(offsets);
        SubMeshes subs = std::move(submeshes);
        return smart_pointer_cast<Mesh>(createResource(name, 5, &attrs, &verts, &vstrides, &voffsets, &subs));
    }

    //--------------------------------------------------------------------------

    MeshPtr MeshManager::loadMesh(Archive *archive, const String &filename)
    {
        return smart_pointer_cast<Mesh>(load(archive, filename));
    }

    //--------------------------------------------------------------------------

    MeshPtr MeshManager::loadMesh(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<Mesh>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult MeshManager::saveMesh(Archive *archive, const String &filename, Mesh *mesh)
    {
        return save(archive, filename, mesh);
    }

    //--------------------------------------------------------------------------

    TResult MeshManager::saveMesh(Archive *archive, Mesh *mesh)
    {
        return save(archive, mesh);
    }

    //--------------------------------------------------------------------------

    SkinnedMeshPtr MeshManager::createSkinnedMesh(const String &name,
        VertexAttributes &&attributes, Vertices &&vertices,
        VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
        SkeletalAnimation *skeletalAni, const Bones &&bones)
    {
        VertexAttributes attrs = std::move(attributes);
        Vertices verts = std::move(vertices);
        VertexStrides vstrides = std::move(strides);
        VertexOffsets voffsets = std::move(offsets);
        SubMeshes subs = std::move(submeshes);
        Bones nodes = std::move(bones);
        return smart_pointer_cast<SkinnedMesh>(createResource(name, 7, &attrs, &verts, &vstrides, &voffsets, &subs, skeletalAni, &nodes));
    }

    //--------------------------------------------------------------------------

    ResourcePtr MeshManager::newResource(const String &name, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 5 || argc == 7);
        if (argc == 5)
        {
            // Mesh
            VertexAttributes *attributes = va_arg(args, VertexAttributes*);
            Vertices *vertices = va_arg(args, Vertices*);
            VertexStrides *strides = va_arg(args, VertexStrides*);
            VertexOffsets *offsets = va_arg(args, VertexOffsets*);
            SubMeshes *submeshes = va_arg(args, SubMeshes*);
            return Mesh::create(name, std::move(*attributes), std::move(*vertices), std::move(*strides), std::move(*offsets), std::move(*submeshes));
        }
        else
        {
            // Skinned Mesh
            VertexAttributes *attributes = va_arg(args, VertexAttributes*);
            Vertices *vertices = va_arg(args, Vertices*);
            VertexStrides *strides = va_arg(args, VertexStrides*);
            VertexOffsets *offsets = va_arg(args, VertexOffsets*);
            SubMeshes *submeshes = va_arg(args, SubMeshes*);
            SkeletalAnimation *skeletalAni = va_arg(args, SkeletalAnimation*);
            Bones *bones = va_arg(args, Bones*);
            return SkinnedMesh::create(name, std::move(*attributes), std::move(*vertices), std::move(*strides), std::move(*offsets), std::move(*submeshes), skeletalAni, std::move(*bones));
        }
        
    }

    //--------------------------------------------------------------------------

    ResourcePtr MeshManager::loadResource(const String &name, DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Mesh>(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MeshManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Mesh>(stream);
    }

    //--------------------------------------------------------------------------

    TResult MeshManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kMesh);
        Mesh *mesh = static_cast<Mesh*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, mesh);
    }

    //--------------------------------------------------------------------------
}

