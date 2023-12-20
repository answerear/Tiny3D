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

        TResult enableKeyword(const String &keyword);

        TResult disableKeyword(const String &keyword);

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

        TechniquePtr getCurrentTechnique() const { return mCurTechnique; }

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

        /// shader 常量
        ShaderConstantParams    mConstants {};
        /// shader 采样器
        ShaderSamplerParams     mSamplers {};
        /// 所有可用的技术对象
        Techniques              mTechniques {};
        /// 当前可用的技术对象
        TechniquePtr            mCurTechnique {nullptr};
    };
}


#endif    /*__T3D_SHADER_H__*/
