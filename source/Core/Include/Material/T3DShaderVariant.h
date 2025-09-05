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

        TResult compile();

        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="getter")
        SHADER_STAGE getShaderStage() const { return mShaderStage; }

        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="setter")
        void setShaderStage(SHADER_STAGE stage) { mShaderStage = stage; }
        
        /**
         * \brief 获取变体对应的关键字
         * \return 返回变体对应的关键字对象
         */
        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getShaderKeyword() const { return *mShaderKeyword; }
        
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

        /**
         * 通过反射获取 shader 信息
         * @return 调用成功返回 T3D_OK
         */
        TResult reflect();

        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="getter")
        const ShaderConstantParams &getShaderConstantParams() const { return mConstantParams; }

        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="getter")
        const ShaderSamplerParams &getShaderSamplerParams() const { return mSamplerParams; }

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
        String getSourceCode() const
        {
            return String(mBytesCode, mBytesCodeSize);
        }

        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="setter")
        void setSourceCode(String code)
        {
            setSourceCode(&code[0], code.length());
        }

        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="setter")
        void setShaderConstantParams(const ShaderConstantParams &params) { mConstantParams = params; }

        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="setter")
        void setShaderSamplerParams(const ShaderSamplerParams &params) { mSamplerParams = params; }
        
    protected:
        ShaderVariant(ShaderKeyword &&key, const String &code);

        void copyCode(const char *code, size_t codeSize)
        {
            if (mBytesCodeCapacity < codeSize || mBytesCodeCapacity == 0)
            {
                T3D_POD_SAFE_DELETE_ARRAY(mBytesCode);
                mBytesCode = T3D_POD_NEW_ARRAY(char, codeSize);
                mBytesCodeCapacity = codeSize;
            }
            memcpy(mBytesCode, code, codeSize);
            mBytesCodeSize = codeSize;
        }

    protected:
        /// 所属 pass
        Pass *mPass {nullptr};
        /// shader keyword
        ShaderKeyword   *mShaderKeyword {nullptr};

        /// shader 类型
        SHADER_STAGE    mShaderStage {SHADER_STAGE::kUnknown};
        /// 编译前是源代码 ，编译后是字节码 
        char            *mBytesCode {nullptr};
        /// mBytesCode 的长度
        size_t          mBytesCodeSize {0};
        /// mBytesCode 的空间容量
        size_t          mBytesCodeCapacity {0};
        /// 是否编译
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
