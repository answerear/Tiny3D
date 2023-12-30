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

#ifndef __T3D_PASS_H__
#define __T3D_PASS_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderVariant.h"


namespace Tiny3D
{
    using ShaderKeywords = TList<ShaderKeyword>;
    using ShaderVariants = TMap<ShaderKeyword, ShaderVariantPtr>;
    using ShaderVariantsValue = ShaderVariants::value_type;
    
    /**
     * \brief 渲染 pass
     */
    TCLASS()
    class T3D_ENGINE_API Pass
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static PassPtr create(const String &name);

        ~Pass() override = default;

        PassPtr clone() const;

        bool addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        TResult addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant);

        ShaderVariant *getCurrentVertexShader() const { return mCurrentVS; }

        ShaderVariant *getCurrentHullShader() const { return mCurrentHS; }

        ShaderVariant *getCurrentDomainShader() const { return mCurrentDS; }

        ShaderVariant *getCurrentGeometryShader() const { return mCurrentGS; }

        ShaderVariant *getCurrentPixelShader() const { return mCurrentPS; }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }
        
        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        RenderStatePtr getRenderState() const
        {
            return mRenderState;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(RenderStatePtr state)
        {
            mRenderState = state;
        }

        TPROPERTY(RTTRFuncName="Keywords", RTTRFuncType="getter")
        const ShaderKeywords &getShaderKeywords() const
        {
            return mKeywords;
        }

        TPROPERTY(RTTRFuncName="VertexShaders", RTTRFuncType="getter")
        const ShaderVariants &getVertexShaders() const
        {
            return mVertexShaders;
        }

        TPROPERTY(RTTRFuncName="PixelShaders", RTTRFuncType="getter")
        const ShaderVariants &getPixelShaders() const
        {
            return mPixelShaders;
        }

        TPROPERTY(RTTRFuncName="GeometryShaders", RTTRFuncType="getter")
        const ShaderVariants &getGeometryShaders() const
        {
            return mGeometryShaders;
        }

        TPROPERTY(RTTRFuncName="HullShaders", RTTRFuncType="getter")
        const ShaderVariants &getHullShaders() const
        {
            return mHullShaders;
        }

        TPROPERTY(RTTRFuncName="DomainShaders", RTTRFuncType="getter")
        const ShaderVariants &getDomainShaders() const
        {
            return mDomainShaders;
        }
    private:
        Pass() : Pass("") {}

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
        }

        TPROPERTY(RTTRFuncName="Keywords", RTTRFuncType="setter")
        void getShaderKeywords(const ShaderKeywords &keywords)
        {
            mKeywords = keywords;
        }

        TPROPERTY(RTTRFuncName="VertexShaders", RTTRFuncType="setter")
        void setVertexShaders(const ShaderVariants &variants)
        {
            mVertexShaders = variants;
        }

        TPROPERTY(RTTRFuncName="PixelShaders", RTTRFuncType="setter")
        void setPixelShaders(const ShaderVariants &variants)
        {
            mPixelShaders = variants;
        }

        TPROPERTY(RTTRFuncName="GeometryShaders", RTTRFuncType="setter")
        void setGeometryShaders(const ShaderVariants &variants)
        {
            mGeometryShaders = variants;
        }

        TPROPERTY(RTTRFuncName="HullShaders", RTTRFuncType="setter")
        void setHullShaders(const ShaderVariants &variants)
        {
            mHullShaders = variants;
        }

        TPROPERTY(RTTRFuncName="DomainShaders", RTTRFuncType="setter")
        void setDomainShaders(const ShaderVariants &variants)
        {
            mDomainShaders = variants;
        }
    protected:
        Pass(const String &name);

        /// Pass 名称
        String              mName {};
        /// Shader 名称
        String              mShaderName {};
        /// Tags
        ShaderLabTags       mTags {};
        /// 渲染状态
        RenderStatePtr      mRenderState {nullptr};
        
        /// 本 pass 着色器用到的所有宏
        ShaderKeywords      mKeywords {};
        /// 本 pass 所有 Vertex Shader 变体
        ShaderVariants      mVertexShaders {};
        /// 本 pass 所有 Pixel Shader 变体
        ShaderVariants      mPixelShaders {};
        /// 本 pass 所有 Geometry Shader 变体
        ShaderVariants      mGeometryShaders {};
        /// 本 pass 所有 Hull Shader 变体
        ShaderVariants      mHullShaders {};
        /// 本 pass 所有 Domain Shader 变体
        ShaderVariants      mDomainShaders {};
        
        /// 当前生效的关键字 
        ShaderKeyword       mCurrentKeyword {};
        
        /// 当前生效的 Vertex Shader 变体
        ShaderVariantPtr    mCurrentVS {nullptr};
        /// 当前生效的 Pixel Shader 变体
        ShaderVariantPtr    mCurrentPS {nullptr};
        /// 当前生效的 Geometry Shader 变体
        ShaderVariantPtr    mCurrentGS {nullptr};
        /// 当前生效的 Hull Shader 变体
        ShaderVariantPtr    mCurrentHS {nullptr};
        /// 当前生效的 Domain Shader 变体
        ShaderVariantPtr    mCurrentDS {nullptr};
        
        /// 关键字是否需要重新生成
        bool                mIsKeywordDirty {false};
    };
}


#endif    /*__T3D_PASS_H__*/
