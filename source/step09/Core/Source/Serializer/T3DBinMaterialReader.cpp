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


#include "Serializer/T3DBinMaterialReader.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DCommon.h"
#include "Resource/T3DMaterial.h"
#include "Kernel/T3DTechnique.h"
#include "Kernel/T3DPass.h"
#include "Kernel/T3DTextureUnit.h"
#include "protobuf/MaterialScriptObject.pb.h"


namespace Tiny3D
{
    using namespace Script;

    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(BinMaterialReader, MaterialReader);

    //--------------------------------------------------------------------------

    BinMaterialReaderPtr BinMaterialReader::create()
    {
        BinMaterialReaderPtr reader = new BinMaterialReader();
        reader->release();
        return reader;
    }

    //--------------------------------------------------------------------------

    BinMaterialReader::BinMaterialReader()
    {

    }

    //--------------------------------------------------------------------------

    BinMaterialReader::~BinMaterialReader()
    {

    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parse(DataStream &stream, Material *material)
    {
        TResult ret = T3D_OK;
        uint8_t *data = nullptr;

        do 
        {
            size_t size = stream.size();
            data = new uint8_t[size];

            size = stream.read(data);

            if (size == 0)
            {
                ret = T3D_ERR_INVALID_SIZE;
                break;
            }

            ret = parse(data, size, material);
        } while (0);

        T3D_SAFE_DELETE_ARRAY(data);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parse(
        const uint8_t *data, size_t size, Material *material)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 读取文件头
            T3DFileHeader *header = (T3DFileHeader *)data;

            if (stricmp(header->magic, T3D_MAGIC) != 0)
            {
                // 非法的文件类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file type !");
                break;
            }

            if (header->subtype != FileSubType::E_FST_SCRIPT)
            {
                // 非法材质脚本类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file subtype !");
                break;
            }

            // 从 proto buffer 解析出来 pb 对象
            data += sizeof(header);
            MaterialSystem::Material src;

            if (!src.ParseFromArray(data, size))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file content !");
                break;
            }

            ret = parseMaterial(&src, material);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseMaterial(
        const MaterialSystem::Material *src, Material *dst)
    {
        TResult ret = T3D_OK;

        do 
        {
            // Material name
            const MaterialSystem::Header &header = src->header();
            dst->setMaterialName(header.name());

            // lod_values & lod_strategy
            // auto values = src->values();

            // receive_shadows
            // src->receive_shadows();

            // transparency_casts_shadows
            // src->transparency_casts_shadows();

            // texture_alias
            // auto alias = src->texture_alias();
            // auto aliasName = alias.alias_name();
            // auto texName = alias.texture_name();

            // Techniques
            auto techniques = src->techniques();

            for (const MaterialSystem::Technique &tech : techniques)
            {
                ret = parseTechnique(&tech, dst);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseTechnique(
        const MaterialSystem::Technique *src, Material *dst)
    {
        TResult ret = T3D_OK;

        do 
        {
            // Technique name
            const MaterialSystem::Header &header = src->header();
            TechniquePtr tech;
            ret = dst->addTechnique(header.name(), tech);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Add technique for material [%s] failed !", 
                    dst->getName().c_str());
                    break;
            }

            // render_queue
            if (src->has_render_queue())
            {
                tech->setRenderQueue(src->render_queue().value());
            }

            // lod_index
            // auto index = src->lod_index();

            // scheme
            // auto scheme = src->scheme();

            // shadow_caster_material
            // auto casterMaterial = src->shadow_caster_material();

            // shadow_receiver_material
            // auto receiverMaterial = src->shadow_receiver_material();

            // gpu_vendor_rule
            // auto vendor = src->gpu_vendor_rule();
            // auto ruleType = vendor.rule();
            // auto vendorName = vendor.vendor_name();

            // gpu_device_rule
            // auto device = src->gpu_device_rule();
            // auto ruleType = device.rule();
            // auto pattern = device.device_pattern();
            // auto caseSensitive = device.case_sensitive();

            // Passes
            auto passes = src->passes();

            for (const MaterialSystem::Pass &pass : passes)
            {
                ret = parsePass(&pass, tech);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parsePass(
        const MaterialSystem::Pass *src, Technique *dst)
    {
        TResult ret = T3D_OK;

        do 
        {
            // Pass name
            const MaterialSystem::Header &header = src->header();
            PassPtr pass;
            ret = dst->addPass(header.name(), pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // ambient
            ret = parseAmbient(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // diffuse
            ret = parseDiffuse(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // specular
            ret = parseSpecular(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // emissive
            ret = parseEmissive(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // scene_blend
            ret = parseSceneBlend(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // separate_scene_blend
            ret = parseSeparateSceneBlend(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // scene_blend_op
            ret = parseSceneBlendOp(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // separate_scene_blend_op
            ret = parseSeparateSceneBlendOp(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // depth_check
            ret = parseDepthCheck(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // depth_write
            ret = parseDepthWrite(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // depth_func
            ret = parseDepthFunc(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // depth_bias
            ret = parseDepthBias(src, pass);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // iteration_depth_bias

            // alpha_rejection

            // alpha_to_coverage

            // light_scissor

            // light_clip_planes

            // illumination_stage

            // normailize_normals

            // transparent_sorting

            // cull_hardware

            // cull_software

            // lighting

            // shading

            // polygon_mode

            // polygon_mode_overrideable

            // fog

            // color_write

            // max_lights

            // start_light

            // iteration

            // point_size

            // point_sprites

            // point_size_attenuation

            // point_size_min

            // point_size_max

            // gpu_program_ref

            // textures
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseAmbient(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_ambient())
        {
            auto ambient = src->ambient();
            if (!ambient.has_color() && ambient.vertex_color())
            {
                // use vertex color for ambient
                dst->setVertexColorTracking(TrackVertexColor::AMBIENT);
            }
            else
            {
                // color value
                auto clr = ambient.color();
                dst->setAmbient(clr.r(), clr.g(), clr.b(), clr.a());
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseDiffuse(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_diffuse())
        {
            auto diffuse = src->diffuse();
            if (!diffuse.has_color() && diffuse.vertex_color())
            {
                // use vertex color for diffuse
                dst->setVertexColorTracking(TrackVertexColor::DIFFUSE);
            }
            else
            {
                // color value
                auto clr = diffuse.color();
                dst->setDiffuse(clr.r(), clr.g(), clr.b(), clr.a());
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseSpecular(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_specular())
        {
            auto specular = src->specular();
            if (!specular.has_color() && specular.vertex_color())
            {
                // use vertex color for specular
                dst->setVertexColorTracking(TrackVertexColor::SPECULAR);
            }
            else
            {
                // color value
                auto clr = specular.color();
                dst->setSpecular(clr.r(), clr.g(), clr.b(), clr.a());
            }

            dst->setShininess(src->shininess());
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseEmissive(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_emissive())
        {
            auto emissive = src->emissive();
            if (!emissive.has_color() && emissive.vertex_color())
            {
                // use vertex color for emissive
                dst->setVertexColorTracking(TrackVertexColor::EMISSIVE);
            }
            else
            {
                // color value
                auto clr = emissive.color();
                dst->setEmissive(clr.r(), clr.g(), clr.b(), clr.a());
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseSceneBlend(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_scene_blend())
        {
            auto blend = src->scene_blend();
            if (blend.has_blend_factor())
            {
                // blend factor
                auto factor = blend.blend_factor();
                BlendFactor srcFactor = (BlendFactor)factor.src_factor();
                BlendFactor dstFactor = (BlendFactor)factor.dest_factor();
                dst->setSceneBlending(srcFactor, dstFactor);
            }
            else
            {
                // blend type
                BlendType blendType = (BlendType)blend.blend_type();
                dst->setSceneBlending(blendType);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseSeparateSceneBlend(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_separate_scene_blend())
        {
            auto blend = src->separate_scene_blend();
            if (blend.has_blend_type())
            {
                // separate blend type
                auto blendType = blend.blend_type();
                BlendType colorType = (BlendType)blendType.color_blend();
                BlendType alphaType = (BlendType)blendType.alpha_blend();
                dst->setSeparateSceneBlending(colorType, alphaType);
            }
            else
            {
                // separate blend factor
                auto blendFactor = blend.blend_factor();
                BlendFactor colorSrcF = (BlendFactor)blendFactor.color_src_factor();
                BlendFactor colorDstF = (BlendFactor)blendFactor.color_dest_factor();
                BlendFactor alphaSrcF = (BlendFactor)blendFactor.alpha_src_factor();
                BlendFactor alphaDstF = (BlendFactor)blendFactor.alpha_dest_factor();
                dst->setSeparateSceneBlending(colorSrcF, colorDstF,
                    alphaSrcF, alphaDstF);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseSceneBlendOp(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        BlendOperation op = (BlendOperation)src->scene_blend_op();
        dst->setSceneBlendingOperation(op);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseSeparateSceneBlendOp(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        if (src->has_separate_scene_blend_op())
        {
            auto op = src->separate_scene_blend_op();
            BlendOperation colorOp = (BlendOperation)op.color_op();
            BlendOperation alphaOp = (BlendOperation)op.alpha_op();
            dst->setSeparateSceneBlendingOperation(colorOp, alphaOp);
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseDepthCheck(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        dst->setDepthCheckEnabled(src->depth_check());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseDepthWrite(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        dst->setDepthWriteEnabled(src->depth_write());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseDepthFunc(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        CompareFunction func = (CompareFunction)src->depth_func();
        dst->setDepthFunction(func);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult BinMaterialReader::parseDepthBias(
        const MaterialSystem::Pass *src, Pass *dst)
    {
        return T3D_OK;
    }
}

