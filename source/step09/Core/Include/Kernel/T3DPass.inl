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
}