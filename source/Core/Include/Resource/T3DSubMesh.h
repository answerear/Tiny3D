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

#ifndef __T3D_SUBMESH_H__
#define __T3D_SUBMESH_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API SubMesh : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static SubMeshPtr create(const String &name, Material *material, PrimitiveType priType, const Buffer &indices, bool is16Bits);

        ~SubMesh() override;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="Material", RTTRFuncType="getter")
        const String &getMaterialName() const { return mMaterialName; }

        TPROPERTY(RTTRFuncName="Indices", RTTRFuncType="getter")
        const Buffer &getIndices() const { return mIndices; }

        TPROPERTY(RTTRFuncName="Is16Bits", RTTRFuncType="getter")
        bool is16Bits() const { return mIs16Bits; }

        TPROPERTY(RTTRFuncName="PrimitiveType", RTTRFuncType="getter")
        PrimitiveType getPrimitiveType() const { return mPriType; }

        Material    *getMaterial() const { return mMaterial; }
        
        IndexBuffer *getIndexBuffer() const { return mIB; }

        TResult generateRenderResource();

    protected:
        SubMesh() = default;

        SubMesh(const String &name, Material *material, PrimitiveType priType, const Buffer &indices, bool is16Bits);

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="Material", RTTRFuncType="setter")
        void setMaterialName(const String &name) { mMaterialName = name; }
        
        TPROPERTY(RTTRFuncName="Indices", RTTRFuncType="setter")
        void setIndices(const Buffer &indices)
        {
            mIndices.setData(indices.Data, indices.DataSize);
        }
        
        TPROPERTY(RTTRFuncName="Is16Bits", RTTRFuncType="setter")
        void enable16Bits(bool is16Bits) { mIs16Bits = is16Bits; }
        
        TPROPERTY(RTTRFuncName="PrimitiveType", RTTRFuncType="setter")
        void setPrimitiveType(PrimitiveType priType) { mPriType = priType; }

    protected:
        /// 材质对象
        MaterialPtr     mMaterial {nullptr};
        /// 渲染用的 index buffer
        IndexBufferPtr  mIB {nullptr};

        /// 图元类型
        PrimitiveType   mPriType {PrimitiveType::kTriangleList};
        /// 索引数据
        Buffer  mIndices {};
        /// 是否 16 位索引
        bool    mIs16Bits {true};

        /// 名称
        String  mName {};
        
        /// 对应材质名称
        String  mMaterialName {};
    };
}


#endif    /*__T3D_SUBMESH_H__*/
