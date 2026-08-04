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

#ifndef __T3D_SHADER_H__
#define __T3D_SHADER_H__


#include "Resource/T3DResource.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderSamplerParam.h"


namespace Tiny3D
{
    using Techniques = TList<TechniquePtr>;
    
    /**
     * \brief 着色器资源，管理 Technique 列表、常量初值与采样器参数
     */
    TCLASS()
    class T3D_ENGINE_API Shader : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 Shader 资源
         * \param [in] name : 着色器名称
         * \return 新 Shader 智能指针
         */
        static ShaderPtr create(const String &name);

        /// 析构 Shader
        ~Shader() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kShader
         */
        Type getType() const override;

        /**
         * \brief 编译所有 Technique；首个编译成功的 Technique 暂设为 mSupportTechnique
         * \return 全部成功返回 T3D_OK，任一失败则中断并返回错误码
         */
        TResult compile();

        /**
         * \brief 对所有 Technique 执行 reflect
         * \return 全部成功返回 T3D_OK，任一失败则中断并返回错误码
         */
        TResult reflect();
        
        /**
         * \brief 查询关键字是否启用
         * \param [in] keyword : 着色器关键字
         * \return 当前实现恒为 false
         */
        bool isKeywordEnable(const String &keyword) const;
        
        /**
         * \brief 按名称添加常量初值
         * \param [in] value : 常量值对象
         * \return 成功返回 T3D_OK；名称重复返回 T3D_ERR_DUPLICATED_ITEM
         */
        TResult addConstantValue(ShaderConstantValuePtr value);

        /**
         * \brief 按名称添加采样器参数
         * \param [in] param : 采样器参数对象
         * \return 成功返回 T3D_OK；名称重复返回 T3D_ERR_DUPLICATED_ITEM
         */
        TResult addSamplerParam(ShaderSamplerParamPtr param);

        /**
         * \brief 添加渲染 Technique（同名已存在则跳过）
         * \param [in] tech : 渲染 Technique；首个添加的自动设为 mSupportTechnique
         * \return 添加成功返回 true，同名已存在返回 false
         */
        bool addTechnique(TechniquePtr tech);

        /**
         * \brief 按名称移除 Technique
         * \param [in] name : Technique 名称
         */
        void removeTechnique(const String &name);

        /**
         * \brief 按名称查找 Technique
         * \param [in] name : Technique 名称
         * \param [out] tech : 输出找到的 Technique
         * \return 找到返回 true，否则 false
         */
        bool getTechnique(const String &name, TechniquePtr &tech) const;

        /// 获取当前硬件支持的 Technique（compile 后首个成功或 addTechnique 首个）
        TechniquePtr getSupportTechnique() const { return mSupportTechnique; }

        /// RTTR：获取采样器参数表
        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="getter")
        const ShaderSamplerParams &getSamplerParams() const { return mSamplers; }

        /// RTTR：获取 Technique 列表
        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="getter")
        const Techniques &getTechniques() const { return mTechniques; }

        /// RTTR：获取常量初值表
        TPROPERTY(RTTRFuncName="ConstantValues", RTTRFuncType="getter")
        const ShaderConstantValues &getConstantValues() const { return mConstantValues; }
        
    private:
        /// 默认构造
        Shader() = default;

        /// RTTR：设置采样器参数表
        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="setter")
        void setSamplerParams(const ShaderSamplerParams &params) { mSamplers = params; }

        /// RTTR：设置 Technique 列表
        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="setter")
        void setTechniques(const Techniques &techniques) { mTechniques = techniques; }

        /// RTTR：设置常量初值表
        TPROPERTY(RTTRFuncName="ConstantValues", RTTRFuncType="setter")
        void setConstantValues(const ShaderConstantValues &values) { mConstantValues = values; }

    protected:
        /**
         * \brief 构造着色器资源
         * \param [in] name : 着色器名称
         */
        Shader(const String &name);
        
        /**
         * \brief 克隆着色器（当前实现仅 create 同名新实例，不复制属性）
         * \return 新 Shader 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从 Archive 加载后自动 compile
         * \param [in] archive : 资源档案
         * \return 加载或编译失败返回对应错误码
         */
        TResult onLoad(Archive *archive) override;

        /**
         * \brief 加载后为各 Technique 设置所属 Shader 指针
         */
        void onPostLoad() override;
        
    protected:
        /// shader 常量初始值
        ShaderConstantValues    mConstantValues {};
        /// shader 采样器参数
        ShaderSamplerParams     mSamplers {};
        /// 所有 Technique
        Techniques              mTechniques {};
        /// 当前选中的支持 Technique
        TechniquePtr            mSupportTechnique {nullptr};
    };
}


#endif    /*__T3D_SHADER_H__*/
