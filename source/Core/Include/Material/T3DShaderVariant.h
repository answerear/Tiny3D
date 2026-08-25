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
#include "Material/T3DShaderResourceParam.h"


namespace Tiny3D
{
    /**
     * \brief Shader 管线阶段枚举
     */
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
     * \brief Shader 目标语言（图形 API 维度）
     * \remarks 与渲染后端一一对应，由 RHIRenderer::getShadingLanguage() 推导
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
     * \brief Shader 变体，持有源码、编译字节码、反射参数与 RHI 着色器对象
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
         * \brief 从字符串源码创建 shader 变体
         * \param [in] keyword : 变体对应的关键字（移动语义）
         * \param [in] code : 源码字符串
         * \return 新建的 ShaderVariant 智能指针
         */
        static ShaderVariantPtr create(ShaderKeyword &&keyword, const String &code);

        /**
         * \brief 从原始数据创建 shader 变体（支持文本或 SPIR-V 等二进制）
         * \param [in] keyword : 变体对应的关键字（移动语义）
         * \param [in] code : 代码数据指针
         * \param [in] codeLength : 数据字节数
         * \return 新建的 ShaderVariant 智能指针
         */
        static ShaderVariantPtr create(ShaderKeyword &&keyword, const char *code, size_t codeLength);

        /// 析构函数，释放关键字、源码与字节码内存
        ~ShaderVariant() override;

        /**
         * \brief 通过 active RHIContext 编译 shader 并创建 RHIShader；已编译时跳过
         * \return 调用成功返回 T3D_OK；编译或 sampler 反射失败时返回对应错误码
         */
        TResult compile();

