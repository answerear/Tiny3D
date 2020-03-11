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


namespace Tiny3D
{
    namespace Script
    {
        namespace MaterialSystem
        {
            class Material;
            class Technique;
            class Pass;
        }
    }

    using namespace Script;

    class BinMaterialReader : public MaterialReader
    {
        T3D_DECLARE_CLASS();
        T3D_DISABLE_COPY(BinMaterialReader);

    public:
        static BinMaterialReaderPtr create();

        virtual ~BinMaterialReader();

    protected:
        BinMaterialReader();

        virtual TResult parse(DataStream &stream, Material *material) override;

        virtual TResult parse(
            const uint8_t *data, size_t size, Material *material) override;

        TResult parseMaterial(
            const MaterialSystem::Material *src, Material *dst);

        TResult parseTechnique(
            const MaterialSystem::Technique *src, Material *dst);

        TResult parsePass(
            const MaterialSystem::Pass *src, Technique *dst);

        TResult parseAmbient(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseDiffuse(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseSpecular(
            const MaterialSystem::Pass *src, Pass *dst);

        TResult parseEmissive(
            const MaterialSystem::Pass *src, Pass *dst);

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
    };
}


#endif    /*__T3D_BIN_MATERIAL_READER_H__*/
