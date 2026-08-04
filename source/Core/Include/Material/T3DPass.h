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
#include "Material/T3DShaderVariantSet.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 渲染 pass 定义，持有各 shader stage 的多语言变体集合、Tags 与渲染状态
     */
    TCLASS()
    class T3D_ENGINE_API Pass
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建指定名称的 Pass 对象
         * \param [in] name : Pass 名称
         * \return 新建的 Pass 智能指针
         */
        static PassPtr create(const String &name);

        /// 析构函数
        ~Pass() override = default;

        /**
         * \brief 浅拷贝 Pass 的名称、Tags 与渲染状态
         * \return 新建的 Pass 副本；不复制 shader 变体集合
         */
        PassPtr clone() const;

        /**
         * \brief 编译本 pass 各 stage 下、当前渲染后端语言对应的 shader 变体
         * \return 调用成功返回 T3D_OK；任一 stage 无匹配变体或编译失败时返回对应错误码
         */
        TResult compile();

        /**
         * \brief 反射本 pass 各 stage 下、当前渲染后端语言对应的 shader 变体，提取常量与采样器绑定信息
         * \return 调用成功返回 T3D_OK；无匹配变体或反射失败时返回对应错误码
         */
        TResult reflect();

        /**
         * \brief 设置本 pass 所属的 Technique
         * \param [in] tech : 所属 Technique，可为 nullptr
         */
        void setTechnique(Technique *tech) { mTech = tech; }

        /// 获取本 pass 所属的 Technique
        Technique *getTechnique() const { return mTech; }

        /**
         * \brief 添加 Tag；若 key 不存在则插入并同步解析内置 Tag（如 LightMode）
         * \param [in] key : Tag 键
         * \param [in] value : Tag 值
         * \return key 不存在且插入成功时返回 true；key 已存在时返回 false
         */
        bool addTag(const String &key, const String &value);

        /**
         * \brief 移除 Tag 并重置对应内置 Tag 的解析值（如 LightMode 恢复为 None）
         * \param [in] key : Tag 键
         */
        void removeTag(const String &key);
        
        /**
         * \brief 查找 Tag 值
         * \param [in] key : Tag 键
         * \param [out] value : 找到的 Tag 值
         * \return key 存在时返回 true，否则返回 false
         */
        bool getTag(const String &key, String &value) const;

        /**
         * \brief 更新已存在的 Tag 值并同步解析内置 Tag
         * \param [in] key : Tag 键
         * \param [in] value : 新 Tag 值
         * \return key 存在且更新成功时返回 true；key 不存在时返回 false
         */
        bool setTag(const String &key, const String &value);

        /// 获取光照模式（由 LightMode Tag 解析得到的内置枚举值）
        uint32_t getLightMode() const { return mLightMode; }

        /**
         * \brief 按 shader stage 将变体加入对应集合，并按语言写入 ShaderVariantSet
         * \param [in] keyword : 变体对应的关键字
         * \param [in] variant : shader 变体对象
         * \return 调用成功返回 T3D_OK；Compute stage 返回 T3D_ERR_NOT_IMPLEMENT；未知 stage 返回 T3D_ERR_RES_INVALID_SHADER
         */
        TResult addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant);

        /**
         * \brief 从各 stage 集合中移除指定 keyword 下的全部语言变体，并清空变体的 pass 引用
         * \param [in] keyword : 要移除的关键字
         * \return 调用成功返回 T3D_OK；keyword 不存在时返回 T3D_ERR_NOT_FOUND
         */
        TResult removeShaderVariant(const ShaderKeyword &keyword);

        /// 获取 Pass 名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const
        {
            return mName;
        }

        /// 获取 Tags 映射表
        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const
        {
            return mTags;
        }
        
        /// 获取渲染状态
        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        const RenderStatePtr &getRenderState() const
        {
            return mRenderState;
        }

        /// 设置渲染状态
        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(const RenderStatePtr &state)
        {
            mRenderState = state;
        }

        /// 获取当前选中的 shader 关键字
        TPROPERTY(RTTRFuncName="CurrentKeyword", RTTRFuncType="getter")
        const ShaderKeyword &getCurrentKeyword() const
        {
            return mCurrentKeyword;
        }

        /// 设置当前选中的 shader 关键字
        TPROPERTY(RTTRFuncName="CurrentKeyword", RTTRFuncType="setter")
        void setCurrentKeyword(const ShaderKeyword &keyword)
        {
            mCurrentKeyword = keyword;
        }

        /// 获取本 pass 登记的全部 shader 关键字列表
        TPROPERTY(RTTRFuncName="Keywords", RTTRFuncType="getter")
        const ShaderKeywords &getShaderKeywords() const
        {
            return mKeywords;
        }

        /// 获取 Vertex Shader 变体集合（keyword → 多语言 ShaderVariantSet）
        TPROPERTY(RTTRFuncName="VertexShaders", RTTRFuncType="getter")
        const ShaderVariantSets &getVertexShaders() const
        {
            return mVertexShaders;
        }

        /// 获取 Pixel Shader 变体集合
        TPROPERTY(RTTRFuncName="PixelShaders", RTTRFuncType="getter")
        const ShaderVariantSets &getPixelShaders() const
        {
            return mPixelShaders;
        }

        /// 获取 Geometry Shader 变体集合
        TPROPERTY(RTTRFuncName="GeometryShaders", RTTRFuncType="getter")
        const ShaderVariantSets &getGeometryShaders() const
        {
            return mGeometryShaders;
        }

        /// 获取 Hull Shader 变体集合
        TPROPERTY(RTTRFuncName="HullShaders", RTTRFuncType="getter")
        const ShaderVariantSets &getHullShaders() const
        {
            return mHullShaders;
        }

        /// 获取 Domain Shader 变体集合
        TPROPERTY(RTTRFuncName="DomainShaders", RTTRFuncType="getter")
        const ShaderVariantSets &getDomainShaders() const
        {
            return mDomainShaders;
        }
    private:
        /// 默认构造，供 RTTR 反序列化使用
        Pass() : Pass("") {}

        /// 设置 Pass 名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name)
        {
            mName = name;
        }

        /// 设置 Tags 并重新解析全部内置 Tag 值（RTTR setter）
        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
            toTagValues();
        }
        
        /// 设置 shader 关键字列表（RTTR setter）
        TPROPERTY(RTTRFuncName="Keywords", RTTRFuncType="setter")
        void getShaderKeywords(const ShaderKeywords &keywords)
        {
            mKeywords = keywords;
        }

        /// 设置 Vertex Shader 变体集合（RTTR setter）
        TPROPERTY(RTTRFuncName="VertexShaders", RTTRFuncType="setter")
        void setVertexShaders(const ShaderVariantSets &variants)
        {
            mVertexShaders = variants;
        }

        /// 设置 Pixel Shader 变体集合（RTTR setter）
        TPROPERTY(RTTRFuncName="PixelShaders", RTTRFuncType="setter")
        void setPixelShaders(const ShaderVariantSets &variants)
        {
            mPixelShaders = variants;
        }

        /// 设置 Geometry Shader 变体集合（RTTR setter）
        TPROPERTY(RTTRFuncName="GeometryShaders", RTTRFuncType="setter")
        void setGeometryShaders(const ShaderVariantSets &variants)
        {
            mGeometryShaders = variants;
        }

        /// 设置 Hull Shader 变体集合（RTTR setter）
        TPROPERTY(RTTRFuncName="HullShaders", RTTRFuncType="setter")
        void setHullShaders(const ShaderVariantSets &variants)
        {
            mHullShaders = variants;
        }

        /// 设置 Domain Shader 变体集合（RTTR setter）
        TPROPERTY(RTTRFuncName="DomainShaders", RTTRFuncType="setter")
        void setDomainShaders(const ShaderVariantSets &variants)
        {
            mDomainShaders = variants;
        }
    protected:
        /**
         * \brief 构造指定名称的 Pass，并初始化当前关键字的 hash
         * \param [in] name : Pass 名称
         */
        Pass(const String &name);

        /// 反序列化完成后，为各 stage 变体重新设置所属 pass 指针
        void onPostLoad() override;

        /// 遍历全部 Tags 并逐个解析内置 Tag 值
        void toTagValues();
        
        /// 解析单个 Tag 键值对到对应内置字段（当前支持 LightMode）
        void toTagValue(const String &key, const String &value);

        /// 移除 Tag 时将对应内置字段恢复为默认值
        void resetTagValue(const String &key);
        
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

        /// 光照模式（由 LightMode Tag 解析）
        uint32_t            mLightMode {ShaderLab::kBuiltinLightModeNone};

        /// 当前选中的 shader 关键字
        ShaderKeyword       mCurrentKeyword {};
        
        /// 本 pass 登记的全部 shader 关键字
        ShaderKeywords      mKeywords {};
        /// Vertex Shader 变体（keyword → 多语言变体集合）
        ShaderVariantSets   mVertexShaders {};
        /// Pixel Shader 变体
        ShaderVariantSets   mPixelShaders {};
        /// Geometry Shader 变体
        ShaderVariantSets   mGeometryShaders {};
        /// Hull Shader 变体
        ShaderVariantSets   mHullShaders {};
        /// Domain Shader 变体
        ShaderVariantSets   mDomainShaders {};
        
        /// 关键字是否需要重新生成
        bool                mIsKeywordDirty {false};
    };
}


#endif    /*__T3D_PASS_H__*/
