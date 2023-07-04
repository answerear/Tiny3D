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

#ifndef __T3D_SHADER_H__
#define __T3D_SHADER_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    /**
     * \brief 着色器里的常量
     */
    class T3D_ENGINE_API ShaderConstantData
        : public Object
        , public Noncopyable
    {
    public:
        /**
         * \brief 数据类型
         */
        enum class DataType : uint32_t
        {
            /**< 浮点数 */
            DT_FLOAT = 0,
            /**< 布尔值 */
            DT_BOOL,
            /**< 整型 */
            DT_INT,
            /**< 结构体 */
            DT_STRUCT,
            DT_MAX
        };

        /**
         * \brief 构造函数
         * \param [in] name : 变量名称
         * \param [in] data : 数据地址
         * \param [in] dataSize : 数据大小
         * \param [in] registerIdx : 数据地址索引，不同 Render API 不一样定义 
         * \param registerNum : 占用寄存器数量
         * \param dataType : 数据类型
         */
        ShaderConstantData(const String &name, const void* data, uint32_t dataSize, uint32_t registerIdx, uint32_t registerNum, DataType dataType = DataType::DT_FLOAT);
        
        virtual ~ShaderConstantData();

        /**
         * \brief 设置数据
         * \param [in] data : 数据地址 
         * \return 调用成功返回 T3D_OK
         */
        TResult setData(void *data)
        {
            if (data == nullptr)
                return T3D_ERR_INVALID_PARAM;
            memcpy(mData, data, mDataSize);
            return T3D_OK;
        }

        /**
         * \brief 获取变量数据
         */
        void *getData() const { return mData; }

        /**
         * \brief 获取数据类型
         */
        DataType getDataType() const { return mDataType; }

        /**
         * \brief 获取数据大小
         */
        uint32_t getSize() const { return mDataSize; }

        /**
         * \brief 获取占用寄存器数量
         */
        uint32_t getRegisterNum() const { return mRegisterNum; }

        /**
         * \brief 获取寄存器数据地址索引
         */
        uint32_t getRegisterIndex() const { return mRegisterIndex; }

    protected:
        /**< 数据地址 */
        uint8_t     *mData = nullptr;
        /**< 数据大小 */
        uint32_t    mDataSize = 0;
        /**< 数据类型 */
        DataType    mDataType = DataType::DT_FLOAT;
        /**< 数据地址索引 */
        uint32_t    mRegisterIndex = 0;
        /**< 占用寄存器数量 */
        uint32_t    mRegisterNum = 0;
        /**< 变量名 */
        String      mName {};
    };

    /**
     * \brief 着色器里的纹理采样器
     */
    class T3D_ENGINE_API ShaderSampler
        : public Object
        , public Noncopyable
    {
    public:
        ShaderSampler(const String &name, uint32_t texType, uint32_t registerIdx, uint32_t registerNum);
        virtual ~ShaderSampler();

        
    };

    /**
     * \brief 着色器宏
     * \remarks
     *      Example :
     *      Shader 中代码有如下宏定义
     *          #multi_compile A1 B1 C1
     *          #multi_compile A2 B2
     *      则产生的 ShaderKey 有：
     *          A1_A2、A1_B2、B1_A2、B1_B2、C1_A2、C1_B2
     *      一共六个 ShaderKey 对象
     */
    class T3D_ENGINE_API ShaderKeyword
    {
    public:
        /**
         * \brief Default constructor
         */
        ShaderKeyword();
        
        /**
         * \brief Constructor with the number of keywords
         */
        ShaderKeyword(uint32_t count);

        /**
         * \brief Copy constructor
         */
        ShaderKeyword(const ShaderKeyword &other);

        /**
         * \brief Move constructor
         */
        ShaderKeyword(ShaderKeyword &&other) noexcept;

        /**
         * \brief Destructor
         */
        ~ShaderKeyword() = default;

        /**
         * \brief 是否有对应的宏
         * \param [in] keyword : 宏定义名称
         * \return 如果有对应的宏定义，则返回 true
         */
        bool hasKey(const String &keyword) const;

        /**
         * \brief 设置宏
         * \param [in] keyword : 宏定义名称
         * \return 调用成功返回 T3D_OK
         */
        TResult setKeyword(const String &keyword, uint32_t index);

        /**
         * \brief 当宏集合有变化时，要调用本接口，生成唯一 hash 值，方便后续快速比较
         * \return 调用成功返回 T3D_OK
         */
        TResult generate();

        bool operator ==(const ShaderKeyword &other) const;
        bool operator <(const ShaderKeyword &other) const;
        bool operator >(const ShaderKeyword &other) const;

        /**
         * \brief Copy assignment
         */
        ShaderKeyword &operator =(const ShaderKeyword &other);

        /**
         * \brief Move assignment
         */
        ShaderKeyword &operator =(ShaderKeyword &&other) noexcept;
        
    protected:
        void copy(const ShaderKeyword &other);
        
        void move(ShaderKeyword &&other);
        
        using Keys = TArray<String>;

        uint32_t    mHashCode = 0;
        Keys        mKeys {};
    };

    /**
     * \brief Shader 变体，也是实际 shader 代码和编译后的字节码
     */
    class T3D_ENGINE_API ShaderVariant
        : public Object
        , public Noncopyable
    {
    public:
        /**
         * \brief 创建 shader 变体对象
         * \param [in] keyword : 变体对应的关键字
         * \param [in] code : 变体对应的源码
         * \return 返回 shader 变体对象
         */
        static ShaderVariantPtr create(ShaderKeyword &&keyword, const String &code);

        /**
         * \brief Destructor
         */
        ~ShaderVariant() override;

        /**
         * \brief 获取变体对应的关键字
         * \return 返回变体对应的关键字对象
         */
        const ShaderKeyword &getShaderKeyword() const { return mShaderKeyword; }

        /**
         * \brief 设置 shader 变量值
         * \param [in] name : 变量名
         * \param [in] data : 变量对应的数据 
         * \return 调用成功返回 T3D_OK
         */
        TResult setParam(const String &name, void *data);

        /**
         * \brief 设置 shader 采样器
         * \param [in] name : 采样器名称
         * \param [in] index : 采样器索引
         * \return 调用成功返回 T3D_OK
         */
        TResult setParam(const String &name, int32_t index);
        
        bool hasCompiled() const { return mHasCompiled; }

        char *getBytesCode(size_t &bytesLength) const
        {
            bytesLength = mBytesCodeSize;
            return mBytesCode;
        }

        void setBytesCode(const char *bytes, size_t bytesLength)
        {
            copyCode(bytes, bytesLength);
            mHasCompiled = true;
        }

        void setSourceCode(const char *code, size_t codeLength)
        {
            copyCode(code, codeLength+1);
            mBytesCode[codeLength] = 0;
            mHasCompiled = false;
        }

        void addConstant(ShaderConstantDataPtr constant)
        {
            mConstants.push_back(constant);
        }

        void addSampler(ShaderSamplerPtr sampler)
        {
            mSamplers.push_back(sampler);
        }
        
    protected:
        ShaderVariant(ShaderKeyword &&key, const String &code);

        void copyCode(const char *code, size_t codeSize)
        {
            if (mBytesCodeSize < codeSize)
            {
                T3D_SAFE_DELETE_ARRAY(mBytesCode);
            }
            memcpy(mBytesCode, code, codeSize);
            mBytesCodeSize = codeSize;
        }

        /// shader 变量
        TArray<ShaderConstantDataPtr>   mConstants {};
        /// shader 采样器
        TArray<ShaderSamplerPtr>        mSamplers {};

        /// shader keyword
        ShaderKeyword   mShaderKeyword {};
        
        /// 编译前是源代码 ，编译后是字节码 
        char        *mBytesCode = nullptr;
        /// mBytesCode 的长度
        size_t      mBytesCodeSize = 0;
        /// 是否编译
        bool        mHasCompiled = false;
    };

    /**
     * \brief 记录着色器关键字 
     */
    TSTRUCT()
    struct T3D_ENGINE_API ShaderKeywords
    {
        TRTTI_ENABLE()

    public:
        virtual ~ShaderKeywords() = default;
        
        using Keywords = TList<TSet<String>>;

        TPROPERTY()
        Keywords  keywords {};
    };

    /**
     * \brief 着色器资源
     */
    class T3D_ENGINE_API Shader : public Resource
    {
    public:
        static ShaderPtr create(const String &name);

        ~Shader() override;

        Type getType() const override;

        TResult enableKeyword(const String &keyword);

        TResult disableKeyword(const String &keyword);
        
    protected:
        Shader(const String &name);
        
        TResult loadData(DataStream &stream) override;

        ResourcePtr clone() const override;
        
        using ShaderVariants = TMap<ShaderKeyword, ShaderVariantPtr>;

        /// 本着色器用到的所有关键字
        ShaderKeywords  mKeywords {};
        
        /// 本着色器的所有变体
        ShaderVariants  mVariants {};

        /// 当前生效的关键字 
        ShaderKeyword       mCurrentKeyword {};
        
        /// 当前生效的变体
        ShaderVariantPtr    mCurrentVariant = nullptr;

        /// 关键字是否需要重新生成
        bool            mIsKeywordDirty = false;
    };
}


#endif    /*__T3D_SHADER_H__*/
