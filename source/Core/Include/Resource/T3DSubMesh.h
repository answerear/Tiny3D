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
    /**
     * \brief 网格子段，持有索引缓冲、图元类型及关联材质 UUID
     */
    TCLASS()
    class T3D_ENGINE_API SubMesh : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建 SubMesh 对象
         * \param [in] name : 子网格名称
         * \param [in] materialUUID : 关联材质 UUID
         * \param [in] priType : 图元类型
         * \param [in] indices : 索引数据缓冲（移动语义）
         * \param [in] is16Bits : 是否为 16 位索引
         * \return 新建的 SubMesh 智能指针
         */
        static SubMeshPtr create(const String &name, const UUID &materialUUID, PrimitiveType priType, Buffer &&indices, bool is16Bits);

        /// 析构；若索引数据未交给 IndexBuffer 则由本对象释放
        ~SubMesh() override;

        /// 获取子网格名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取关联材质 UUID
        TPROPERTY(RTTRFuncName="Material", RTTRFuncType="getter")
        const UUID &getMaterialUUID() const { return mMaterialUUID; }

        /// 获取 CPU 端索引数据缓冲
        TPROPERTY(RTTRFuncName="Indices", RTTRFuncType="getter")
        const Buffer &getIndices() const { return mIndices; }

        /// 是否为 16 位索引
        TPROPERTY(RTTRFuncName="Is16Bits", RTTRFuncType="getter")
        bool is16Bits() const { return mIs16Bits; }

        /// 获取图元类型
        TPROPERTY(RTTRFuncName="PrimitiveType", RTTRFuncType="getter")
        PrimitiveType getPrimitiveType() const { return mPriType; }

        // Material    *getMaterial() const { return mMaterial; }
        
        /// 获取 GPU 索引缓冲；未 generateRenderResource 时为 nullptr
        IndexBuffer *getIndexBuffer() const { return mIB; }

        /**
         * \brief 创建 GPU 索引缓冲
         * \param [in] archive : 档案对象（当前实现未使用）
         * \return 调用成功返回 T3D_OK
         */
        TResult generateRenderResource(Archive *archive);

    protected:
        /// 默认构造
        SubMesh() = default;

        /**
         * \brief 构造 SubMesh
         * \param [in] name : 子网格名称
         * \param [in] materialUUID : 材质 UUID
         * \param [in] priType : 图元类型
         * \param [in] indices : 索引数据
         * \param [in] is16Bits : 是否 16 位索引
         */
        SubMesh(const String &name, const UUID &materialUUID, PrimitiveType priType, Buffer &&indices, bool is16Bits);

        /// 设置子网格名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /// 设置关联材质 UUID
        TPROPERTY(RTTRFuncName="Material", RTTRFuncType="setter")
        void setMaterialUUID(const UUID &uuid) { mMaterialUUID = uuid; }
        
        /**
         * \brief 设置索引数据（浅拷贝）
         * \param [in] indices : 新索引缓冲；与旧 Data 不同时先 release 旧块
         */
        TPROPERTY(RTTRFuncName="Indices", RTTRFuncType="setter")
        void setIndices(const Buffer &indices);

        /// 设置是否使用 16 位索引
        TPROPERTY(RTTRFuncName="Is16Bits", RTTRFuncType="setter")
        void enable16Bits(bool is16Bits) { mIs16Bits = is16Bits; }
        
        /// 设置图元类型
        TPROPERTY(RTTRFuncName="PrimitiveType", RTTRFuncType="setter")
        void setPrimitiveType(PrimitiveType priType) { mPriType = priType; }

    protected:
        /// GPU 索引缓冲
        IndexBufferPtr  mIB {nullptr};

        /// 图元类型
        PrimitiveType   mPriType {PrimitiveType::kTriangleList};
        /// CPU 索引数据
        Buffer  mIndices {};
        /// 是否 16 位索引
        bool    mIs16Bits {true};

        /// 子网格名称
        String  mName {};
        
        /// 关联材质 UUID
        UUID    mMaterialUUID {UUID::INVALID};
    };
}


#endif    /*__T3D_SUBMESH_H__*/
