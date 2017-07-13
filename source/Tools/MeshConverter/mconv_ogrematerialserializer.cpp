
#include "mconv_ogrematerialserializer.h"
#include "mconv_string.h"


using namespace Tiny3D;


namespace mconv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // root attribute parsers
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool parseMaterial(String &params, MaterialScriptContext &context)
    {
        StringVector strings = StringUtil::split(params, ":", 1);

        String &name = strings[0];
        StringUtil::trim(name);

        StringUtil::replaceAll(name, "/", "_");
        auto ret = context.materials->insert(OgreMaterialsValue(name, OgreMaterial()));
        T3D_ASSERT(ret.second);

        OgreMaterial &material = (ret.first)->second;

        if (strings.size() >= 2)
        {
            String &baseName = strings[1];
            StringUtil::trim(baseName);
            material.baseName = baseName;
        }

        context.material = &material;
        material.name = name;
        context.section = MSS_MATERIAL;

        return true;
    }

    bool parseVertexProgram(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseGeometryProgram(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseFragmentProgram(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // material attribute parsers
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool parseLodDistances(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.material != nullptr);
        OgreMaterial &material = *(context.material);

        StringVector strings = StringUtil::split(params, " \t");

        auto itr = strings.begin();
        while (itr != strings.end())
        {
            material.LODDistances.push_back(StringConverter::parseReal(*itr));
            ++itr;
        }

        return false;
    }

    bool parseReceiveShadows(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.material != nullptr);
        OgreMaterial &material = *(context.material);

        StringUtil::toLowerCase(params);
        if (params == "on")
        {
            material.receiveShadows = true;
        }
        else if (params == "off")
        {
            material.receiveShadows = false;
        }
        else
        {
            T3D_ASSERT(0);
        }

        return false;
    }

    bool parseTransparencyCastsShadows(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.material != nullptr);
        OgreMaterial &material = *(context.material);

        StringUtil::toLowerCase(params);
        if (params == "on")
        {
            material.transparencyCastsShadows = true;
        }
        else if (params == "off")
        {
            material.transparencyCastsShadows = false;
        }
        else
        {
            T3D_ASSERT(0);
        }

        return false;
    }

    bool parseTechnique(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.material != nullptr);
        OgreMaterial &material = *(context.material);

        material.techniques.push_back(OgreTechnique());
        OgreTechnique &technique = material.techniques.back();
        context.technique = &technique;

        if (!params.empty())
        {
            StringUtil::trim(params);
            technique.name = params;
        }

        context.section = MSS_TECHNIQUE;

        return true;
    }

    bool parseSetTextureAlias(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // technique attribute parsers
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool parseLodIndex(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        technique.LODIndex = StringConverter::parseInt(params);

        return false;
    }

    bool parseShadowCasterMaterial(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        technique.shadowCasterMaterial = params;

        return false;
    }

    bool parseShadowReceiverMaterial(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        technique.shadowReceiverMaterial = params;

        return false;
    }

    bool parseScheme(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        technique.scheme = params;

        return false;
    }

    bool parseGPUVendorRule(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        StringVector strings = StringUtil::split(params, " \t");

        T3D_ASSERT(strings.size() == 2);

        if (strings[0] == "include")
        {
            technique.gpuVendorRule.rule = TR_INCLUDE;
        }
        else if (strings[0] == "exclude")
        {
            technique.gpuVendorRule.rule = TR_EXCLUDE;
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        technique.gpuVendorRule.name = strings[1];

        return false;
    }

    bool parseGPUDeviceRule(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        StringVector strings = StringUtil::split(params, " \t");

        T3D_ASSERT(strings.size() == 2 || strings.size() == 3);

        if (strings[0] == "include")
        {
            technique.gpuDeviceRule.rule = TR_INCLUDE;
        }
        else if (strings[0] == "exclude")
        {
            technique.gpuDeviceRule.rule = TR_EXCLUDE;
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        technique.gpuDeviceRule.devicePattern = strings[1];

        if (strings.size() == 3)
        {
            technique.gpuDeviceRule.caseSensitive = StringConverter::parseBool(strings[2]);
        }

        return false;
    }

    bool parsePass(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.technique != nullptr);
        OgreTechnique &technique = *(context.technique);

        String &name = params;

        technique.passes.push_back(OgrePass());
        OgrePass &pass = technique.passes.back();
        pass.name = params;
        context.pass = &pass;
        context.section = MSS_PASS;

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // pass attribute parsers
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool parseAmbient(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringVector strings = StringUtil::split(params, " \t");

        if (strings.size() == 1)
        {
            // 只有一个参数，是vertexcolour，目前暂时不支持，先跳过吧
        }
        else if (strings.size() == 3 || strings.size() == 4)
        {
            pass.ambient[0] = StringConverter::parseReal(strings[2]);   // blue
            pass.ambient[1] = StringConverter::parseReal(strings[1]);   // green
            pass.ambient[2] = StringConverter::parseReal(strings[0]);   // red
            pass.ambient[3] = ((strings.size() == 4) ? StringConverter::parseReal(strings[3]) : 1.0f);  // alpha
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        return false;
    }

    bool parseDiffuse(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringVector strings = StringUtil::split(params, " \t");

        if (strings.size() == 1)
        {
            // 只有一个参数，是vertexcolour，目前暂时不支持，先跳过吧
        }
        else if (strings.size() == 3 || strings.size() == 4)
        {
            pass.diffuse[0] = StringConverter::parseReal(strings[2]);   // blue
            pass.diffuse[1] = StringConverter::parseReal(strings[1]);   // green
            pass.diffuse[2] = StringConverter::parseReal(strings[0]);   // red
            pass.diffuse[3] = ((strings.size() == 4) ? StringConverter::parseReal(strings[3]) : 1.0f);  // alpha
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        return false;
    }

    bool parseSpecular(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringVector strings = StringUtil::split(params, " \t");

        if (strings.size() == 2)
        {
            // 2个参数，是vertexcolour和shininess，目前先用shininess
            pass.shininess = StringConverter::parseReal(strings[1]);
        }
        else if (strings.size() == 4 || strings.size() == 5)
        {
            pass.diffuse[0] = StringConverter::parseReal(strings[2]);   // blue
            pass.diffuse[1] = StringConverter::parseReal(strings[1]);   // green
            pass.diffuse[2] = StringConverter::parseReal(strings[0]);   // red
            pass.diffuse[3] = ((strings.size() == 5) ? StringConverter::parseReal(strings[3]) : 1.0f);  // alpha
            pass.shininess = StringConverter::parseReal(strings[strings.size() - 1]);   // shininess
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        return false;
    }

    bool parseEmissive(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringVector strings = StringUtil::split(params, " \t");

        if (strings.size() == 1)
        {
            // 只有一个参数，是vertexcolour，目前暂时不支持，先跳过吧
        }
        else if (strings.size() == 3 || strings.size() == 4)
        {
            pass.emissive[0] = StringConverter::parseReal(strings[2]);   // blue
            pass.emissive[1] = StringConverter::parseReal(strings[1]);   // green
            pass.emissive[2] = StringConverter::parseReal(strings[0]);   // red
            pass.emissive[3] = ((strings.size() == 4) ? StringConverter::parseReal(strings[3]) : 1.0f);  // alpha
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        return false;
    }

    bool parseSceneBlend(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringUtil::toLowerCase(params);
        StringVector strings = StringUtil::split(params, " \t");

        if (strings.size() == 1)
        {
            pass.sceneBlend.push_back(strings[0]);
        }
        else if (strings.size() == 2)
        {
            pass.sceneBlend.push_back(strings[0]);
            pass.sceneBlend.push_back(strings[1]);
        }
        else
        {
            T3D_ASSERT(0);
            return false;
        }

        return false;
    }

    bool parseSeparateSceneBlend(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseDepthCheck(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseDepthWrite(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseDepthFunc(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseNormalizeNormals(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseAlphaRejection(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseAlphaToCoverage(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTransparentSorting(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseColourWrite(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseLightScissor(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseLightClip(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseCullHardware(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseCullSoftware(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseLighting(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseFogging(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseShading(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        StringUtil::toLowerCase(params);
        pass.shading = params;

        return false;
    }

    bool parsePolygonMode(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePolygonModeOverrideable(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseDepthBias(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseIterationDepthBias(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTextureUnit(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.pass != nullptr);
        OgrePass &pass = *(context.pass);

        pass.textures.push_back(OgreTextureUnit());
        OgreTextureUnit &textureUnit = pass.textures.back();
        textureUnit.name = params;
        context.textureUnit = &textureUnit;
        context.section = MSS_TEXTUREUNIT;

        return true;
    }

    bool parseVertexProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseGeometryProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseShadowCasterVertexProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseShadowReceiverVertexProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseShadowReceiverFragmentProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseFragmentProgramRef(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseMaxLights(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseStartLight(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseIteration(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePointSize(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePointSprites(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePointAttenuation(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePointSizeMin(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parsePointSizeMax(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseIlluminationStage(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // texture unit attribute parsers
    ////////////////////////////////////////////////////////////////////////////////////////////////

    bool parseTextureSource(String &params, MaterialScriptContext &context)
    {
        return true;
    }

    bool parseTexture(String &params, MaterialScriptContext &context)
    {
        T3D_ASSERT(context.textureUnit != nullptr);
        OgreTextureUnit &textureUnit = *(context.textureUnit);

        StringVector strings = StringUtil::split(params, " \t");
        textureUnit.texture.name = strings[0];

        return false;
    }

    bool parseAnimTexture(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseCubicTexture(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseBindingType(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTexCoord(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTexAddressMode(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTexBorderColour(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseColourOp(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseColourOpEx(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseColourOpFallback(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseAlphaOpEx(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseEnvMap(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseScroll(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseScrollAnim(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseRotate(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseRotateAnim(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseScale(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseWaveXform(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTransform(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseFiltering(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseAnisotropy(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseTextureAlias(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseMipmapBias(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    bool parseContentType(String &params, MaterialScriptContext &context)
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Ogre Material Serializer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    OgreMaterialSerializer::OgreMaterialSerializer()
    {
        // root attribute parsers
        mRootAttribParsers.insert(AttribParserListValue("material", (ATTRIBUTE_PARSER)parseMaterial));
        mRootAttribParsers.insert(AttribParserListValue("vertex_program", (ATTRIBUTE_PARSER)parseVertexProgram));
        mRootAttribParsers.insert(AttribParserListValue("geometry_program", (ATTRIBUTE_PARSER)parseGeometryProgram));
        mRootAttribParsers.insert(AttribParserListValue("fragment_program", (ATTRIBUTE_PARSER)parseFragmentProgram));

        // material attribute parsers
        mMaterialAttribParsers.insert(AttribParserListValue("lod_distances", (ATTRIBUTE_PARSER)parseLodDistances));
        mMaterialAttribParsers.insert(AttribParserListValue("receive_shadows", (ATTRIBUTE_PARSER)parseReceiveShadows));
        mMaterialAttribParsers.insert(AttribParserList::value_type("transparency_casts_shadows", (ATTRIBUTE_PARSER)parseTransparencyCastsShadows));
        mMaterialAttribParsers.insert(AttribParserList::value_type("technique", (ATTRIBUTE_PARSER)parseTechnique));
        mMaterialAttribParsers.insert(AttribParserList::value_type("set_texture_alias", (ATTRIBUTE_PARSER)parseSetTextureAlias));

        // technique attribute parsers
        mTechniqueAttribParsers.insert(AttribParserList::value_type("lod_index", (ATTRIBUTE_PARSER)parseLodIndex));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("shadow_caster_material", (ATTRIBUTE_PARSER)parseShadowCasterMaterial));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("shadow_receiver_material", (ATTRIBUTE_PARSER)parseShadowReceiverMaterial));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("scheme", (ATTRIBUTE_PARSER)parseScheme));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("gpu_vendor_rule", (ATTRIBUTE_PARSER)parseGPUVendorRule));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("gpu_device_rule", (ATTRIBUTE_PARSER)parseGPUDeviceRule));
        mTechniqueAttribParsers.insert(AttribParserList::value_type("pass", (ATTRIBUTE_PARSER)parsePass));

        // pass attribute parsers
        mPassAttribParsers.insert(AttribParserList::value_type("ambient", (ATTRIBUTE_PARSER)parseAmbient));
        mPassAttribParsers.insert(AttribParserList::value_type("diffuse", (ATTRIBUTE_PARSER)parseDiffuse));
        mPassAttribParsers.insert(AttribParserList::value_type("specular", (ATTRIBUTE_PARSER)parseSpecular));
        mPassAttribParsers.insert(AttribParserList::value_type("emissive", (ATTRIBUTE_PARSER)parseEmissive));
        mPassAttribParsers.insert(AttribParserList::value_type("scene_blend", (ATTRIBUTE_PARSER)parseSceneBlend));
        mPassAttribParsers.insert(AttribParserList::value_type("separate_scene_blend", (ATTRIBUTE_PARSER)parseSeparateSceneBlend));
        mPassAttribParsers.insert(AttribParserList::value_type("depth_check", (ATTRIBUTE_PARSER)parseDepthCheck));
        mPassAttribParsers.insert(AttribParserList::value_type("depth_write", (ATTRIBUTE_PARSER)parseDepthWrite));
        mPassAttribParsers.insert(AttribParserList::value_type("depth_func", (ATTRIBUTE_PARSER)parseDepthFunc));
        mPassAttribParsers.insert(AttribParserList::value_type("normalise_normals", (ATTRIBUTE_PARSER)parseNormalizeNormals));
        mPassAttribParsers.insert(AttribParserList::value_type("alpha_rejection", (ATTRIBUTE_PARSER)parseAlphaRejection));
        mPassAttribParsers.insert(AttribParserList::value_type("alpha_to_coverage", (ATTRIBUTE_PARSER)parseAlphaToCoverage));
        mPassAttribParsers.insert(AttribParserList::value_type("transparent_sorting", (ATTRIBUTE_PARSER)parseTransparentSorting));
        mPassAttribParsers.insert(AttribParserList::value_type("colour_write", (ATTRIBUTE_PARSER)parseColourWrite));
        mPassAttribParsers.insert(AttribParserList::value_type("light_scissor", (ATTRIBUTE_PARSER)parseLightScissor));
        mPassAttribParsers.insert(AttribParserList::value_type("light_clip_planes", (ATTRIBUTE_PARSER)parseLightClip));
        mPassAttribParsers.insert(AttribParserList::value_type("cull_hardware", (ATTRIBUTE_PARSER)parseCullHardware));
        mPassAttribParsers.insert(AttribParserList::value_type("cull_software", (ATTRIBUTE_PARSER)parseCullSoftware));
        mPassAttribParsers.insert(AttribParserList::value_type("lighting", (ATTRIBUTE_PARSER)parseLighting));
        mPassAttribParsers.insert(AttribParserList::value_type("fog_override", (ATTRIBUTE_PARSER)parseFogging));
        mPassAttribParsers.insert(AttribParserList::value_type("shading", (ATTRIBUTE_PARSER)parseShading));
        mPassAttribParsers.insert(AttribParserList::value_type("polygon_mode", (ATTRIBUTE_PARSER)parsePolygonMode));
        mPassAttribParsers.insert(AttribParserList::value_type("polygon_mode_overrideable", (ATTRIBUTE_PARSER)parsePolygonModeOverrideable));
        mPassAttribParsers.insert(AttribParserList::value_type("depth_bias", (ATTRIBUTE_PARSER)parseDepthBias));
        mPassAttribParsers.insert(AttribParserList::value_type("iteration_depth_bias", (ATTRIBUTE_PARSER)parseIterationDepthBias));
        mPassAttribParsers.insert(AttribParserList::value_type("texture_unit", (ATTRIBUTE_PARSER)parseTextureUnit));
        mPassAttribParsers.insert(AttribParserList::value_type("vertex_program_ref", (ATTRIBUTE_PARSER)parseVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("geometry_program_ref", (ATTRIBUTE_PARSER)parseGeometryProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("shadow_caster_vertex_program_ref", (ATTRIBUTE_PARSER)parseShadowCasterVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("shadow_receiver_vertex_program_ref", (ATTRIBUTE_PARSER)parseShadowReceiverVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("shadow_receiver_fragment_program_ref", (ATTRIBUTE_PARSER)parseShadowReceiverFragmentProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("fragment_program_ref", (ATTRIBUTE_PARSER)parseFragmentProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type("max_lights", (ATTRIBUTE_PARSER)parseMaxLights));
        mPassAttribParsers.insert(AttribParserList::value_type("start_light", (ATTRIBUTE_PARSER)parseStartLight));
        mPassAttribParsers.insert(AttribParserList::value_type("iteration", (ATTRIBUTE_PARSER)parseIteration));
        mPassAttribParsers.insert(AttribParserList::value_type("point_size", (ATTRIBUTE_PARSER)parsePointSize));
        mPassAttribParsers.insert(AttribParserList::value_type("point_sprites", (ATTRIBUTE_PARSER)parsePointSprites));
        mPassAttribParsers.insert(AttribParserList::value_type("point_size_attenuation", (ATTRIBUTE_PARSER)parsePointAttenuation));
        mPassAttribParsers.insert(AttribParserList::value_type("point_size_min", (ATTRIBUTE_PARSER)parsePointSizeMin));
        mPassAttribParsers.insert(AttribParserList::value_type("point_size_max", (ATTRIBUTE_PARSER)parsePointSizeMax));
        mPassAttribParsers.insert(AttribParserList::value_type("illumination_stage", (ATTRIBUTE_PARSER)parseIlluminationStage));

        // texture unit attribute parsers
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("texture_source", (ATTRIBUTE_PARSER)parseTextureSource));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("texture", (ATTRIBUTE_PARSER)parseTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("anim_texture", (ATTRIBUTE_PARSER)parseAnimTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("cubic_texture", (ATTRIBUTE_PARSER)parseCubicTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("binding_type", (ATTRIBUTE_PARSER)parseBindingType));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("tex_coord_set", (ATTRIBUTE_PARSER)parseTexCoord));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("tex_address_mode", (ATTRIBUTE_PARSER)parseTexAddressMode));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("tex_border_colour", (ATTRIBUTE_PARSER)parseTexBorderColour));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("colour_op", (ATTRIBUTE_PARSER)parseColourOp));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("colour_op_ex", (ATTRIBUTE_PARSER)parseColourOpEx));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("colour_op_multipass_fallback", (ATTRIBUTE_PARSER)parseColourOpFallback));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("alpha_op_ex", (ATTRIBUTE_PARSER)parseAlphaOpEx));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("env_map", (ATTRIBUTE_PARSER)parseEnvMap));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("scroll", (ATTRIBUTE_PARSER)parseScroll));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("scroll_anim", (ATTRIBUTE_PARSER)parseScrollAnim));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("rotate", (ATTRIBUTE_PARSER)parseRotate));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("rotate_anim", (ATTRIBUTE_PARSER)parseRotateAnim));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("scale", (ATTRIBUTE_PARSER)parseScale));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("wave_xform", (ATTRIBUTE_PARSER)parseWaveXform));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("transform", (ATTRIBUTE_PARSER)parseTransform));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("filtering", (ATTRIBUTE_PARSER)parseFiltering));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("max_anisotropy", (ATTRIBUTE_PARSER)parseAnisotropy));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("texture_alias", (ATTRIBUTE_PARSER)parseTextureAlias));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("mipmap_bias", (ATTRIBUTE_PARSER)parseMipmapBias));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type("content_type", (ATTRIBUTE_PARSER)parseContentType));
    }

    OgreMaterialSerializer::~OgreMaterialSerializer()
    {

    }

    bool OgreMaterialSerializer::load(const String &path, void *&pData)
    {
        bool ret = false;

        OgreMaterials *materials = new OgreMaterials();

        FileDataStream fs;

        if (fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            mScriptContext.materials = materials;
            ret = parseScript(fs);
            fs.close();
        }

        if (ret)
        {
            pData = materials;
        }
        else
        {
            delete materials;
        }

        return ret;
    }

    bool OgreMaterialSerializer::save(const String &path, void *pData)
    {
        return false;
    }

    bool OgreMaterialSerializer::parseScript(DataStream &stream)
    {
        bool ret = true;
        bool nextIsOpenBrace = false;

        while (!stream.eof())
        {
            String line = stream.readLine();
            mScriptContext.lineNumber++;

            if (!(line.length() == 0 || line.substr(0, 2) == "//"))
            {
                if (nextIsOpenBrace)
                {
                    if (line != "{")
                    {
                        ret = false;
                        break;
                    }
                    else
                    {
                        nextIsOpenBrace = false;
                    }
                }
                else
                {
                    nextIsOpenBrace = parseScriptLine(line);
                }
            }
        }

        if (mScriptContext.section != MSS_NONE)
        {
            ret = false;
        }

        return ret;
    }

    bool OgreMaterialSerializer::parseScriptLine(const String &line)
    {
        bool ret = false;

        switch (mScriptContext.section)
        {
        case MSS_NONE:
            {
                if (line == "}")
                {
                    ret = false;
                }
                else
                {
                    ret = invokeParser(line, mRootAttribParsers);
                }
            }
            break;
        case MSS_MATERIAL:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_NONE;
                    mScriptContext.material = nullptr;
                }
                else
                {
                    ret = invokeParser(line, mMaterialAttribParsers);
                }
            }
            break;
        case MSS_TECHNIQUE:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_MATERIAL;
                    mScriptContext.technique = nullptr;
                }
                else
                {
                    ret = invokeParser(line, mTechniqueAttribParsers);
                }
            }
            break;
        case MSS_PASS:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_TECHNIQUE;
                    mScriptContext.pass = nullptr;
                }
                else
                {
                    ret = invokeParser(line, mPassAttribParsers);
                }
            }
            break;
        case MSS_TEXTUREUNIT:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_PASS;
                    mScriptContext.textureUnit = nullptr;
                }
                else
                {
                    ret = invokeParser(line, mTextureUnitAttribParsers);
                }
            }
            break;
        case MSS_PROGRAM_REF:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_PASS;
                }
                else
                {
                    ret = true;
                }
            }
            break;
        case MSS_PROGRAM:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_NONE;
                }
                else
                {
                    ret = true;
                }
            }
            break;
        case MSS_DEFAULT_PARAMETERS:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_NONE;
                }
                else
                {
                    ret = true;
                }
            }
            break;
        case MSS_TEXTURESOURCE:
            {
                if (line == "}")
                {
                    mScriptContext.section = MSS_TEXTUREUNIT;
                }
                else
                {
                    ret = true;
                }
            }
            break;
        }

        return ret;
    }

    bool OgreMaterialSerializer::invokeParser(const String &line, AttribParserList &parsers)
    {
        bool ret = false;
        StringVector commands = StringUtil::split(line, " \t", 1);

        auto itr = parsers.find(commands[0]);
        if (itr != parsers.end())
        {
            String cmd;
            if (commands.size() >= 2)
                cmd = commands[1];
            ret = itr->second(cmd, mScriptContext);
        }

        return ret;
    }
}