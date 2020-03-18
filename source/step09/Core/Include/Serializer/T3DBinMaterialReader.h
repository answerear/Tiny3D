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

#ifndef __T3D_BIN_MATERIAL_READER_H__
#define __T3D_BIN_MATERIAL_READER_H__


#include "Serializer/T3DMaterialReader.h"
#include "Kernel/T3DCommon.h"


namespace Tiny3D
{
    namespace Script
    {
        namespace MaterialSystem
        {
            class Material;
            class Technique;
            class Pass;
            class GPUConstantBufferRef;
            class TextureUnit;
            class Sampler;
            class GPUConstantBuffer;
            class GPUProgram;
            class Param;
            class ParamAuto;
            class Shader;
        }
    }

    using namespace Script;

    /**
     * @class   BinMaterialReader
     * @brief   A bin material reader.
     */
    class BinMaterialReader : public MaterialReader
    {
        T3D_DECLARE_CLASS();
        T3D_DISABLE_COPY(BinMaterialReader);

    public:
        /**
         * @fn  static BinMaterialReaderPtr BinMaterialReader::create(
         *      const BuiltinConstantMap &definitions);
         * @brief   Creates a new BinMaterialReaderPtr
         * @param   definitions The definitions.
         * @return  A BinMaterialReaderPtr.
         */
        static BinMaterialReaderPtr create(const BuiltinConstantMap &definitions);

        /**
         * @fn  virtual BinMaterialReader::~BinMaterialReader();
         * @brief   Destructor
         */
        virtual ~BinMaterialReader();

    protected:
        /**
         * @fn  BinMaterialReader::BinMaterialReader(
         *      const BuiltinConstantMap &definitions);
         * @brief   Constructor
         * @param   definitions The definitions.
         */
        BinMaterialReader(const BuiltinConstantMap &definitions);

        /**
         * @fn  virtual TResult BinMaterialReader::parse(DataStream &stream, 
         *      Material *material) override;
         * @brief   Parses
         * @param [in,out]  stream      The stream.
         * @param [in,out]  material    If non-null, the material.
         * @return  A TResult.
         */
        virtual TResult parse(DataStream &stream, Material *material) override;

        /**
         * @fn  TResult BinMaterialReader::parseMaterial(
         *      const MaterialSystem::Material *src, Material *dst);
         * @brief   Parse material
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseMaterial(
            const MaterialSystem::Material *src, Material *dst);

        /**
         * @fn  TResult BinMaterialReader::parseTechnique(
         *      const MaterialSystem::Technique *src, Material *dst);
         * @brief   Parse technique
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseTechnique(
            const MaterialSystem::Technique *src, Material *dst);

        /**
         * @fn  TResult BinMaterialReader::parsePass(
         *      const MaterialSystem::Pass *src, Technique *dst);
         * @brief   Parse pass
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parsePass(
            const MaterialSystem::Pass *src, Technique *dst);

        /**
         * @fn  TResult BinMaterialReader::parseAmbient(
         *      const MaterialSystem::Pass *src, Pass *dst);
         * @brief   Parse ambient
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseAmbient(
            const MaterialSystem::Pass *src, Pass *dst);

        /**
         * @fn  TResult BinMaterialReader::parseDiffuse(
         *      const MaterialSystem::Pass *src, Pass *dst);
         * @brief   Parse diffuse
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseDiffuse(
            const MaterialSystem::Pass *src, Pass *dst);

        /**
         * @fn  TResult BinMaterialReader::parseSpecular(
         *      const MaterialSystem::Pass *src, Pass *dst);
         * @brief   Parse specular
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseSpecular(
            const MaterialSystem::Pass *src, Pass *dst);

        /**
         * @fn  TResult BinMaterialReader::parseEmissive(
         *      const MaterialSystem::Pass *src, Pass *dst);
         * @brief   Parse emissive
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseEmissive(
            const MaterialSystem::Pass *src, Pass *dst);

        /**
         * @fn  TResult BinMaterialReader::parseSceneBlend(
         *      const MaterialSystem::Pass *src, Pass *dst);
         * @brief   Parse scene blend
         * @param           src Source for the.
         * @param [in,out]  dst If non-null, destination for the.
         * @return  A TResult.
         */
        TResult parseSceneBlend(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseSeparateSceneBlend(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseSceneBlendOp(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseSeparateSceneBlendOp(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseDepthCheck(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseDepthWrite(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseDepthFunc(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseDepthBias(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseIterationDepthBias(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseAlphaRejection(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseAlpha2Coverage(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseLightScissor(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseLightClipPlanes(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseIlluminationStage(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseNormalizeNormals(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseTransparentSorting(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseCullHardware(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseCullSoftware(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseLighting(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseShading(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePolygonMode(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePolygonModeOverridable(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseFog(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseColorWrite(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseMaxLights(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseStartLight(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseIteration(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePointSize(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePointSprites(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePointSizeAttenuation(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePointSizeMin(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parsePointSizeMax(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseGPUProgramRef(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseTextures(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseGPUConstantBufferRef(
            const MaterialSystem::GPUConstantBufferRef *src, GPUProgramRef *dst);

        TResult parseTextureUnit(
            const MaterialSystem::TextureUnit *src, Pass *dst);

        TResult parseTextureAlias(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseTexture(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseAnimTexture(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseCubicTexture(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseBindingType(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseTexCoordSet(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseColorOp(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseSamplerRef(
            const MaterialSystem::TextureUnit *src, TextureUnit *dst);

        TResult parseSampler(
            const MaterialSystem::Sampler *src, Material *dst);

        TResult parseTexAddressMode(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseTexBorderColor(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseFiltering(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseCompareTest(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseCompareFunc(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseMaxAnisotropy(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseMipmapBias(
            const MaterialSystem::Sampler *src, Sampler *dst);

        TResult parseGPUConstantBuffer(
            const MaterialSystem::GPUConstantBuffer *src, Material *dst);

        TResult parseParam(
            const MaterialSystem::Param *src, GPUConstBuffer *dst, bool named);

        TResult parseParamAuto(
            const MaterialSystem::ParamAuto *src, GPUConstBuffer *dst, bool named);

        TResult parseGPUProgram(
            const MaterialSystem::GPUProgram *src, Material *dst);

        TResult parseShader(
            const MaterialSystem::Shader *src, GPUProgram *dst);

    protected:
        const BuiltinConstantMap    &mDefinitions;
    };
}


#endif    /*__T3D_BIN_MATERIAL_READER_H__*/
