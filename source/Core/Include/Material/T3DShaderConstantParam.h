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

#ifndef __T3D_SHADER_CONSTANT_PARAM_H__
#define __T3D_SHADER_CONSTANT_PARAM_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 着色器里的常量
     */
    TCLASS()
    class T3D_ENGINE_API ShaderConstantParam
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 数据类型
         */
        TENUM()
        enum class DATA_TYPE : uint32_t
        {
            /// 浮点数
            DT_FLOAT = 0,
            /// 布尔值
            DT_BOOL,
            /// 整型
            DT_INTEGER,
            /// 颜色
            DT_COLOR,
            /// 4D 向量
            DT_VECTOR4,
            /// 4D 方阵
            DT_MATRIX4,
            /// 结构体
            DT_STRUCT,
            DT_MAX
        };

        /**
         * \brief 创建对象函数
         * \param [in] name : 变量名称
         * \param [in] data : 数据地址
         * \param [in] dataSize : 数据大小
         * \param [in] dataOffset : 数据在 cbuffer 中的偏移
         * \param [in] dataType : 数据类型
         */
        static ShaderConstantParamPtr create(const String &name, const void* data, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);

        /**
         * \brief Destructor
         */
        ~ShaderConstantParam() override;

        ShaderConstantParamPtr clone() const;

        /**
         * \brief 获取变量名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /**
         * \brief 设置数据
         * \param [in] data : 数据地址 
         * \return 调用成功返回 T3D_OK
         */
        TResult setData(const void *data)
        {
            if (data == nullptr)
                return T3D_ERR_INVALID_PARAM;
            memcpy(mData.Data, data, mData.DataSize);
            return T3D_OK;
        }

        /**
         * \brief 获取变量数据
         */
        void *getData() const { return mData.Data; }

        /**
         * \brief 获取数据类型
         */
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="getter")
        DATA_TYPE getDataType() const { return mDataType; }
        
        /**
         * \brief 获取数据大小
         */
        uint32_t getSize() const { return mData.DataSize; }

        /**
         * \brief 获取数据在 cbuffer 中的偏移
         */
        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="getter")
        uint32_t getDataOffset() const { return mDataOffset; }

    private:
        ShaderConstantParam() = default;

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="getter")
        const Buffer &getBuffer() const
        {
            return mData;
        }

        TPROPERTY(RTTRFuncName="Data", RTTRFuncType="setter")
        void setBuffer(const Buffer &buffer)
        {
            mData = buffer;
        }

        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="setter")
        void setDataType(DATA_TYPE type)
        {
            mDataType = type;
        }

        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="setter")
        void setDataOffset(uint32_t offset)
        {
            mDataOffset = offset;
        }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }
        
    protected:
        /**
         * \brief 构造函数
         * \param [in] name : 变量名称
         * \param [in] data : 数据地址
         * \param [in] dataSize : 数据大小
         * \param [in] dataOffset : 数据在 cbuffer 中的偏移
         * \param [in] dataType : 数据类型
         */
        ShaderConstantParam(const String &name, const void* data, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);
        
        /// 数据
        Buffer      mData {};
        /// 数据类型
        DATA_TYPE   mDataType {DATA_TYPE::DT_FLOAT};
        /// 数据在 cbuffer 中的偏移
        uint32_t    mDataOffset {0};
        /// 变量名 
        String      mName {};
    };

    using ShaderConstantParams = TMap<String, ShaderConstantParamPtr>;
}


#endif    /*__T3D_SHADER_CONSTANT_PARAM_H__*/
