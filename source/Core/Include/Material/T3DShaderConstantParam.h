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
     * \brief 着色器常量参数的反射描述，记录 cbuffer 布局中的名称、偏移、类型与绑定点
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
         * \brief 常量参数的数据类型枚举
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
            /// 枚举上界，所有枚举值须小于该值
            DT_MAX
        };

        /**
         * \brief 创建着色器常量参数对象
         * \param [in] cbufferName : 所属常量缓冲区名称
         * \param [in] name : 变量名称
         * \param [in] bindingPoint : 所属 cbuffer 的绑定点
         * \param [in] dataSize : 变量占用字节数
         * \param [in] dataOffset : 变量在 cbuffer 内的偏移
         * \param [in] dataType : 数据类型
         * \return 新建的 ShaderConstantParam 智能指针
         */
        static ShaderConstantParamPtr create(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType);

        /// 析构函数
        ~ShaderConstantParam() override;

        /**
         * \brief 复制全部布局字段创建新对象
         * \return 新建的 ShaderConstantParam 副本
         */
        ShaderConstantParamPtr clone() const;

        /// 获取变量在 cbuffer 内的字节偏移
        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="getter")
        uint32_t getDataOffset() const { return mDataOffset; }
        
        /// 获取变量名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取所属常量缓冲区名称
        TPROPERTY(RTTRFuncName="CBufferName", RTTRFuncType="getter")
        const String &getCBufferName() const { return mCBufferName; }
        
        /// 获取数据类型
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="getter")
        DATA_TYPE getDataType() const { return mDataType; }
        
        /// 获取变量占用字节数
        TPROPERTY(RTTRFuncName="DataSize", RTTRFuncType="getter")
        uint32_t getDataSize() const { return mDataSize; }

        /// 获取所属 cbuffer 的绑定点
        TPROPERTY(RTTRFuncName="BindingPoint", RTTRFuncType="getter")
        uint32_t getBindingPoint() const { return mBindingPoint; }
    
    private:
        /// 默认构造，供 RTTR 反序列化使用
        ShaderConstantParam() = default;
        
        /// 设置数据类型（RTTR setter）
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="setter")
        void setDataType(DATA_TYPE type) { mDataType = type; }

        /// 设置数据大小（RTTR setter）
        TPROPERTY(RTTRFuncName="DataSize", RTTRFuncType="setter")
        void setDataSize(uint32_t size) { mDataSize = size; }

        /// 设置变量名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /// 设置 cbuffer 名称（RTTR setter）
        TPROPERTY(RTTRFuncName="CBufferName", RTTRFuncType="setter")
        void setCBufferName(const String &name) { mCBufferName = name; }

        /// 设置数据偏移（RTTR setter）
        TPROPERTY(RTTRFuncName="DataOffset", RTTRFuncType="setter")
        void setDataOffset(uint32_t offset) { mDataOffset = offset; }
        
        /// 设置绑定点（RTTR setter）
        TPROPERTY(RTTRFuncName="BindingPoint", RTTRFuncType="setter")
        void setBindingPoint(uint32_t bindingPoint) { mBindingPoint = bindingPoint; }
        
    protected:
        /**
         * \brief 构造着色器常量参数并初始化全部布局字段
         * \param [in] cbufferName : 所属常量缓冲区名称
         * \param [in] name : 变量名称
         * \param [in] bindingPoint : 所属 cbuffer 的绑定点
         * \param [in] dataSize : 变量占用字节数
         * \param [in] dataOffset : 变量在 cbuffer 内的偏移
         * \param [in] dataType : 数据类型
         */
        ShaderConstantParam(const String &cbufferName, const String &name, uint32_t bindingPoint, uint32_t dataSize, uint32_t dataOffset, DATA_TYPE dataType);
        
        /// 数据类型
        DATA_TYPE   mDataType {DATA_TYPE::DT_FLOAT};
        /// 变量占用字节数
        uint32_t    mDataSize {0};
        /// 变量在 cbuffer 内的字节偏移
        uint32_t    mDataOffset {0};
        /// 所属 cbuffer 的绑定点
        uint32_t    mBindingPoint {0};
        /// 变量名称
        String      mName {};
        /// 所属常量缓冲区名称
        String      mCBufferName {};
    };

    /**
     * \brief 着色器常量的运行时值容器，按 DATA_TYPE 分配并读写底层 Buffer
     */
    TCLASS()
    class T3D_ENGINE_API ShaderConstantValue
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建指定类型与大小的常量值对象，并分配底层存储
         * \param [in] name : 常量名称
         * \param [in] dataType : 数据类型
         * \param [in] dataSize : 底层 Buffer 字节数
         * \return 新建的 ShaderConstantValue 智能指针
         */
        static ShaderConstantValuePtr create(const String &name, ShaderConstantParam::DATA_TYPE dataType, size_t dataSize);
        
        /// 析构函数，释放底层 Buffer
        ~ShaderConstantValue() override;

        /**
         * \brief 复制名称、类型与底层数据创建新对象
         * \return 新建的 ShaderConstantValue 副本
         */
        ShaderConstantValuePtr clone() const;
        
        /// 获取常量名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取数据类型
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="getter")
        ShaderConstantParam::DATA_TYPE getDataType() const { return mDataType; }

        /**
         * \brief 写入 bool 值
         * \param [in] value : 目标值
         * \note 断言 mDataType 为 DT_BOOL
         */
        void setBool(bool value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL);
            *(bool *)mValue.Data = value;
        }

        /// 读取 bool 值
        bool getBool() const { return *(bool *)mValue.Data; }

        /**
         * \brief 写入 bool 数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_BOOL_ARRAY 且数组字节数与 Buffer 大小一致
         */
        void setBoolArray(const BoolArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(bool) * values.size());
            std::copy(values.begin(), values.end(), reinterpret_cast<bool*>(mValue.Data));
        }

        /// 读取 bool 数组视图
        BoolArray getBoolArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY);
            T3D_ASSERT(mValue.DataSize >= sizeof(bool));
            return BoolArray((bool *)mValue.Data, (bool *)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        /**
         * \brief 写入 float 值
         * \param [in] value : 目标值
         * \note 断言 mDataType 为 DT_FLOAT
         */
        void setFloat(float32_t value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT);
            *(float32_t *)mValue.Data = value;
        }

        /// 读取 float 值
        float32_t getFloat() const { return *(float32_t *)mValue.Data; }

        /**
         * \brief 写入 float 数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_FLOAT_ARRAY 且数组字节数与 Buffer 大小一致
         */
        void setFloatArray(const FloatArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(float32_t) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(float32_t) * values.size());
        }

        /// 读取 float 数组视图
        FloatArray getFloatArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(float32_t));
            return FloatArray((float32_t*)mValue.Data, (float32_t*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        /**
         * \brief 写入 int 值
         * \param [in] value : 目标值
         * \note 断言 mDataType 为 DT_INTEGER
         */
        void setInteger(int32_t value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER);
            *(int32_t *)mValue.Data = value;
        }

        /// 读取 int 值
        int32_t getInteger() const { return *(int32_t *)mValue.Data; }

        /**
         * \brief 写入 int 数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_INTEGER_ARRAY 且数组字节数与 Buffer 大小一致
         */
        void setIntArray(const IntArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(int32_t) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(int32_t) * values.size());
        }

        /// 读取 int 数组视图
        IntArray getIntArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(int32_t));
            return IntArray((int32_t*)mValue.Data, (int32_t*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        /**
         * \brief 写入颜色值
         * \param [in] value : 目标颜色
         * \note 断言 mDataType 为 DT_COLOR 或 DT_VECTOR4
         */
        void setColor(const ColorRGBA &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR || mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4);
            *(ColorRGBA *)mValue.Data = value;
        }

        /// 读取颜色值
        const ColorRGBA &getColor() const { return *(ColorRGBA*)mValue.Data; }

        /**
         * \brief 写入颜色数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_COLOR_ARRAY 或 DT_VECTOR4_ARRAY 且字节数一致
         */
        void setColorArray(const ColorArray &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY || mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(ColorRGBA) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(ColorRGBA) * values.size());
        }

        /// 读取颜色数组视图
        ColorArray getColorArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY || mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(ColorRGBA));
            return ColorArray((ColorRGBA*)mValue.Data, (ColorRGBA *)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        /**
         * \brief 写入 Vector4 值
         * \param [in] value : 目标向量
         * \note 断言 mDataType 为 DT_VECTOR4
         */
        void setVector(const Vector4 &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4);
            *(Vector4 *)mValue.Data = value;
        }

        /// 读取 Vector4 值
        const Vector4 &getVector() const { return *(Vector4 *)mValue.Data; }

        /**
         * \brief 写入 Vector4 数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_VECTOR4_ARRAY 且字节数一致
         */
        void setVectorArray(const Vector4Array &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(Vector4) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(Vector4) * values.size());
        }

        /// 读取 Vector4 数组视图
        Vector4Array getVectorArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(Vector4));
            return Vector4Array((Vector4*)mValue.Data, (Vector4*)((uint8_t*)mValue.Data + mValue.DataSize));
        }
        
        /**
         * \brief 写入 Matrix4 值
         * \param [in] value : 目标矩阵
         * \note 断言 mDataType 为 DT_MATRIX4
         */
        void setMatrix(const Matrix4 &value)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4);
            *(Matrix4 *)mValue.Data = value;
        }

        /// 读取 Matrix4 值
        const Matrix4 &getMatrix() const { return *(Matrix4 *)mValue.Data; }

        /**
         * \brief 写入 Matrix4 数组
         * \param [in] values : 目标数组
         * \note 断言类型为 DT_MATRIX4_ARRAY 且字节数一致
         */
        void setMatrixArray(const Matrix4Array &values)
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY);
            T3D_ASSERT(mValue.DataSize == sizeof(Matrix4) * values.size());
            memcpy(mValue.Data, values.data(), sizeof(Matrix4) * values.size());
        }

        /// 读取 Matrix4 数组视图
        Matrix4Array getMatrixArray() const
        {
            T3D_ASSERT(mDataType == ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY);
            T3D_ASSERT(mValue.DataSize > sizeof(Matrix4));
            return Matrix4Array((Matrix4*)mValue.Data, (Matrix4*)((uint8_t*)mValue.Data + mValue.DataSize));
        }

        /**
         * \brief 写入原始数据到底层 Buffer
         * \param [in] data : 源数据指针
         * \param [in] dataSize : 数据字节数
         */
        void setData(const void *data, uint32_t dataSize) { mValue.setData(data, dataSize); }

        /// 获取底层 Buffer 数据指针
        const void *getData() const { return mValue.Data; }

        /// 获取底层 Buffer 字节数
        uint32_t getDataSize() const { return (uint32_t)mValue.DataSize; }
        
    private:
        /// 默认构造，供 RTTR 反序列化使用
        ShaderConstantValue() = default;

        /// 设置数据类型（RTTR setter）
        TPROPERTY(RTTRFuncName="DataType", RTTRFuncType="setter")
        void setDataType(ShaderConstantParam::DATA_TYPE type) { mDataType = type; }

        /// 设置常量名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /// 获取底层 Buffer（RTTR getter）
        TPROPERTY(RTTRFuncName="Value", RTTRFuncType="getter")
        const Buffer &getValue() const { return mValue; }

        /// 设置底层 Buffer（RTTR setter）
        TPROPERTY(RTTRFuncName="Value", RTTRFuncType="setter")
        void setValue(const Buffer &value) { mValue = value; }
        
    protected:
        /**
         * \brief 构造常量值对象并分配指定大小的底层 Buffer
         * \param [in] name : 常量名称
         * \param [in] type : 数据类型
         * \param [in] dataSize : 底层 Buffer 字节数
         */
        ShaderConstantValue(const String &name, ShaderConstantParam::DATA_TYPE type, size_t dataSize);
        
        /// 常量名称
        String  mName {};
        /// 数据类型
        ShaderConstantParam::DATA_TYPE mDataType {ShaderConstantParam::DATA_TYPE::DT_FLOAT};
        /// 底层数值 Buffer
        Buffer mValue {};
    };
}


#endif    /*__T3D_SHADER_CONSTANT_PARAM_H__*/
