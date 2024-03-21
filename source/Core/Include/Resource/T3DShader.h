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

        TResult compile();
        
        bool isKeywordEnable(const String &keyword) const;

        TResult addConstantParam(ShaderConstantParamPtr param);

        TResult addSamplerParam(ShaderSamplerParamPtr param);

        /**
         * \brief 添加一个渲染技术
         * \param [in] tech : 渲染技术
         * \return 成功返回 true
         */
        bool addTechnique(TechniquePtr tech);

        /**
         * \brief 删除一个渲染技术
         * \param [in] name : 渲染技术名称 
         */
        void removeTechnique(const String &name);

        /**
         * \brief 获取指定名称的渲染技术
         * \param [in] name : 要获取的渲染技术名称
         * \param [in] tech : 获取到的渲染技术对象 
         * \return 如果能获取到对象，返回 true
         */
        bool getTechnique(const String &name, TechniquePtr &tech) const;

        TechniquePtr getSupportTechnique() const { return mSupportTechnique; }

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="getter")
        const ShaderConstantParams &getConstantParams() const
        {
            return mConstants;
        }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="getter")
        const ShaderSamplerParams &getSamplerParams() const
        {
            return mSamplers;
        }

        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="getter")
        const Techniques &getTechniques() const
        {
            return mTechniques;
        }

    private:
        Shader() = default;

        TPROPERTY(RTTRFuncName="Constants", RTTRFuncType="setter")
        void setConstantParams(const ShaderConstantParams &params)
        {
            mConstants = params;
        }

        TPROPERTY(RTTRFuncName="Samplers", RTTRFuncType="setter")
        void setSamplerParams(const ShaderSamplerParams &params)
        {
            mSamplers = params;
        }

        TPROPERTY(RTTRFuncName="Techniques", RTTRFuncType="setter")
        void setTechniques(const Techniques &techniques)
        {
            mTechniques = techniques;
        }

    protected:
        Shader(const String &name);
        
        ResourcePtr clone() const override;

        TResult onLoad() override;
        
    protected:
        /// shader 常量
        ShaderConstantParams    mConstants {};
        /// shader 采样器
        ShaderSamplerParams     mSamplers {};
        /// 所有可用的技术对象
        Techniques              mTechniques {};
        /// 当前硬件支持的 technique
        TechniquePtr            mSupportTechnique {nullptr};
    };
}


#endif    /*__T3D_SHADER_H__*/
