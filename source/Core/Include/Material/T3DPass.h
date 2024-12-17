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

#ifndef __T3D_PASS_H__
#define __T3D_PASS_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderVariant.h"


namespace Tiny3D
{
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

        TResult compile();

        /**
         * @brief 反射 shader 信息
         * @return 调用成功返回 T3D_OK
         */
        TResult reflect();

        void setTechnique(Technique *tech) { mTech = tech; }

        Technique *getTechnique() const { return mTech; }

        bool addTag(const String &key, const String &value);

        void removeTag(const String &key);
        
        bool getTag(const String &key, String &value) const;

        TResult addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant);

        TResult removeShaderVariant(const ShaderKeyword &keyword);

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
        const RenderStatePtr &getRenderState() const
        {
            return mRenderState;
        }

        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(const RenderStatePtr &state)
        {
            mRenderState = state;
        }

        TPROPERTY(RTTRFuncName="CurrentKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getCurrentKeyword() const
        {
            return mCurrentKeyword;
        }

        TPROPERTY(RTTRFuncName="CurrentKeyword", RTTRFuncType="setter")
        void setCurrentKeyword(const ShaderKeyword &keyword)
        {
            mCurrentKeyword = keyword;
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

        void onPostLoad() override;
        
    protected:
        /// Pass 所属的 Technique
        Technique           *mTech {nullptr};
        
        /// Pass 名称
        String              mName {};
        /// Shader 名称
        String              mShaderName {};
        /// Tags
        ShaderLabTags       mTags {};
        /// 渲染状态
        RenderStatePtr      mRenderState {nullptr};

        /// 当前的宏
        ShaderKeyword       mCurrentKeyword {};
        
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
        
        /// 关键字是否需要重新生成
        bool                mIsKeywordDirty {false};
    };
}


#endif    /*__T3D_PASS_H__*/
