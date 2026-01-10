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
            case Resource::Type::kTexture:
                {
                    ret = exportTexture(opts, static_cast<Texture *>(resource));
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
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting material (%s) ...", material->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = material->getName() + "." + Resource::EXT_MATERIAL;
            ret = T3D_MATERIAL_MGR.saveMaterial(archive, filename, material);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export material (%s)", material->getName().c_str())
                break;
            }

            MCONV_LOG_INFO("Completed exporting material (%s) !", material->getName().c_str())
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EngineExporter::exportTexture(const ConverterOptions &opts, Texture *texture)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting texture (%s) ...", texture->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = texture->getName() + "." + Resource::EXT_TEXTURE;
            ret = T3D_TEXTURE_MGR.saveTexture(archive, filename, texture);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export texture (%s)", texture->getName().c_str())
                break;
            }

            MCONV_LOG_INFO("Completed exporting texture (%s) !", texture->getName().c_str())
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult EngineExporter::exportMesh(const ConverterOptions &opts, Mesh *mesh)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting mesh (%s) ...", mesh->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = mesh->getName() + "." + Resource::EXT_MESH;
            ret = T3D_MESH_MGR.saveMesh(archive, filename, mesh);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export mesh (%s)", mesh->getName().c_str());
                break;
            }

            MCONV_LOG_INFO("Completed exporting mesh (%s) !", mesh->getName().c_str())
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkinnedMesh(const ConverterOptions &opts,SkinnedMesh *mesh)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting skinned mesh (%s) ...", mesh->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = mesh->getName() + "." + Resource::EXT_MESH;
            ret = T3D_MESH_MGR.saveMesh(archive, filename, mesh);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export skinned mesh (%s)", mesh->getName().c_str());
                break;
            }

            MCONV_LOG_INFO("Completed exporting skinned mesh (%s) !", mesh->getName().c_str())
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkeleton(const ConverterOptions &opts, Skeleton *skeleton)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting skeleton (%s) ...", skeleton->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = skeleton->getName() + "." + Resource::EXT_SKELETON;
            ret = T3D_SKELETON_MGR.saveSkeleton(archive, filename, skeleton);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export skeleton (%s)", skeleton->getName().c_str());
                break;
            }

            MCONV_LOG_INFO("Completed exporting skeleton (%s) !", skeleton->getName().c_str())
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult EngineExporter::exportSkeletalAnimation(const ConverterOptions &opts, SkeletalAnimation *anim)
    {
        TResult ret = T3D_OK;

        do
        {
            MCONV_LOG_INFO("Start exporting skeletal animation (%s) ...", anim->getName().c_str())
            
            ArchivePtr archive;
            if (opts.isGeneratingMeta)
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            }
            else
            {
                archive = T3D_ARCHIVE_MGR.loadArchive(opts.dstDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kTruncate);
            }
            T3D_ASSERT(archive);
            String filename = anim->getName() + "." + Resource::EXT_ANIMATION;
            ret = T3D_ANIMATION_MGR.saveSkeletalAnimation(archive, filename, anim);
            if (T3D_FAILED(ret))
            {
                MCONV_LOG_ERROR("Failed to export skeletal animation (%s)", anim->getName().c_str());
                break;
            }

            MCONV_LOG_INFO("Completed exporting skeletal animation (%s) !", anim->getName().c_str())
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------
}

