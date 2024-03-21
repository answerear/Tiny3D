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


#ifndef __T3D_VERTEX_ATTRIBUTE_H__
#define __T3D_VERTEX_ATTRIBUTE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API VertexAttribute
    {
        TRTTI_FRIEND
        
    public:
        TENUM()
        enum class Type : uint32_t
        {
            E_VAT_FLOAT1 = 0,
            E_VAT_FLOAT2 = 1,
            E_VAT_FLOAT3 = 2,
            E_VAT_FLOAT4 = 3,
            E_VAT_COLOR = 4,
            E_VAT_BYTE4 = 5,
            E_VAT_BYTE4_NORM = 6,
            E_VAT_UBYTE4 = 7,
            E_VAT_UBYTE4_NORM = 8,
            E_VAT_SHORT2 = 9,
            E_VAT_SHORT4 = 10,
            E_VAT_SHORT2_NORM = 11,
            E_VAT_SHORT4_NORM = 12,
            E_VAT_USHORT2 = 13,
            E_VAT_USHORT4 = 14,
            E_VAT_USHORT2_NORM = 15,
            E_VAT_USHORT4_NORM = 16,
            E_VAT_DOUBLE1 = 17,
            E_VAT_DOUBLE2 = 18,
            E_VAT_DOUBLE3 = 19,
            E_VAT_DOUBLE4 = 20,
            E_VAT_INT1 = 21,
            E_VAT_INT2 = 22,
            E_VAT_INT3 = 23,
            E_VAT_INT4 = 24,
            E_VAT_UINT1 = 25,
            E_VAT_UINT2 = 26,
            E_VAT_UINT3 = 27,
            E_VAT_UINT4 = 28,
            E_VAT_FLOAT16_2 = 29,
            E_VAT_FLOAT16_4 = 30,
        };

        TENUM()
        enum class Semantic : uint32_t
        {
            E_VAS_POSITION = 0,     /**< 位置 */
            E_VAS_BLENDWEIGHT = 1,  /**< 骨骼混合权重 */
            E_VAS_BLENDINDICES = 2, /**< 骨骼索引 */
            E_VAS_NORMAL = 3,       /**< 法线 */
            E_VAS_DIFFUSE = 4,      /**< 漫反射值 */
            E_VAS_SPECULAR = 5,     /**< 镜面反射值 */
            E_VAS_TEXCOORD = 6,     /**< 纹理坐标 */
            E_VAS_TANGENT = 7,      /**< 切线 */
            E_VAS_BINORMAL = 8,     /**< 副法线 */
        };

        VertexAttribute() : VertexAttribute(0, 0, Type::E_VAT_FLOAT3, Semantic::E_VAS_POSITION, 0) {}

        VertexAttribute(uint32_t slot, uint32_t offset, Type type, Semantic semantic, uint32_t semanticIndex);

        TPROPERTY(RTTRFuncName="slot", RTTRFuncType="getter")
        uint32_t getSlot() const { return mSlot; }

        TPROPERTY(RTTRFuncName="offset", RTTRFuncType="getter")
        uint32_t getOffset() const { return mOffset; }

        TPROPERTY(RTTRFuncName="type", RTTRFuncType="getter")
        Type getType() const { return mType; }

        TPROPERTY(RTTRFuncName="semantic", RTTRFuncType="getter")
        Semantic getSemantic() const { return mSemantic; }

        TPROPERTY(RTTRFuncName="semanticIndex", RTTRFuncType="getter")
        uint32_t getSemanticIndex() const { return mSemanticIndex; }

        uint32_t getSize() const;

    protected:
        TPROPERTY(RTTRFuncName="slot", RTTRFuncType="setter")
        void setSlot(uint32_t slot) { mSlot = slot; }

        TPROPERTY(RTTRFuncName="offset", RTTRFuncType="setter")
        void setOffset(uint32_t offset) { mOffset = offset; }

        TPROPERTY(RTTRFuncName="type", RTTRFuncType="setter")
        void setType(Type type) { mType = type; }

        TPROPERTY(RTTRFuncName="semantic", RTTRFuncType="setter")
        void setSemantic(Semantic semantic) { mSemantic = semantic; }

        TPROPERTY(RTTRFuncName="semanticIndex", RTTRFuncType="setter")
        void setSemanticIndex(uint32_t index) { mSemanticIndex = index; }
        
    protected:
        /// 顶点属性数据类型
        Type        mType {Type::E_VAT_FLOAT4};
        /// 顶点属性语义
        Semantic    mSemantic {Semantic::E_VAS_POSITION};
        /// 顶点属性定义的在顶点缓存中的偏移值
        uint32_t    mOffset {0};
        /// 对应定义的顶点缓存索引
        uint32_t    mSlot {0};
        /// 顶点语义索引
        uint32_t    mSemanticIndex {0};
    };
}


#endif  /*__T3D_ERTEX_ATTRIBUTE_H__*/