/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
            ConstantBufferPtr cbuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(buffer, MemoryType::kVRAM, Usage::kDynamic, CPUAccessMode::kCPUWrite);
            mConstantBuffers.emplace(binding.first, cbuffer);
        }
    }

    //--------------------------------------------------------------------------
}


