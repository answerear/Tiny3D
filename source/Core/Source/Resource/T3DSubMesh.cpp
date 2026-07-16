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
#include "T3DErrorDef.h"
#include "Render/T3DRenderResourceManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Render/T3DIndexBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SubMeshPtr SubMesh::create(const String &name, const UUID &materialUUID, PrimitiveType priType, Buffer &&indices, bool is16Bits)
    {
        return T3D_NEW SubMesh(name, materialUUID, priType, std::move(indices), is16Bits);
    }

    //--------------------------------------------------------------------------

    SubMesh::SubMesh(const String &name, const UUID &materialUUID, PrimitiveType priType, Buffer &&indices, bool is16Bits)
        : mPriType(priType)
        , mIndices(std::move(indices))
        , mIs16Bits(is16Bits)
        , mName(name)
        , mMaterialUUID(materialUUID)
    {
        
    }
    
    //--------------------------------------------------------------------------

    SubMesh::~SubMesh()
    {
        // 索引数据所有权与 Mesh 顶点数据一致：kVRAM 等非 kBoth 模式下 RenderBuffer
        // 直接接管 mIndices.Data 指针（见 RenderBuffer 构造），其数据由 index buffer
        // 析构时释放，SubMesh 不再重复释放；其余情况（例如无渲染设备的工具/离线环境，
        // 反序列化后未调用 generateRenderResource 生成渲染资源，mIB 为空）则由 SubMesh
        // 释放自身持有的索引数据，避免泄漏。
        const bool ownedByIB = (mIB != nullptr)
            && (mIndices.Data == mIB->getBuffer().Data);

        if (!ownedByIB)
        {
            mIndices.release();
        }
    }

    //--------------------------------------------------------------------------

    void SubMesh::setIndices(const Buffer &indices)
    {
        // 浅拷贝接管指针前先丢掉旧块，避免重复 set 时泄漏上一份 Data。
        if (mIndices.Data != indices.Data)
        {
            mIndices.release();
        }
        mIndices = indices;
    }

    //--------------------------------------------------------------------------

    TResult SubMesh::generateRenderResource(Archive *archive)
    {
        TResult ret = T3D_OK;

        do
        {
            // 加载材质资源
            // if (archive != nullptr)
            // {
            //     mMaterial = T3D_MATERIAL_MGR.loadMaterial(archive, mMaterialUUID);
            //     if (mMaterial == nullptr)
            //     {
            //         ret = T3D_ERR_RES_LOAD_FAILED;
            //         T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to load material (%s) !", mMaterialUUID.toString().c_str());
            //         break;
            //     }
            // }
            
            // 索引缓冲区
            IndexType indexType;
            uint32_t indexCount;
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
