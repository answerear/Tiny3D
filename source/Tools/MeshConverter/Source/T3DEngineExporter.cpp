/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DEngineExporter.h"

#include "Resource/T3DAnimationManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    EngineExporterPtr EngineExporter::create()
    {
        return new EngineExporter();
    }
    
    //--------------------------------------------------------------------------

    EngineExporter::~EngineExporter()
    {

    }

    //--------------------------------------------------------------------------

    TResult EngineExporter::run(const ConverterOptions &opts, const Assets &resources)
    {
        TResult ret = T3D_OK;

        for (auto res : resources)
        {
            ret = exportResource(opts, res.second);
        }
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportResource(const ConverterOptions &opts, Resource *resource)
    {
        TResult ret = T3D_OK;

        do
        {
            switch (resource->getType())
            {
            case Resource::Type::kMaterial:
                {
                    ret = exportMaterial(opts, static_cast<Material *>(resource));
                }
                break;
            case Resource::Type::kMesh:
                {
                    ret = exportMesh(opts, static_cast<Mesh *>(resource));
                }
                break;
            case Resource::Type::kSkinnedMesh:
                {
                    ret = exportSkinnedMesh(opts, static_cast<SkinnedMesh *>(resource));
                }
                break;
            case Resource::Type::kSkeleton:
                {
                    ret = exportSkeleton(opts, static_cast<Skeleton *>(resource));
                }
                break;
            case Resource::Type::kSkeletalAnimation:
                {
                    ret = exportSkeletalAnimation(opts, static_cast<SkeletalAnimation *>(resource));
                }
                break;
            default:
                {
                    ret = T3D_ERR_FAIL;
                    MCONV_LOG_ERROR("Invalid resource type");
                }
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportMaterial(const ConverterOptions &opts, Material *material)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive);
        String filename = material->getName() + "." + Resource::EXT_MATERIAL;
        return T3D_MATERIAL_MGR.saveMaterial(archive, filename, material);
    }

    //--------------------------------------------------------------------------

    TResult EngineExporter::exportMesh(const ConverterOptions &opts, Mesh *mesh)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive);
        String filename = mesh->getName() + "." + Resource::EXT_MESH;
        return T3D_MESH_MGR.saveMesh(archive, filename, mesh);
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkinnedMesh(const ConverterOptions &opts,SkinnedMesh *mesh)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive);
        String filename = mesh->getName() + "." + Resource::EXT_MESH;
        return T3D_MESH_MGR.saveMesh(archive, filename, mesh);
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkeleton(const ConverterOptions &opts, Skeleton *skeleton)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive);
        String filename = skeleton->getName() + "." + Resource::EXT_SKELETON;
        return T3D_SKELETON_MGR.saveSkeleton(archive, filename, skeleton);
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkeletalAnimation(const ConverterOptions &opts, SkeletalAnimation *anim)
    {
        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
        T3D_ASSERT(archive);
        String filename = anim->getName() + "." + Resource::EXT_ANIMATION;
        return T3D_ANIMATION_MGR.saveSkeletalAnimation(archive, filename, anim);
    }
    
    //--------------------------------------------------------------------------
}

