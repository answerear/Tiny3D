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
         * \param [in] dataType : 数据类型
         */
        static ShaderConstantParamPtr create(const String &name, const void* data, uint32_t dataSize, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);

        /**
         * \brief Destructor
         */
        ~ShaderConstantParam() override;

        ShaderConstantParamPtr clone() const;

        /**
         * \brief 获取数据在 cbuffer 中的偏移
         */
        uint32_t getDataOffset() const { return mDataOffset; }

        void setDataOffset(uint32_t offset) { mDataOffset = offset; }

        ConstantBufferPtr getConstantBuffer() const { return mConstantBuffer; }

        void setConstantBuffer(ConstantBufferPtr cbuffer) { mConstantBuffer = cbuffer; }
        
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
        uint32_t getDataSize() const { return (uint32_t)mData.DataSize; }
    
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
         * \param [in] dataType : 数据类型
         */
        ShaderConstantParam(const String &name, const void* data, uint32_t dataSize, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);
        
        /// 数据
        Buffer      mData {};
        /// 数据类型
        DATA_TYPE   mDataType {DATA_TYPE::DT_FLOAT};
        /// 数据在 cbuffer 中的偏移
        uint32_t    mDataOffset {0};
        /// 变量名 
        String      mName {};

        /// 对应的常量缓冲区
        ConstantBufferPtr   mConstantBuffer {nullptr};
        
        /// 是否需要更新 GPU 缓冲区数据
        bool        mIsDirty {false};
    };
}


#endif    /*__T3D_SHADER_CONSTANT_PARAM_H__*/
