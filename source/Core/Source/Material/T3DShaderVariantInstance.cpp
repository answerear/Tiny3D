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


#include "Material/T3DShaderVariantInstance.h"
#include "Material/T3DShaderVariant.h"
#include "Render/T3DRenderResourceManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderVariantInstancePtr ShaderVariantInstance::create(ShaderVariantPtr shaderVariant)
    {
        return new ShaderVariantInstance(shaderVariant);
    }

    //--------------------------------------------------------------------------

    ShaderVariantInstance::ShaderVariantInstance(ShaderVariantPtr shaderVariant)
        : mShaderVariant(shaderVariant)
    {
        static Buffer buffer;
        
        auto reallocate = [](uint8_t *&data, uint32_t &dataSize, uint32_t newSize)
        {
            if (dataSize < newSize)
            {
                T3D_SAFE_DELETE_ARRAY(data);
                dataSize = newSize;
                data = new uint8_t[dataSize];
            }
            else
            {
                dataSize = newSize;
            }
        };
        
        for (const auto &binding : mShaderVariant->getShaderConstantBindings())
        {
            reallocate(buffer.Data, (uint32_t &)buffer.DataSize, binding.second.size);
            ConstantBufferPtr cbuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(buffer, MemoryType::kVRAM, Usage::kStatic, CPUAccessMode::kCPUNone);
            mConstantBuffers.emplace(binding.first, cbuffer);
        }
    }

    //--------------------------------------------------------------------------
}


