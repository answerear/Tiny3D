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

#include "Resource/T3DSubMesh.h"
#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DVertexDeclaration.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SubMeshPtr SubMesh::create(const String &name, PrimitiveType priType, const Buffer &indices, bool is16Bits)
    {
        return new SubMesh(name, priType, indices, is16Bits);
    }

    //--------------------------------------------------------------------------

    SubMesh::SubMesh(const String &name, PrimitiveType priType, const Buffer &indices, bool is16Bits)
        : mPriType(priType)
        , mName(name)
        , mIs16Bits(is16Bits)
    {
        mIndices.setData(indices.Data, indices.DataSize);
    }
    
    //--------------------------------------------------------------------------

    SubMesh::~SubMesh()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult SubMesh::generateRenderResource()
    {
        TResult ret = T3D_OK;

        do
        {
            IndexType indexType;
            size_t indexCount;
            if (mIs16Bits)
            {
                indexType = IndexType::E_IT_16BITS;
                indexCount = mIndices.DataSize / sizeof(uint16_t);
            }
            else
            {
                indexType = IndexType::E_IT_32BITS;
                indexCount = mIndices.DataSize / sizeof(uint32_t);
            }
            
            mIB = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(indexType, indexCount, mIndices, MemoryType::kVRAM, Usage::kImmutable, CPUAccessMode::kCPUNone);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
