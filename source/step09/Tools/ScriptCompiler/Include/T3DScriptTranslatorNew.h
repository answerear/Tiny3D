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

#ifndef __T3D_SCRIPT_TRANSLATOR_H__
#define __T3D_SCRIPT_TRANSLATOR_H__


#include "T3DScriptPrerequisites.h"
#include "T3DScriptType.h"
#include "T3DScriptASTree.h"
#include "protobuf/MaterialScriptObject.pb.h"


namespace Tiny3D
{
    class ScriptCompiler;

    using namespace Script;

    //--------------------------------------------------------------------------

    class ScriptTranslatorNew
    {
    public:
        T3D_DECLARE_INTERFACE(ScriptTranslatorNew);

        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) = 0;

    protected:
        bool processNode(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node);

        bool translateObjectHeader(ObjectAbstractNode *obj, 
            MaterialSystem::Header *header);

        AbstractNodeList::const_iterator getNodeAt(
            const AbstractNodeList &nodes, int32_t index);

        bool getBoolean(const AbstractNodePtr &node, bool *result);
        bool getString(const AbstractNodePtr &node, String *result);
        bool getSingle(const AbstractNodePtr &node, float32_t *result);
        bool getDouble(const AbstractNodePtr &node, float64_t *result);
        bool getInt(const AbstractNodePtr &node, int32_t *result);
        bool getUInt(const AbstractNodePtr &node, uint32_t *result);
        bool getHex(const AbstractNodePtr &node, uint32_t *result);

        bool getColor(AbstractNodeList::const_iterator i, 
            AbstractNodeList::const_iterator end, 
            MaterialSystem::Color *result, 
            int32_t maxEntries = 4);

        bool getMatrix4(AbstractNodeList::const_iterator i, 
            AbstractNodeList::const_iterator end, 
            MaterialSystem::Matrix4 *m);

        bool getInts(AbstractNodeList::const_iterator i, 
            AbstractNodeList::const_iterator end, 
            int32_t *vals, int32_t count);

        bool getSingles(AbstractNodeList::const_iterator i, 
            AbstractNodeList::const_iterator end, 
            float32_t *vals, int32_t count);

