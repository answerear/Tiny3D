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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    inline const String &Pass::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    inline GPUProgramPtr Pass::getGPUProgram() const
    {
        return mGPUProgram;
    }

    //--------------------------------------------------------------------------

    inline Pass::GPUConstBuffers Pass::getGPUConstBuffers() const
    {
        return mConstBuffers;
    }

    //--------------------------------------------------------------------------

    inline GPUConstBufferPtr Pass::getGPUConstBuffer(uint32_t slot) const
    {
        return mConstBuffers[slot];
    }

    //--------------------------------------------------------------------------

    inline size_t Pass::getGPUConstBufferCount() const
    {
        return mConstBuffers.size();
    }

    //--------------------------------------------------------------------------

    inline size_t Pass::getTextureUnitsCount() const
    {
        return mTextureUnits.size();
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasVertexShader() const
    {
        return (mGPUProgram != nullptr 
            && mGPUProgram->getVertexShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasPixelShader() const
    {
        return (mGPUProgram != nullptr
            && mGPUProgram->getPixelShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasGeometryShader() const
    {
        return (mGPUProgram != nullptr
            && mGPUProgram->getGeometryShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasComputeShader() const
    {
        return (mGPUProgram != nullptr
            && mGPUProgram->getComputeShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasHullShader() const
    {
        return (mGPUProgram != nullptr
            && mGPUProgram->getHullShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline bool Pass::hasDomainShader() const
    {
        return (mGPUProgram != nullptr
            && mGPUProgram->getDomainShader() != nullptr);
    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &Pass::getAmbient() const
    {
        return mAmbient;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setAmbient(const ColorRGBA &color)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setAmbient(Real red, Real green, Real blue)
    {

    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &Pass::getDiffuse() const
    {
        return mDiffuse;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDiffuse(const ColorRGBA &color)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setDiffuse(Real red, Real green, Real blue)
    {

    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &Pass::getSpecular() const
    {
        return mSpecular;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setSpecular(const ColorRGBA &color)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setSpecular(Real red, Real green, Real blue)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getShininess() const
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setShininess(Real value)
    {

    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &Pass::getEmissive() const
    {
        return mEmissive;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setEmissive(const ColorRGBA &color)
    {
        mEmissive = color;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setEmissive(Real red, Real green, Real blue)
    {

    }

    //--------------------------------------------------------------------------

    inline TrackVertexColorType Pass::getVertexColorTracking() const
    {
        return mTracking;
    }

    //--------------------------------------------------------------------------

    void setVertexColorTracking(TrackVertexColorType tracking);

    //--------------------------------------------------------------------------

    BlendFactor getSourceBlendFactor() const;

    //--------------------------------------------------------------------------

    BlendFactor getDestBlendFactor() const;

    //--------------------------------------------------------------------------

    BlendFactor getSourceBlendFactorAlpha() const;

    //--------------------------------------------------------------------------

    BlendFactor getDestBlendFactorAlpha() const;

    //--------------------------------------------------------------------------

    void setSceneBlending(BlendType bt);

    //--------------------------------------------------------------------------

    void setSceneBlending(BlendFactor src, BlendFactor dst);

    //--------------------------------------------------------------------------

    void setSeparateSceneBlending(BlendType bt, BlendType bta);

    //--------------------------------------------------------------------------

    void setSeparateSceneBlending(BlendFactor src, BlendFactor dst,
        BlendFactor srcAlpha, BlendFactor dstAlpha);

    //--------------------------------------------------------------------------

    BlendOperation getSceneBlendingOperation() const;

    //--------------------------------------------------------------------------

    BlendOperation getSceneBlendingOperationAlpha() const;

    //--------------------------------------------------------------------------

    void setSceneBlendingOperation(BlendOperation op);

    //--------------------------------------------------------------------------

    void setSeparateSceneBlendingOperation(BlendOperation op,
        BlendOperation alphaOp);

    //--------------------------------------------------------------------------

    bool isDepthCheckEnabled() const;

    //--------------------------------------------------------------------------

    void setDepthCheckEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isDepthWriteEnabled() const;

    //--------------------------------------------------------------------------

    void setDepthWriteEnabled(bool enabled);

    //--------------------------------------------------------------------------

    CompareFunction getDepthFunction() const;

    //--------------------------------------------------------------------------

    void setDepthFunction(CompareFunction func);

    //--------------------------------------------------------------------------

    Real getDepthBiasConstant() const;

    //--------------------------------------------------------------------------

    Real getDepthBiasSlopeScale() const;

    //--------------------------------------------------------------------------

    void setDepthBias(Real constantBias, Real slopeScaleBias = 0.0f);

    //--------------------------------------------------------------------------

    Real getDepthBiasPerIteration() const;

    //--------------------------------------------------------------------------

    void setDepthBiasPerIteration(Real biasPerIteration);

    //--------------------------------------------------------------------------

    CompareFunction getAlphaRejectFunction() const;

    //--------------------------------------------------------------------------

    void setAlphaRejectFunction(CompareFunction func);

    //--------------------------------------------------------------------------

    uint8_t getAlphaRejectValue() const;

    //--------------------------------------------------------------------------

    void setAlphaRejectValue(uint8_t val);

    //--------------------------------------------------------------------------

    bool isAlphaToCoverageEnabled() const;

    //--------------------------------------------------------------------------

    void setAlphaToCoverageEnabled(bool enabled);

    //--------------------------------------------------------------------------

    void setAlphaRejectSettings(CompareFunction func, uint8_t val,
        bool alphaToCoverageEnabled = false);

    //--------------------------------------------------------------------------

    bool isLightScissoringEnabled() const;

    //--------------------------------------------------------------------------

    void setLightScissoringEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isLightClipPlanesEnabled() const;

    //--------------------------------------------------------------------------

    void setLightClipPlanesEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isLightEnabled() const;

    //--------------------------------------------------------------------------

    void setLightEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isNormalizeNormalsEnabled() const;

    //--------------------------------------------------------------------------

    void setNormalizeNormalsEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isTransparentSortingEnabled() const;

    //--------------------------------------------------------------------------

    void setTransparentSortingEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isTransparentSortingForced() const;

    //--------------------------------------------------------------------------

    void setTransparentSortingForced(bool enabled);

    //--------------------------------------------------------------------------

    bool isColorWriteEnabled() const;

    //--------------------------------------------------------------------------

    void setColorWriteEnabled(bool enabled);

    //--------------------------------------------------------------------------

    bool isPolygonModeOverrideable() const;

    //--------------------------------------------------------------------------

    void setPolygonModeOverrideable(bool overrideable);

    //--------------------------------------------------------------------------

    CullingMode getCullingMode() const;

    //--------------------------------------------------------------------------

    void setCullingMode(CullingMode mode);

    //--------------------------------------------------------------------------

    ManualCullingMode getManualCullingMode() const;

    //--------------------------------------------------------------------------

    void setManualCullingMode(ManualCullingMode mode);

    //--------------------------------------------------------------------------

    IlluminationStage getIlluminationStage() const;

    //--------------------------------------------------------------------------

    void setIlluminationStage(IlluminationStage stage);

    //--------------------------------------------------------------------------

    ShadingMode getShadingMode() const;

    //--------------------------------------------------------------------------

    void setShadingMode(ShadingMode mode);

    //--------------------------------------------------------------------------

    PolygonMode getPolygonMode() const;

    //--------------------------------------------------------------------------

    void setPolygonMode(PolygonMode mode);

    //--------------------------------------------------------------------------

    bool isFogOverrideable() const;

    //--------------------------------------------------------------------------

    FogMode getFogMode() const;

    //--------------------------------------------------------------------------

    const ColorRGBA &getFogColor() const;

    //--------------------------------------------------------------------------

    Real getFogStart() const;

    //--------------------------------------------------------------------------

    Real getFogEnd() const;

    //--------------------------------------------------------------------------

    Real getFogDensity() const;

    //--------------------------------------------------------------------------

    void setFog(bool overrideable, FogMode mode = FogMode::NONE,
        const ColorRGBA &color = ColorRGBA::WHITE, Real density = 0.001f,
        Real linearStart = 0.0f, Real linearEnd = 1.0f);

    //--------------------------------------------------------------------------

    uint16_t getStartLight() const;

    //--------------------------------------------------------------------------

    void setStartLight(uint16_t startLight);

    //--------------------------------------------------------------------------

    uint16_t getLightCountPerIteration() const;

    //--------------------------------------------------------------------------

    void setLightCountPerIteration(uint16_t val);

    //--------------------------------------------------------------------------

    SceneLight::LightType getOnlyLightType() const;

    //--------------------------------------------------------------------------

    bool isIteratePerLight() const;

    //--------------------------------------------------------------------------

    bool isRunOnlyForOneLightType() const;

    //--------------------------------------------------------------------------

    void setIteratePerLight(bool enabled, bool onlyForOneLightType = true,
        SceneLight::LightType lightType = SceneLight::LightType::E_LT_POINT);

    //--------------------------------------------------------------------------

    bool isPointSpritesEnabled() const;

    //--------------------------------------------------------------------------

    void setPointSpritesEnabled(bool enabled);

    //--------------------------------------------------------------------------

    Real getPointSize() const;

    //--------------------------------------------------------------------------

    void setPointSize(Real val);

    //--------------------------------------------------------------------------

    bool isPointAttenuationEnabled() const;

    //--------------------------------------------------------------------------

    Real getPointAttenuationConstant() const;

    //--------------------------------------------------------------------------

    Real getPointAttenuationLinear() const;

    //--------------------------------------------------------------------------

    Real getPointAttenuationQuadratic() const;

    //--------------------------------------------------------------------------

    void setPointAttenuation(bool enabled, Real constant = 0.0f, Real linear = 1.0f, Real quadratic = 0.0f);

    //--------------------------------------------------------------------------

    Real getPointMinSize() const;

    //--------------------------------------------------------------------------

    void setPointMinSize(Real val);

    //--------------------------------------------------------------------------

    Real getPointMaxSize() const;

    //--------------------------------------------------------------------------

    void setPointMaxSize(Real val);
}