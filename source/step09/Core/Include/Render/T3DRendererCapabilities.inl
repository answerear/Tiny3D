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

    inline const DriverVersion &RendererCapabilities::getDriverVersion() const
    {
        return mDriverVersion;
    }

    //--------------------------------------------------------------------------

    inline GPUVendor RendererCapabilities::getVendor() const
    {
        return mVendor;
    }

    //--------------------------------------------------------------------------

    inline const String &RendererCapabilities::getDeviceName() const
    {
        return mDeviceName;
    }

    //--------------------------------------------------------------------------

    inline const String &RendererCapabilities::getRendererName() const
    {
        return mRendererName;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getNumTextureUnits() const
    {
        return mNumTextureUnits;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getNumVertexTextureUnits() const
    {
        return mNumVertexTextureUnits;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getStencilBufferBitDepth() const
    {
        return mStencilBufferBitDepth;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getNumVertexBlendMatrices() const
    {
        return mNumVertexBlendMatrices;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getNumMultiRenderTargets() const
    {
        return mNumMultiRenderTargets;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getNumVertexAttributes() const
    {
        return mNumVertexAttributes;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getVSConstantFloatCount() const
    {
        return mVSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getVSConstantIntCount() const
    {
        return mVSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getVSConstantBoolCount() const
    {
        return mVSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getGSConstantFloatCount() const
    {
        return mGSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getGSConstantIntCount() const
    {
        return mGSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getGSConstantBoolCount() const
    {
        return mGSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getPSConstantFloatCount() const
    {
        return mPSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getPSConstantIntCount() const
    {
        return mPSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getPSConstantBoolCount() const
    {
        return mPSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getHSConstantFloatCount() const
    {
        return mHSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getHSConstantIntCount() const
    {
        return mHSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getHSConstantBoolCount() const
    {
        return mHSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getDSConstantFloatCount() const
    {
        return mDSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getDSConstantIntCount() const
    {
        return mDSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getDSConstantBoolCount() const
    {
        return mDSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getCSConstantFloatCount() const
    {
        return mCSConstantFloatCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getCSConstantIntCount() const
    {
        return mCSConstantIntCount;
    }

    //--------------------------------------------------------------------------

    inline uint16_t RendererCapabilities::getCSConstantBoolCount() const
    {
        return mCSConstantBoolCount;
    }

    //--------------------------------------------------------------------------

    inline int32_t RendererCapabilities::getGSNumOutputVertices() const
    {
        return mGSNumOutputVertices;
    }

    //--------------------------------------------------------------------------

    inline Real RendererCapabilities::getMaxPointSize() const
    {
        return mMaxPointSize;
    }

    //--------------------------------------------------------------------------

    inline Real RendererCapabilities::getMaxAnisotropy() const
    {
        return mMaxAnisotropy;
    }

    //--------------------------------------------------------------------------

    inline bool RendererCapabilities::isNPOTLimited() const
    {
        return mNPOTLimited;
    }

    //--------------------------------------------------------------------------

    inline bool RendererCapabilities::hasCapabilities(Capabilities cap) const
    {
        bool ret = false;
        int32_t idx = (CAPS_CATEGORY_MASK & cap) >> CAPS_BITSHIFT;
        if (mCapabilities[idx] & (cap & ~CAPS_CATEGORY_MASK))
            ret = true;
        return ret;
    }

    //--------------------------------------------------------------------------

    inline bool RendererCapabilities::isCapabilityRendererSpecific(
        Capabilities cap) const
    {
        int cat = cap >> CAPS_BITSHIFT;
        if (cat == CapabilitiesCategory::DIRECTX 
            || cat == CapabilitiesCategory::OPENGL
            || cat == CapabilitiesCategory::OPENGLES
            || cat == CapabilitiesCategory::VULKAN
            || cat == CapabilitiesCategory::METAL)
            return true;
        return false;
    }

    //--------------------------------------------------------------------------

    inline void RendererCapabilities::setCapability(Capabilities cap)
    {
        int index = (CAPS_CATEGORY_MASK & cap) >> CAPS_BITSHIFT;
        mCapabilities[index] |= (cap & ~CAPS_CATEGORY_MASK);
    }

    //--------------------------------------------------------------------------

    inline void RendererCapabilities::unsetCapability(Capabilities cap)
    {
        int index = (CAPS_CATEGORY_MASK & cap) >> CAPS_BITSHIFT;
        mCapabilities[index] &= (~cap | CAPS_CATEGORY_MASK);
    }
}

