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
            /// 浮点数组
            DT_FLOAT_ARRAY,
            /// 布尔值
            DT_BOOL,
            /// 布尔数组
            DT_BOOL_ARRAY,
            /// 整型
            DT_INTEGER,
            /// 整型数组
            DT_INTEGER_ARRAY,
            /// 颜色
            DT_COLOR,
            /// 颜色数组
            DT_COLOR_ARRAY,
            /// 4D 向量
            DT_VECTOR4,
            /// 4D 向量数组
            DT_VECTOR4_ARRAY,
            /// 4D 方阵
            DT_MATRIX4,
            /// 4D 方阵数组
            DT_MATRIX4_ARRAY,
            /// 结构体
            DT_STRUCT,
            /// 最大值，所有枚举不能超过该值
            DT_MAX
        };

        /**
         * @brief 创建对象函数
         * @param [in] cbufferName : 常量缓冲区名称
         * @param [in] name : 变量名称
         * @param [in] bindingPoint : 绑定到的 constant buffer 的绑定点
         * @param [in] dataSize : 数据大小
         * @param [in] dataOffset : 缓冲区中的偏移
         * @param [in] dataType : 数据类型
         */
        static ShaderConstantParamPtr create(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType);

        /**
         * \brief Destructor
         */
        ~ShaderConstantParam() override;

        ShaderConstantParamPtr clone() const;

        /**
         * \brief 获取数据在 cbuffer 中的偏移
         */
        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="getter")
        uint32_t getDataOffset() const { return mDataOffset; }
        
        /**
         * \brief 获取常量名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /**
         * @brief 获取常量缓冲区名称
         * @return 返回常量缓冲区名称
         */
        TPROPERTY(RTTRFuncName="CBufferName", RTTRFuncType="getter")
        const String &getCBufferName() const { return mCBufferName; }
        
        /**
         * \brief 获取数据类型
         */
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="getter")
        DATA_TYPE getDataType() const { return mDataType; }
        
        /**
         * \brief 获取数据大小
         */
        TPROPERTY(RTTRFuncName="DataSize", RTTRFuncType="getter")
        uint32_t getDataSize() const { return mDataSize; }

        /**
         * @brief 数据绑定到 constant buffer 的绑定点
         */
        TPROPERTY(RTTRFuncName="BindingPoint", RTTRFuncType="getter")
        uint32_t getBindingPoint() const { return mBindingPoint; }
    
    private:
        ShaderConstantParam() = default;
        
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="setter")
        void setDataType(DATA_TYPE type) { mDataType = type; }

        TPROPERTY(RTTRFuncName="DataSize", RTTRFuncType="setter")
        void setDataSize(uint32_t size) { mDataSize = size; }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="CBufferName", RTTRFuncType="setter")
        void setCBufferName(const String &name) { mCBufferName = name; }

        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="setter")
        void setDataOffset(uint32_t offset) { mDataOffset = offset; }
        
        TPROPERTY(RTTRFuncName="BindingPoint", RTTRFuncType="setter")
        void setBindingPoint(uint32_t bindingPoint) { mBindingPoint = bindingPoint; }
        
    protected:
        /**
         * @brief 构造函数
         * @param [in] cbufferName : 常量缓冲区名称
         * @param [in] name : 常量名称
         * @param [in] bindingPoint : 常量缓冲区绑定点
         * @param [in] dataSize : 数据大小
         * @param [in] dataOffset : 数据在常量缓冲区中的偏移 
         * @param [in] dataType : 数据类型
         */
        ShaderConstantParam(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType);
        
        /// 数据类型
        DATA_TYPE   mDataType {DATA_TYPE::DT_FLOAT};
        /// 数据大小
        uint32_t    mDataSize {0};
        /// 数据在 cbuffer 中的偏移
        uint32_t    mDataOffset {0};
        /// 数据所属的 cbuffer 的绑定点
        uint32_t    mBindingPoint {0};
        /// 常量名 
        String      mName {};
        /// 常量缓冲区名
        String      mCBufferName {};
    };

    TCLASS()
    class T3D_ENGINE_API ShaderConstantValue
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static ShaderConstantValuePtr create(const String &name, ShaderConstantParam::DATA_TYPE dataType, size_t dataSize);
        
        ~ShaderConstantValue() override;

        ShaderConstantValuePtr clone() const;
        
        /**
         * \brief 获取常量名称
         */
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /**
         * \brief 获取数据类型
         */
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="getter")
        ShaderConstantParam::DATA_TYPE getDataType() const { return mDataType; }

        void setBool(bool value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL);
            *(bool *)mValue.Data = value;
        }

        bool getBool() const { return *(bool *)mValue.Data; }

        void setBoolArray(const BoolArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(bool) * values.size());
            const auto &val = values[0];
            memcpy(mValue.Data, &val, sizeof(bool) * values.size());
        }

        BoolArray getBoolArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY);
            T3D_ASSERT(mValue.DataSize >= sizeof(bool));
            return BoolArray((bool *)mValue.Data, (bool *)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        void setFloat(float32_t value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT);
            *(float32_t *)mValue.Data = value;
        }

        float32_t getFloat() const { return *(float32_t *)mValue.Data; }

        void setFloatArray(const FloatArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(float32_t) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(float32_t) * values.size());
        }

        FloatArray getFloatArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(float32_t));
            return FloatArray((float32_t*)mValue.Data, (float32_t*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        void setInteger(int32_t value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER);
            *(int32_t *)mValue.Data = value;
        }

        int32_t getInteger() const { return *(int32_t *)mValue.Data; }

        void setIntArray(const IntArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(int32_t) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(int32_t) * values.size());
        }

        IntArray getIntArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(int32_t));
            return IntArray((int32_t*)mValue.Data, (int32_t*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        void setColor(const ColorRGBA &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR || mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4);
            *(ColorRGBA *)mValue.Data = value;
        }

        const ColorRGBA &getColor() const { return *(ColorRGBA*)mValue.Data; }

        void setColorArray(const ColorArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(ColorRGBA) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(ColorRGBA) * values.size());
        }

        ColorArray getColorArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(ColorRGBA));
            return ColorArray((ColorRGBA*)mValue.Data, (ColorRGBA *)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        void setVector(const Vector4 &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4);
            *(Vector4 *)mValue.Data = value;
        }

        const Vector4 &getVector() const { return *(Vector4 *)mValue.Data; }

        void setVectorArray(const Vector4Array &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(Vector4) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(Vector4) * values.size());
        }

        Vector4Array getVectorArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(Vector4));
            return Vector4Array((Vector4*)mValue.Data, (Vector4*)((uint8_t*)mValue.Data + mValue.DataSize));
        }
        
        void setMatrix(const Matrix4 &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
            *(Matrix4 *)mValue.Data = value;
        }

        const Matrix4 &getMatrix() const { return *(Matrix4 *)mValue.Data; }

        void setMatrixArray(const Matrix4Array &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
            T3D_ASSERT(mValue.DataSize == sizeof(Matrix4) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(Matrix4) * values.size());
        }

        Matrix4Array getMatrixArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
            T3D_ASSERT(mValue.DataSize > sizeof(Matrix4));
            return Matrix4Array((Matrix4*)mValue.Data, (Matrix4*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        void setData(const void *data, uint32_t dataSize) { mValue.setData(data, dataSize); }

        const void *getData() const { return mValue.Data; }

        uint32_t getDataSize() const { return (uint32_t)mValue.DataSize; }
        
    private:
        ShaderConstantValue() = default;

        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="setter")
        void setDataType(ShaderConstantParam::DATA_TYPE type) { mDataType = type; }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="Value", RTTRFuncType="getter")
        const Buffer &getValue() const { return mValue; }

        TPROPERTY(RTTRFuncName="Value", RTTRFuncType="setter")
        void setValue(const Buffer &value) { mValue = value; }
        
    protected:
        ShaderConstantValue(const String &name, ShaderConstantParam::DATA_TYPE type, size_t dataSize);
        
        /// 名字
        String  mName {};
        /// 数据类型
        ShaderConstantParam::DATA_TYPE mDataType {ShaderConstantParam::DATA_TYPE::DT_FLOAT};
        /// 数值
        Buffer mValue {};
    };
}


#endif    /*__T3D_SHADER_CONSTANT_PARAM_H__*/