        BuiltinType getBuiltinType(const String &name);
    };

    //--------------------------------------------------------------------------

    class MaterialTranslatorNew : public ScriptTranslatorNew
    {
    public:
        virtual bool translate(ScriptCompiler *compiler, void *object, 
            const AbstractNodePtr &node) override;

    protected:
        bool translateLODValues(
            PropertyAbstractNode *prop, MaterialSystem::Material *material);
        bool translateLODStrategy(
            PropertyAbstractNode *prop, MaterialSystem::Material *material);
        bool translateReceiveShadow(
            PropertyAbstractNode *prop, MaterialSystem::Material *material);
        bool translateTransparentCastsShadow(
            PropertyAbstractNode *prop, MaterialSystem::Material *material);
        bool translateSetTextuerAlias(
            PropertyAbstractNode *prop, MaterialSystem::Material *material);
    };

    //--------------------------------------------------------------------------

    class TechniqueTranslatorNew : public ScriptTranslatorNew
    {
    public:
        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) override;

    protected:
        bool translateScheme(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateLODIndex(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateRenderQueue(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateShadowCasterMaterial(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateShadowReceiveMaterial(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateGPUVendorRule(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
        bool translateGPUDeviceRule(
            PropertyAbstractNode *prop, MaterialSystem::Technique *tech);
    };

    //--------------------------------------------------------------------------

    class PassTranslatorNew : public ScriptTranslatorNew
    {
    public:
        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) override;

    protected:
        bool translateAmbient(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateDiffuse(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateSpecular(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateEmissive(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateSceneBlend(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateSeparateSceneBlend(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateSceneBlendOp(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateSeparateSceneBlendOp(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateDepthCheck(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateDepthWrite(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateDepthBias(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateDepthFunc(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateIterationDepthBias(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateAlphaRejection(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateAlphaToCoverage(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateLightScissor(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateLightClipPlanes(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateTransparentSorting(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateIlluminationStage(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateCullHardware(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateCullSoftware(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateNormalizeNormals(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateLighting(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateShading(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePolygonMode(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePolygonModeOverridable(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateFogOverride(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateColorWrite(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateMaxLights(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateStartLight(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateLightMask(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translateIteration(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePointSize(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePointSprites(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePointSizeAttenuation(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePointSizeMin(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
        bool translatePointSizeMax(
            PropertyAbstractNode *prop, MaterialSystem::Pass *pass);
    };

    //--------------------------------------------------------------------------

    class TextureUnitTranslatorNew : public ScriptTranslatorNew
    {
    public:
        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) override;

    protected:
        bool translateSamplerRef(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateTextureAlias(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);

        bool translateTexture(ScriptCompiler *compiler, 
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);

        bool translateAnimTexture(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateCubicTexture(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateTexCoordSet(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateColorOp(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateColorOpEx(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateColorOpMultiPassFallback(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateAlphaOpEx(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateEnvMap(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateScroll(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateScrollAnim(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateRotate(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateRotateAnim(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateScale(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateWaveXform(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateTransform(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateBindingType(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
        bool translateContentType(
            PropertyAbstractNode *prop, MaterialSystem::TextureUnit *unit);
    };

    //--------------------------------------------------------------------------

    class SamplerTranslatorNew : public ScriptTranslatorNew
    {
    public:
        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) override;

        bool translateSamplerParams(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);

    protected:
        bool translateTexAddressMode(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateTexBorderColor(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateFiltering(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateCompareTest(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateCompareFunc(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateMaxAnisotropy(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
        bool translateMipmapBias(
            PropertyAbstractNode *prop, MaterialSystem::Sampler *sampler);
    };

    //--------------------------------------------------------------------------

    class GPUProgramTranslatorNew : public ScriptTranslatorNew
    {
    public:
        GPUProgramTranslatorNew();

        virtual bool translate(ScriptCompiler *compiler, void *object,
            const AbstractNodePtr &node) override;

    protected:
        bool translateGPUProgram(ScriptCompiler *compiler, 
            void *object, ObjectAbstractNode *obj);

        bool translateGPUProgramRef(ScriptCompiler *compiler, 
            void *object, ObjectAbstractNode *obj);

        bool translateShader(ScriptCompiler* compiler,
            void *object, ObjectAbstractNode* obj);

        bool translateGPUCBuffer(ScriptCompiler *compiler, 
            void *object, ObjectAbstractNode *obj);

        bool translateGPUCBufferRef(ScriptCompiler* compiler, 
            void *object, ObjectAbstractNode *obj);

        bool translateSharedParamRef(
            PropertyAbstractNode *prop, MaterialSystem::GPUConstantBuffer *cbuffer);
        bool translateParamIndexed(
            PropertyAbstractNode *prop, MaterialSystem::GPUConstantBuffer *cbuffer);
        bool translateParamIndexedAuto(
            PropertyAbstractNode *prop, MaterialSystem::GPUConstantBuffer *cbuffer);

        bool translateCBuffer(
            PropertyAbstractNode* prop, MaterialSystem::GPUConstantBufferRef *cref);

//         size_t translateGPUProgramEx(ScriptCompiler *compiler, 
//             DataStream &stream, ObjectAbstractNode *obj);
//         size_t translateGPUProgramRefEx(ScriptCompiler *compiler, 
//             DataStream &stream, ObjectAbstractNode *obj);

        void constructBuiltinConstantMap();

        typedef std::map<String, BuiltinConstantDefinition> BuiltinConstantMap;
        typedef BuiltinConstantMap::iterator                BuiltinConstantMapItr;
        typedef BuiltinConstantMap::const_iterator          BuiltinConstantMapConstItr;
        typedef BuiltinConstantMap::value_type              BuiltinConstantMapValue;

        BuiltinConstantMap   mBuiltinConstantMap;
    };
}


#endif  /*__T3D_SCRIPT_TRANSLATOR_H__*/
