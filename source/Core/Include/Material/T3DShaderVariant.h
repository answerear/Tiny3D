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

#ifndef __T3D_SHADER_VARIANT_H__
#define __T3D_SHADER_VARIANT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderBinding.h"


namespace Tiny3D
{
    TENUM()
    enum class SHADER_STAGE : uint32_t
    {
        kUnknown = 0,
        kVertex,
        kPixel,
        kCompute,
        kGeometry,
        kHull,
        kDomain,
        kMax
    };

    /**
     * \brief Shader 目标语言（图形 API 维度）。
     * \remarks 与渲染后端一一对应，由 RHIRenderer::getShadingLanguage() 推导。
     */
    TENUM()
    enum class SHADER_LANGUAGE : uint32_t
    {
        kUnknown = 0,
        kHLSL,      ///< Direct3D11/12
        kGLSL,      ///< OpenGL 4
        kESSL,      ///< OpenGL ES 3
        kSPIRV,     ///< Vulkan
        kMSL,       ///< Metal（预留）
        kMax
    };
    
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
         * \brief 创建 shader 变体对象（接受二进制/文本数据 + 长度）
         * \param [in] keyword : 变体对应的关键字
         * \param [in] code : 变体对应的代码数据（可以是文本或二进制如 SPIR-V）
         * \param [in] codeLength : 代码数据长度（字节数）
         * \return 返回 shader 变体对象
         */
        static ShaderVariantPtr create(ShaderKeyword &&keyword, const char *code, size_t codeLength);

        /**
         * \brief Destructor
         */
        ~ShaderVariant() override;

