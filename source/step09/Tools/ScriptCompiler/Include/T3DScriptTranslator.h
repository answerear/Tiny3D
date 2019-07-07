﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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

#ifndef __T3D_SCRIPT_TRANSLATOR_H__
#define __T3D_SCRIPT_TRANSLATOR_H__


#include "T3DScriptPrerequisites.h"
#include "T3DScriptType.h"
#include "T3DScriptASTree.h"


namespace Tiny3D
{
    class ScriptCompiler;

    //--------------------------------------------------------------------------

    class ScriptTranslator
    {
    public:
        T3D_DECLARE_INTERFACE(ScriptTranslator);

        /**
         * @brief 转换成对应格式，写到数据流对象中
         */
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) = 0;

    protected:
        size_t processNode(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node);

        size_t translateObjectHeader(ObjectAbstractNode *obj, DataStream &stream);

        AbstractNodeList::const_iterator getNodeAt(const AbstractNodeList &nodes, int index);

        bool getBoolean(const AbstractNodePtr &node, bool *result);
        bool getString(const AbstractNodePtr &node, String *result);
        bool getSingle(const AbstractNodePtr &node, float32_t *result);
        bool getDouble(const AbstractNodePtr &node, float64_t *result);
        bool getInt(const AbstractNodePtr &node, int32_t *result);
        bool getUInt(const AbstractNodePtr &node, uint32_t *result);
        bool getHex(const AbstractNodePtr &node, uint32_t *result);
        bool getColor(AbstractNodeList::const_iterator i, AbstractNodeList::const_iterator end, ColorARGB *result, int32_t maxEntries = 4)
    };

    //--------------------------------------------------------------------------

    class MaterialTranslator : public ScriptTranslator
    {
    public:
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) override;

    protected:
        size_t translateLODValues(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateReceiveShadow(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateTransparentCastsShadow(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSetTextuerAlias(PropertyAbstractNode *prop, DataStream &stream);
    };

    //--------------------------------------------------------------------------

    class TechniqueTranslator : public ScriptTranslator
    {
    public:
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) override;

    protected:
        size_t translateScene(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateLODIndex(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateShadowCasterMaterial(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateGPUVendorRule(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateGPUDeviceRule(PropertyAbstractNode *prop, DataStream &stream);
    };

    //--------------------------------------------------------------------------

    class PassTranslator : public ScriptTranslator
    {
    public:
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) override;

    protected:
        size_t translateAmbient(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateDiffuse(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSpecular(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateEmissive(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSceneBlend(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSeparateSceneBlend(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSceneBlendOp(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateSeparateSceneBlendOp(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateDepthCheck(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateDepthWrite(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateDepthBias(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateDepthFunc(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateIterationDepthBias(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateAlphaRejection(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateAlphaToCoverage(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateLightScissor(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateLightClipPlanes(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateTransparentSorting(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateIlluminationStage(PropertyAbstractNode *prop, DataStream &steram);
        size_t translateCullHardware(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateCullSoftware(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateNormalizeNormals(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateLighting(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateShading(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePolygonMode(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePolygonModeOverridable(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateFogOverride(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateColorWrite(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateMaxLights(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateStartLight(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateLightMask(PropertyAbstractNode *prop, DataStream &stream);
        size_t translateIteration(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePointSize(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePointSprite(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePointSizeAttenuation(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePointSizeMin(PropertyAbstractNode *prop, DataStream &stream);
        size_t translatePointSizeMax(PropertyAbstractNode *prop, DataStream &stream);
    };
}


#endif  /*__T3D_SCRIPT_TRANSLATOR_H__*/