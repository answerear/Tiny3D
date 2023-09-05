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
            DT_INT,
            /// 结构体
            DT_STRUCT,
            DT_MAX
        };

        /**
         * \brief 创建对象函数
         * \param [in] name : 变量名称
         * \param [in] data : 数据地址
         * \param [in] dataSize : 数据大小
         * \param [in] registerIdx : 数据地址索引，不同 Render API 不一样定义 
         * \param registerNum : 占用寄存器数量
         * \param dataType : 数据类型
         */
        static ShaderConstantParamPtr create(const String &name, const void* data, uint32_t dataSize, uint32_t registerIdx, uint32_t registerNum, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);

        /**
         * \brief Destructor
         */
        ~ShaderConstantParam() override;

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
         * \brief 获取占用寄存器数量
         */
        TPROPERTY(RTTRFuncName="RegisterNum", RTTRFuncType="getter")
        uint32_t getRegisterNum() const { return mRegisterNum; }

        /**
         * \brief 获取寄存器数据地址索引
         */
        TPROPERTY(RTTRFuncName="RegisterIndex", RTTRFuncType="getter")
        uint32_t getRegisterIndex() const { return mRegisterIndex; }

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

        TPROPERTY(RTTRFuncName="RegisterIndex", RTTRFuncType="setter")
        void setRegisterIndex(uint32_t registerIdx)
        {
            mRegisterIndex = registerIdx;
        }

        TPROPERTY(RTTRFuncName="RegisterNum", RTTRFuncType="setter")
        void setRegisterNum(uint32_t registerNum)
        {
            mRegisterNum = registerNum;
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
         * \param [in] registerIdx : 数据地址索引，不同 Render API 不一样定义 
         * \param registerNum : 占用寄存器数量
         * \param dataType : 数据类型
         */
        ShaderConstantParam(const String &name, const void* data, uint32_t dataSize, uint32_t registerIdx, uint32_t registerNum, DATA_TYPE dataType = DATA_TYPE::DT_FLOAT);
        
        // /// 数据地址
        // uint8_t     *mData = nullptr;
        // /// 数据大小
        // uint32_t    mDataSize = 0;

        /// 数据
        Buffer      mData;
        /// 数据类型
        DATA_TYPE   mDataType = DATA_TYPE::DT_FLOAT;
        /// 数据地址索引
        uint32_t    mRegisterIndex = 0;
        /// 占用寄存器数量
        uint32_t    mRegisterNum = 0;
        /// 变量名 
        String      mName {};
    };

    /**
     * \brief 着色器里的纹理采样器
     */
    TCLASS()
    class T3D_ENGINE_API ShaderSamplerParam
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief Constructor
         * \param [in] name : 
         * \param [in] texType : 
         * \param [in] registerIdx :
         * \param [in] registerNum :
         */
        ShaderSamplerParam(const String &name, uint32_t texType, uint32_t registerIdx, uint32_t registerNum);

        /**
         * \brief Destructor
         */
        ~ShaderSamplerParam() override;

    private:
        ShaderSamplerParam() = default;
    };

    using Keys = TSet<String>;
    
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
    TCLASS(RTTRConstructAsObject)
    class T3D_ENGINE_API ShaderKeyword
    {
        TRTTI_ENABLE()
        TRTTI_FRIEND
        
    public:
        /**
         * \brief Constructor
         */
        ShaderKeyword();

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
        virtual ~ShaderKeyword() = default;

        /**
         * \brief 是否有对应的宏
         * \param [in] keyword : 宏定义名称
         * \return 如果有对应的宏定义，则返回 true
         */
        bool hasKey(const String &keyword) const;

        TPROPERTY(RTTRFuncName = "HashCode", RTTRFuncType = "getter")
        uint32_t getHashCode() const { return mHashCode; }

        TPROPERTY(RTTRFuncName = "Keys", RTTRFuncType = "getter")
        const Keys &getKeys() const { return mKeys; }

        /**
         * \brief 设置宏
         * \param [in] keyword : 宏定义名称
         * \return 调用成功返回 T3D_OK
         */
        TResult addKeyword(const String &keyword);

        TResult removeKeyword(const String &keyword);

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

    private:
        TPROPERTY(RTTRFuncName="HashCode", RTTRFuncType="setter")
        void setHashCode(uint32_t hash)
        {
            mHashCode = hash;
        }

        TPROPERTY(RTTRFuncName="Keys", RTTRFuncType="setter")
        void setKeys(const Keys &keys)
        {
            mKeys = keys;
        }
        
    protected:
        void copy(const ShaderKeyword &other);
        
        void move(ShaderKeyword &&other);
        
        uint32_t    mHashCode = 0;
        Keys        mKeys {};
    };

    using ShaderConstantParams = TArray<ShaderConstantParamPtr>;
    using ShaderSamplerParams = TArray<ShaderSamplerParamPtr>;
    
    /**
     * \brief Shader 变体，也是实际 shader 代码和编译后的字节码
     */
    TCLASS()
    class T3D_ENGINE_API ShaderVariant
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
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
        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getShaderKeyword() const { return *mShaderKeyword; }

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
            copyCode(code, codeLength);
            mHasCompiled = false;
        }

        void addConstant(ShaderConstantParamPtr constant)
        {
            mConstants.push_back(constant);
        }

        void addSampler(ShaderSamplerParamPtr sampler)
        {
            mSamplers.push_back(sampler);
        }

    private:
        ShaderVariant() = default;

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="getter")
        const ShaderConstantParams &getConstantParams() const
        {
            return mConstants;
        }

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="setter")
        void setConstantParams(const ShaderConstantParams &constants)
        {
            mConstants = constants;
        }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="getter")
        const ShaderSamplerParams &getSamplerParams() const
        {
            return mSamplers;
        }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="setter")
        void setSamplerParams(const ShaderSamplerParams &samplers)
        {
            mSamplers = samplers;
        }

        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="setter")
        void setShaderKeyword(const ShaderKeyword &keyword)
        {
            if (mShaderKeyword == nullptr)
            {
                mShaderKeyword = new ShaderKeyword(keyword);
            }
            else
            {
                *mShaderKeyword = keyword;
            }
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="getter")
        String getSourceCode() const
        {
            return String(mBytesCode, mBytesCodeSize);
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="setter")
        void setSourceCode(String code)
        {
            setSourceCode(&code[0], code.length());
        }
        
    protected:
        ShaderVariant(ShaderKeyword &&key, const String &code);

        void copyCode(const char *code, size_t codeSize)
        {
            if (mBytesCodeCapacity < codeSize || mBytesCodeCapacity == 0)
            {
                T3D_SAFE_DELETE_ARRAY(mBytesCode);
                mBytesCode = new char[codeSize];
                mBytesCodeCapacity = codeSize;
            }
            memcpy(mBytesCode, code, codeSize);
            mBytesCodeSize = codeSize;
        }

        /// shader 变量
        ShaderConstantParams  mConstants {};
        /// shader 采样器
        ShaderSamplerParams   mSamplers {};

        /// shader keyword
        ShaderKeyword   *mShaderKeyword = nullptr;
        
        /// 编译前是源代码 ，编译后是字节码 
        char        *mBytesCode = nullptr;
        /// mBytesCode 的长度
        size_t      mBytesCodeSize = 0;
        /// mBytesCode 的空间容量
        size_t      mBytesCodeCapacity = 0;
        /// 是否编译
        bool        mHasCompiled = false;
    };

    using ShaderKeywords = TList<ShaderKeyword>;
    using ShaderVariants = TMap<ShaderKeyword, ShaderVariantPtr>;
    using ShaderVariantsValue = ShaderVariants::value_type;
    
    /**
     * \brief 着色器资源
     */
    TCLASS()
    class T3D_ENGINE_API Shader : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND

    public:
        static ShaderPtr create(const String &name);

        ~Shader() override;

        Type getType() const override;

        TResult addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant);

        TResult enableKeyword(const String &keyword);

        TResult disableKeyword(const String &keyword);
        
        TPROPERTY(RTTRFuncName = "Keywords", RTTRFuncType = "getter")
        const ShaderKeywords &getKeywords() const
        {
            return mKeywords;
        }

        TPROPERTY(RTTRFuncName = "Variants", RTTRFuncType = "getter")
        const ShaderVariants &getShaderVariants() const
        {
            return mVariants;
        }

        const ShaderKeyword &getCurrentKeyword() const
        {
            return *mCurrentKeyword;
        }

        ShaderVariantPtr getCurrentVariant() const
        {
            return mCurrentVariant;
        }

    private:
        Shader() = default;

        TPROPERTY(RTTRFuncName = "Keywords", RTTRFuncType = "setter")
        void setKeywords(const ShaderKeywords &keywords)
        {
            mKeywords = keywords;
        }

        TPROPERTY(RTTRFuncName = "Variants", RTTRFuncType = "setter")
        void setShaderVariants(const ShaderVariants &variants)
        {
            mVariants = variants;
        }

    protected:
        Shader(const String &name);
        
        ResourcePtr clone() const override;

        /// 本着色器用到的所有关键字
        ShaderKeywords  mKeywords {};
        
        /// 本着色器的所有变体
        ShaderVariants  mVariants {};

        /// 当前生效的关键字 
        const ShaderKeyword *mCurrentKeyword = nullptr;
        
        /// 当前生效的变体
        ShaderVariantPtr    mCurrentVariant = nullptr;

        /// 关键字是否需要重新生成
        bool            mIsKeywordDirty = false;
    };
}


#endif    /*__T3D_SHADER_H__*/