        TResult compile();

        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="getter")
        SHADER_STAGE getShaderStage() const { return mShaderStage; }

        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="setter")
        void setShaderStage(SHADER_STAGE stage) { mShaderStage = stage; }

        TPROPERTY(RTTRFuncName="Language", RTTRFuncType="getter")
        SHADER_LANGUAGE getLanguage() const { return mLanguage; }

        TPROPERTY(RTTRFuncName="Language", RTTRFuncType="setter")
        void setLanguage(SHADER_LANGUAGE lang) { mLanguage = lang; }
        
        /**
         * \brief 获取变体对应的关键字
         * \return 返回变体对应的关键字对象
         */
        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getShaderKeyword() const { return *mShaderKeyword; }
        
        bool hasCompiled() const { return mHasCompiled; }

        /**
         * \brief 获取当前可用代码：未编译时返回语言源码，已编译时返回编译产物字节码。
         * \note 该语义供 RHIContext 透明使用（编译前取源码、编译后取字节码），
         *       因此各渲染器插件无需改动。
         */
        char *getBytesCode(size_t &bytesLength) const
        {
            if (mHasCompiled)
            {
                bytesLength = mByteCodeSize;
                return mByteCode;
            }
            bytesLength = mSourceCodeSize;
            return mSourceCode;
        }

        /**
         * \brief 写入编译产物字节码。不破坏源码 mSourceCode，使切后端 / 设备重置可重编译。
         */
        void setBytesCode(const char *bytes, size_t bytesLength)
        {
            copyByteCode(bytes, bytesLength);
            mHasCompiled = true;
        }

        /**
         * \brief 写入语言源码（反序列化得到，只读、永不被编译覆盖），并重置编译状态。
         */
        void setSourceCode(const char *code, size_t codeLength)
        {
            copySourceCode(code, codeLength);
            mHasCompiled = false;
        }

        /**
         * \brief 失效后端相关资源：释放 RHIShader 与字节码，保留源码，等待按新后端重编译。
         */
        void invalidateRHI();

        /**
         * 通过反射获取 shader 信息
         * @return 调用成功返回 T3D_OK
         */
        TResult reflect();

        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="getter")
        const ShaderConstantParams &getShaderConstantParams() const { return mConstantParams; }

        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="getter")
        const ShaderSamplerParams &getShaderSamplerParams() const { return mSamplerParams; }

        /**
         * \brief 从同 keyword 的另一语言变体复制反射得到的常量/采样参数。
         * \remarks 离线编译时反射上下文（如 scc 的 D3D11）只能编译/反射 HLSL 变体，
         *          同一 keyword 下其它语言变体共用同一逻辑常量布局，直接复制其参数即可；
         *          运行时各后端会在 compile() 中自行重新反射 sampler 的实际绑定槽位。
         */
        void copyReflectionParamsFrom(const ShaderVariant &other)
        {
            mConstantParams = other.mConstantParams;
            mSamplerParams = other.mSamplerParams;
        }

        RHIShader *getRHIShader() const { return mRHIShader; }

        void setPass(Pass *pass) { mPass = pass; }

        Pass *getPass() const { return mPass; }
        
    private:
        ShaderVariant() = default;

        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="setter")
        void setShaderKeyword(const ShaderKeyword &keyword)
        {
            if (mShaderKeyword == nullptr)
            {
                mShaderKeyword = T3D_NEW ShaderKeyword(keyword);
            }
            else
            {
                *mShaderKeyword = keyword;
            }
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="getter")
        Buffer getSourceCode() const
        {
            Buffer code;
            code.Data = (uint8_t *)mSourceCode;
            code.DataSize = mSourceCodeSize;
            return code;
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="setter")
        void setSourceCode(Buffer code)
        {
            setSourceCode((const char *)code.Data, code.DataSize);
            code.release();
        }

        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="setter")
        void setShaderConstantParams(const ShaderConstantParams &params) { mConstantParams = params; }

        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="setter")
        void setShaderSamplerParams(const ShaderSamplerParams &params) { mSamplerParams = params; }
        
    protected:
        ShaderVariant(ShaderKeyword &&key, const String &code);
        ShaderVariant(ShaderKeyword &&key, const char *code, size_t codeLength);

        void copySourceCode(const char *code, size_t codeSize)
        {
            if (mSourceCodeCapacity < codeSize || mSourceCodeCapacity == 0)
            {
                T3D_POD_SAFE_DELETE_ARRAY(mSourceCode);
                mSourceCode = T3D_POD_NEW_ARRAY(char, codeSize);
                mSourceCodeCapacity = codeSize;
            }
            memcpy(mSourceCode, code, codeSize);
            mSourceCodeSize = codeSize;
        }

        void copyByteCode(const char *code, size_t codeSize)
        {
            if (mByteCodeCapacity < codeSize || mByteCodeCapacity == 0)
            {
                T3D_POD_SAFE_DELETE_ARRAY(mByteCode);
                mByteCode = T3D_POD_NEW_ARRAY(char, codeSize);
                mByteCodeCapacity = codeSize;
            }
            memcpy(mByteCode, code, codeSize);
            mByteCodeSize = codeSize;
        }

    protected:
        /// 所属 pass
        Pass *mPass {nullptr};
        /// shader keyword
        ShaderKeyword   *mShaderKeyword {nullptr};

        /// shader 类型
        SHADER_STAGE    mShaderStage {SHADER_STAGE::kUnknown};
        /// 目标语言（图形 API 维度）
        SHADER_LANGUAGE mLanguage {SHADER_LANGUAGE::kUnknown};

        /// 语言源码（反序列化得到，只读，编译时不被覆盖）
        char            *mSourceCode {nullptr};
        /// mSourceCode 的长度
        size_t          mSourceCodeSize {0};
        /// mSourceCode 的空间容量
        size_t          mSourceCodeCapacity {0};

        /// 编译后的字节码（后端相关，可释放、可重建）
        char            *mByteCode {nullptr};
        /// mByteCode 的长度
        size_t          mByteCodeSize {0};
        /// mByteCode 的空间容量
        size_t          mByteCodeCapacity {0};

        /// 当前 active 后端的 RHIShader 是否就绪
        bool            mHasCompiled {false};

        /// 常量信息
        ShaderConstantParams    mConstantParams {};
        
        /// 纹理和纹理采样绑定关系
        ShaderSamplerParams     mSamplerParams {};

        /// RHI 着色器对象
        RHIShaderPtr    mRHIShader {nullptr};
    };
}


#endif    /*__T3D_SHADER_VARIANT_H__*/
