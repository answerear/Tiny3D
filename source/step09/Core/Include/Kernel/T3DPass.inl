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
        return mShininess;
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

    inline void Pass::setVertexColorTracking(TrackVertexColorType tracking)
    {

    }

    //--------------------------------------------------------------------------

    inline BlendFactor Pass::getSourceBlendFactor() const
    {
        return mSrcBlendFactor;
    }

    //--------------------------------------------------------------------------

    inline BlendFactor Pass::getDestBlendFactor() const
    {
        return mDstBlendFactor;
    }

    //--------------------------------------------------------------------------

    inline BlendFactor Pass::getSourceBlendFactorAlpha() const
    {
        return mSrcBlendFactorAlpha;
    }

    //--------------------------------------------------------------------------

    inline BlendFactor Pass::getDestBlendFactorAlpha() const
    {
        return mDstBlendFactorAlpha;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setSceneBlending(BlendType bt)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setSceneBlending(BlendFactor src, BlendFactor dst)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setSeparateSceneBlending(BlendType bt, BlendType bta)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setSeparateSceneBlending(BlendFactor src, BlendFactor dst,
        BlendFactor srcAlpha, BlendFactor dstAlpha)
    {

    }

    //--------------------------------------------------------------------------

    inline BlendOperation Pass::getSceneBlendingOperation() const
    {
        return mBlendOperation;
    }

    //--------------------------------------------------------------------------

    inline BlendOperation Pass::getSceneBlendingOperationAlpha() const
    {
        return mAlphaBlendOperation;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setSceneBlendingOperation(BlendOperation op)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setSeparateSceneBlendingOperation(BlendOperation op,
        BlendOperation alphaOp)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isDepthCheckEnabled() const
    {
        return mDepthCheck;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDepthCheckEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isDepthWriteEnabled() const
    {
        return mDepthWrite;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDepthWriteEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline CompareFunction Pass::getDepthFunction() const
    {
        return mDepthFunc;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDepthFunction(CompareFunction func)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getDepthBiasConstant() const
    {
        return mDepthBiasConstant;
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getDepthBiasSlopeScale() const
    {
        return mDepthBiasSlopeScale;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDepthBias(Real constantBias, 
        Real slopeScaleBias/* = 0.0f*/)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getDepthBiasPerIteration() const
    {
        return mDepthBiasPerIteration;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setDepthBiasPerIteration(Real biasPerIteration)
    {

    }

    //--------------------------------------------------------------------------

    inline CompareFunction Pass::getAlphaRejectFunction() const
    {
        return mAlphaRejectFunc;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setAlphaRejectFunction(CompareFunction func)
    {

    }

    //--------------------------------------------------------------------------

    inline uint8_t Pass::getAlphaRejectValue() const
    {
        return mAlphaRejectVal;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setAlphaRejectValue(uint8_t val)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isAlphaToCoverageEnabled() const
    {
        return mAlpha2CoverageEnabled;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setAlphaToCoverageEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline void Pass::setAlphaRejectSettings(CompareFunction func, uint8_t val,
        bool alphaToCoverageEnabled/* = false*/)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isLightScissoringEnabled() const
    {
        return mLightScissor;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setLightScissoringEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isLightClipPlanesEnabled() const
    {
        return mLightClipPlanes;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setLightClipPlanesEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isLightEnabled() const
    {
        return mLightingEnabled;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setLightEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isNormalizeNormalsEnabled() const
    {
        return mNormalizeNormals;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setNormalizeNormalsEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isTransparentSortingEnabled() const
    {
        return mTransparentSorting;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setTransparentSortingEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isTransparentSortingForced() const
    {
        return mTransparentSortingForced;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setTransparentSortingForced(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isColorWriteEnabled() const
    {
        return mColorWrite;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setColorWriteEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isPolygonModeOverrideable() const
    {
        return mPolygonModeOverrideable;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPolygonModeOverrideable(bool overrideable)
    {

    }

    //--------------------------------------------------------------------------

    inline CullingMode Pass::getCullingMode() const
    {
        return mCullMode;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setCullingMode(CullingMode mode)
    {

    }

    //--------------------------------------------------------------------------

    inline ManualCullingMode Pass::getManualCullingMode() const
    {
        return mManualCullMode;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setManualCullingMode(ManualCullingMode mode)
    {

    }

    //--------------------------------------------------------------------------

    inline IlluminationStage Pass::getIlluminationStage() const
    {
        return mIlluminationStage;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setIlluminationStage(IlluminationStage stage)
    {

    }

    //--------------------------------------------------------------------------

    inline ShadingMode Pass::getShadingMode() const
    {
        return mShadingMode;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setShadingMode(ShadingMode mode)
    {

    }

    //--------------------------------------------------------------------------

    inline PolygonMode Pass::getPolygonMode() const
    {
        return mPolygonMode;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPolygonMode(PolygonMode mode)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isFogOverrideable() const
    {
        return mFogOverride;
    }

    //--------------------------------------------------------------------------

    inline FogMode Pass::getFogMode() const
    {
        return mFogMode;
    }

    //--------------------------------------------------------------------------

    inline const ColorRGBA &Pass::getFogColor() const
    {
        return mFogColor;
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getFogStart() const
    {
        return mFogStart;
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getFogEnd() const
    {
        return mFogEnd;
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getFogDensity() const
    {
        return mFogDensity;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setFog(bool overrideable, FogMode mode/* = FogMode::NONE*/,
        const ColorRGBA &color/* = ColorRGBA::WHITE*/, Real density/* = 0.001f*/,
        Real linearStart/* = 0.0f*/, Real linearEnd/* = 1.0f*/)
    {

    }

    //--------------------------------------------------------------------------

    inline uint16_t Pass::getStartLight() const
    {
        return mStartLight;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setStartLight(uint16_t startLight)
    {

    }

    //--------------------------------------------------------------------------

    inline uint16_t Pass::getLightCountPerIteration() const
    {
        return mLightsPerIteration;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setLightCountPerIteration(uint16_t val)
    {

    }

    //--------------------------------------------------------------------------

    inline SceneLight::LightType Pass::getOnlyLightType() const
    {
        return mOnlyLightType;
    }

    //--------------------------------------------------------------------------

    inline bool Pass::isIteratePerLight() const
    {
        return mIteratePerLight;
    }

    //--------------------------------------------------------------------------

    inline bool Pass::isRunOnlyForOneLightType() const
    {
        return mRunOnlyForOneLightType;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setIteratePerLight(bool enabled, 
        bool onlyForOneLightType/* = true*/,
        SceneLight::LightType lightType/* = SceneLight::LightType::E_LT_POINT*/)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isPointSpritesEnabled() const
    {
        return mPointSpritesEnabled;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPointSpritesEnabled(bool enabled)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointSize() const
    {
        return mPointSize;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPointSize(Real val)
    {

    }

    //--------------------------------------------------------------------------

    inline bool Pass::isPointAttenuationEnabled() const
    {
        return mPointAttenuationEnabled;
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointAttenuationConstant() const
    {
        return mPointAttenuationCoeffs[0];
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointAttenuationLinear() const
    {
        return mPointAttenuationCoeffs[1];
    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointAttenuationQuadratic() const
    {
        return mPointAttenuationCoeffs[2];
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPointAttenuation(bool enabled, Real constant/* = 0.0f*/,
        Real linear/* = 1.0f*/, Real quadratic/* = 0.0f*/)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointMinSize() const
    {
        return mPointMinSize;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPointMinSize(Real val)
    {

    }

    //--------------------------------------------------------------------------

    inline Real Pass::getPointMaxSize() const
    {
        return mPointMaxSize;
    }

    //--------------------------------------------------------------------------

    inline void Pass::setPointMaxSize(Real val)
    {

    }
}