        /// 获取 shader 管线阶段
        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="getter")
        SHADER_STAGE getShaderStage() const { return mShaderStage; }

        /// 设置 shader 管线阶段
        TPROPERTY(RTTRFuncName="Stage", RTTRFuncType="setter")
        void setShaderStage(SHADER_STAGE stage) { mShaderStage = stage; }

        /// 获取目标着色语言
        TPROPERTY(RTTRFuncName="Language", RTTRFuncType="getter")
        SHADER_LANGUAGE getLanguage() const { return mLanguage; }

        /// 设置目标着色语言
        TPROPERTY(RTTRFuncName="Language", RTTRFuncType="setter")
        void setLanguage(SHADER_LANGUAGE lang) { mLanguage = lang; }
        
        /// 获取变体对应的 shader 关键字
        TPROPERTY(RTTRFuncName="ShaderKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getShaderKeyword() const { return *mShaderKeyword; }
        
        /// 是否已完成编译（RHIShader 就绪）
        bool hasCompiled() const { return mHasCompiled; }

        /**
         * \brief 获取当前可用代码：未编译时返回源码，已编译时返回字节码
         * \param [out] bytesLength : 输出数据字节数
         * \return 代码数据指针；供 RHIContext 透明使用
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
         * \brief 写入编译产物字节码并标记为已编译
         * \param [in] bytes : 字节码数据
         * \param [in] bytesLength : 字节码字节数
         * \note 不覆盖 mSourceCode，便于切后端或设备重置后重编译
         */
        void setBytesCode(const char *bytes, size_t bytesLength)
        {
            copyByteCode(bytes, bytesLength);
            mHasCompiled = true;
        }

        /**
         * \brief 写入语言源码并重置编译状态
         * \param [in] code : 源码数据
         * \param [in] codeLength : 源码字节数
         */
        void setSourceCode(const char *code, size_t codeLength)
        {
            copySourceCode(code, codeLength);
            mHasCompiled = false;
        }

        /**
         * \brief 释放 RHIShader 与字节码，保留源码，等待按新后端重编译
         */
        void invalidateRHI();

        /**
         * \brief 反射 shader 的全部常量与采样器绑定信息
         * \return 调用成功返回 T3D_OK；编译或反射失败时返回对应错误码
         * \note 若反射前未编译，会临时编译并在反射后恢复未编译状态
         */
        TResult reflect();

        /// 获取反射得到的常量参数列表
        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="getter")
        const ShaderConstantParams &getShaderConstantParams() const { return mConstantParams; }

        /// 获取反射得到的采样器参数列表
        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="getter")
        const ShaderSamplerParams &getShaderSamplerParams() const { return mSamplerParams; }

        /// 获取反射得到的缓冲型资源（结构化缓冲 / UAV）参数列表
        TPROPERTY(RTTRFuncName="ShaderResourceParams", RTTRFuncType="getter")
        const ShaderResourceParams &getShaderResourceParams() const { return mResourceParams; }

        /**
         * \brief 设置缓冲型资源参数列表
         * \param [in] params : 反射产出的资源绑定表
         * \remarks 由后端在 reflectShaderAllBindings() 中回填
         */
        void setShaderResourceParams(const ShaderResourceParams &params) { mResourceParams = params; }

        /**
         * \brief 获取 compute shader 的线程组尺寸
         * \param [out] x : 线程组 X 维尺寸
         * \param [out] y : 线程组 Y 维尺寸
         * \param [out] z : 线程组 Z 维尺寸
         * \remarks 非 compute 阶段或未反射时三者均为 0；上层据此把线程数换算成组数
         */
        void getThreadGroupSize(uint32_t &x, uint32_t &y, uint32_t &z) const
        {
            x = mThreadGroupSize[0];
            y = mThreadGroupSize[1];
            z = mThreadGroupSize[2];
        }

        /**
         * \brief 设置 compute shader 的线程组尺寸
         * \param [in] x : 线程组 X 维尺寸
         * \param [in] y : 线程组 Y 维尺寸
         * \param [in] z : 线程组 Z 维尺寸
         * \remarks 由后端在 reflectShaderAllBindings() 中回填
         */
        void setThreadGroupSize(uint32_t x, uint32_t y, uint32_t z)
        {
            mThreadGroupSize[0] = x;
            mThreadGroupSize[1] = y;
            mThreadGroupSize[2] = z;
        }

        /**
         * \brief 从同 keyword 的另一语言变体复制反射参数
         * \param [in] other : 源变体
         * \remarks 离线编译时仅 HLSL 变体能被反射，其它语言变体共用同一逻辑布局；
         *          运行时各后端在 compile() 中自行重新反射 sampler 绑定点
         */
        void copyReflectionParamsFrom(const ShaderVariant &other)
        {
            mConstantParams = other.mConstantParams;
            mSamplerParams = other.mSamplerParams;
            mResourceParams = other.mResourceParams;
            mThreadGroupSize[0] = other.mThreadGroupSize[0];
            mThreadGroupSize[1] = other.mThreadGroupSize[1];
            mThreadGroupSize[2] = other.mThreadGroupSize[2];
        }

        /// 获取 RHI 着色器对象
        RHIShader *getRHIShader() const { return mRHIShader; }

        /// 设置所属 Pass
        void setPass(Pass *pass) { mPass = pass; }

        /// 获取所属 Pass
        Pass *getPass() const { return mPass; }
        
    private:
        /// 默认构造，供 RTTR 反序列化使用
        ShaderVariant() = default;

        /// 设置 shader 关键字（RTTR setter）
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

        /// 获取源码 Buffer 视图（RTTR getter）
        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="getter")
        Buffer getSourceCode() const
        {
            Buffer code;
            code.Data = (uint8_t *)mSourceCode;
            code.DataSize = mSourceCodeSize;
            return code;
        }

        /// 从 Buffer 设置源码（RTTR setter）
        TPROPERTY(RTTRFuncName="Code", RTTRFuncType="setter")
        void setSourceCode(Buffer code)
        {
            setSourceCode((const char *)code.Data, code.DataSize);
            code.release();
        }

        /// 设置常量参数列表（RTTR setter）
        TPROPERTY(RTTRFuncName="ShaderConstantParams", RTTRFuncType="setter")
        void setShaderConstantParams(const ShaderConstantParams &params) { mConstantParams = params; }

        /// 设置采样器参数列表（RTTR setter）
        TPROPERTY(RTTRFuncName="ShaderSamplerParams", RTTRFuncType="setter")
        void setShaderSamplerParams(const ShaderSamplerParams &params) { mSamplerParams = params; }

        /// 设置资源参数列表（RTTR setter）
        TPROPERTY(RTTRFuncName="ShaderResourceParams", RTTRFuncType="setter")
        void setShaderResourceParamsForRTTR(const ShaderResourceParams &params) { mResourceParams = params; }

        /// 获取线程组 X 维尺寸（RTTR getter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeX", RTTRFuncType="getter")
        uint32_t getThreadGroupSizeX() const { return mThreadGroupSize[0]; }

        /// 设置线程组 X 维尺寸（RTTR setter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeX", RTTRFuncType="setter")
        void setThreadGroupSizeX(uint32_t size) { mThreadGroupSize[0] = size; }

        /// 获取线程组 Y 维尺寸（RTTR getter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeY", RTTRFuncType="getter")
        uint32_t getThreadGroupSizeY() const { return mThreadGroupSize[1]; }

        /// 设置线程组 Y 维尺寸（RTTR setter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeY", RTTRFuncType="setter")
        void setThreadGroupSizeY(uint32_t size) { mThreadGroupSize[1] = size; }

        /// 获取线程组 Z 维尺寸（RTTR getter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeZ", RTTRFuncType="getter")
        uint32_t getThreadGroupSizeZ() const { return mThreadGroupSize[2]; }

        /// 设置线程组 Z 维尺寸（RTTR setter）
        TPROPERTY(RTTRFuncName="ThreadGroupSizeZ", RTTRFuncType="setter")
        void setThreadGroupSizeZ(uint32_t size) { mThreadGroupSize[2] = size; }
        
    protected:
        /**
         * \brief 从字符串源码构造变体
         * \param [in] key : shader 关键字（移动语义）
         * \param [in] code : 源码字符串
         */
        ShaderVariant(ShaderKeyword &&key, const String &code);

        /**
         * \brief 从原始数据构造变体
         * \param [in] key : shader 关键字（移动语义）
         * \param [in] code : 代码数据指针
         * \param [in] codeLength : 数据字节数
         */
        ShaderVariant(ShaderKeyword &&key, const char *code, size_t codeLength);

        /// 复制源码到内部缓冲，必要时扩容
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

        /// 复制字节码到内部缓冲，必要时扩容
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
        /// 所属 Pass
        Pass *mPass {nullptr};
        /// shader 关键字
        ShaderKeyword   *mShaderKeyword {nullptr};

        /// shader 管线阶段
        SHADER_STAGE    mShaderStage {SHADER_STAGE::kUnknown};
        /// 目标着色语言
        SHADER_LANGUAGE mLanguage {SHADER_LANGUAGE::kUnknown};

        /// 语言源码（反序列化得到，编译时不被覆盖）
        char            *mSourceCode {nullptr};
        /// 源码字节数
        size_t          mSourceCodeSize {0};
        /// 源码缓冲容量
        size_t          mSourceCodeCapacity {0};

        /// 编译后的字节码（后端相关，可释放重建）
        char            *mByteCode {nullptr};
        /// 字节码字节数
        size_t          mByteCodeSize {0};
        /// 字节码缓冲容量
        size_t          mByteCodeCapacity {0};

        /// 当前 active 后端是否已编译就绪
        bool            mHasCompiled {false};

        /// 反射得到的常量参数
        ShaderConstantParams    mConstantParams {};
        
        /// 反射得到的纹理/采样器参数
        ShaderSamplerParams     mSamplerParams {};

        /// 反射得到的缓冲型资源（结构化缓冲 / UAV）参数
        ShaderResourceParams    mResourceParams {};

        /// compute shader 的 numthreads 尺寸，非 compute 阶段为全 0
        uint32_t                mThreadGroupSize[3] {0, 0, 0};

        /// RHI 着色器对象
        RHIShaderPtr    mRHIShader {nullptr};
    };
}


#endif    /*__T3D_SHADER_VARIANT_H__*/
