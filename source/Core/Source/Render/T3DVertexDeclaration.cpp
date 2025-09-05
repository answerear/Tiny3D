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


#include "Render/T3DVertexDeclaration.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIVertexDeclaration.h"
#include "Material/T3DShaderVariant.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VertexDeclarationPtr VertexDeclaration::create(const VertexAttributes &attributes, ShaderVariant *vertexShader)
    {
        return T3D_NEW VertexDeclaration(attributes, vertexShader);
    }

    //--------------------------------------------------------------------------

    VertexDeclaration::VertexDeclaration(const VertexAttributes &attributes, ShaderVariant *vertexShader)
        : mVertexAttributes(attributes)
        , mVertexShader(vertexShader)
    {

    }

    //--------------------------------------------------------------------------

    VertexDeclaration::~VertexDeclaration()
    {

    }

    //--------------------------------------------------------------------------

    RenderResource::Type VertexDeclaration::getType() const 
    { 
        return Type::kVertexDeclaration;
    }

    //--------------------------------------------------------------------------

    uint32_t VertexDeclaration::getVertexSize(uint32_t source) const
    {
        size_t s = 0;
        auto itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (source == itr->getSlot())
            {
                s += itr->getSize();
            }

            ++itr;
        }

        return s;
    }

    //--------------------------------------------------------------------------

    uint32_t VertexDeclaration::hash()
    {
        // 数据更新了，重新计算 hash 值
        mHash = CRC::crc32((uint8_t*)mVertexAttributes.data(), (uint32_t)mVertexAttributes.size() * sizeof(VertexAttribute));        
        return mHash;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onLoad() 
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createVertexDeclaration(this);
        return true;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onUnload() 
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------
}

