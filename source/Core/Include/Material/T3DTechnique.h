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

#ifndef __T3D_TECHNIQUE_H__
#define __T3D_TECHNIQUE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 渲染 Technique 定义，包含多个 Pass、Tags、LOD 与渲染队列
     */
    TCLASS()
    class T3D_ENGINE_API Technique
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建指定名称的 Technique 对象
         * \param [in] name : Technique 名称
         * \return 新建的 Technique 智能指针
         */
        static TechniquePtr create(const String &name);

        /// 析构函数
        ~Technique() override = default;

        /**
         * \brief 深拷贝 Technique，包括各 Pass 的 clone
         * \return 新建的 Technique 副本
         */
        TechniquePtr clone() const;

        /**
         * \brief 依次编译全部 Pass 的 shader 变体
         * \return 调用成功返回 T3D_OK；任一 Pass 编译失败时返回对应错误码
         */
        TResult compile();

        /**
         * \brief 依次反射全部 Pass 的 shader 变体
         * \return 调用成功返回 T3D_OK；任一 Pass 反射失败时返回对应错误码
         */
        TResult reflect();

        /**
         * \brief 设置所属 Shader
         * \param [in] shader : 所属 Shader，可为 nullptr
         */
        void setShader(Shader *shader) { mShader = shader; }

        /// 获取所属 Shader
        Shader *getShader() const { return mShader; }

        /**
         * \brief 添加 Tag；若 key 不存在则插入并同步解析内置 Tag（如 Queue）
         * \param [in] key : Tag 键
         * \param [in] value : Tag 值
         * \return key 不存在且插入成功时返回 true；key 已存在时返回 false
         */
        bool addTag(const String &key, const String &value);

        /**
         * \brief 移除 Tag 并重置对应内置 Tag 的解析值
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

        /**
         * \brief 添加 Pass；同名 Pass 不会重复添加
         * \param [in] pass : Pass 对象
         * \return 添加成功返回 true；同名 Pass 已存在时返回 false
         */
        bool addPass(PassPtr pass);

        /**
         * \brief 按名称移除 Pass 并清空其 Technique 引用
         * \param [in] name : Pass 名称
         */
        void removePass(const String &name);

        /**
         * \brief 按名称查找 Pass
         * \param [in] name : Pass 名称
         * \param [out] pass : 找到的 Pass 对象
         * \return 找到时返回 true，否则返回 false
         */
        bool getPass(const String &name, PassPtr &pass) const;

        /// 获取 Technique 名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }
        
        /// 获取 LOD 级别
        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="getter")
        uint32_t getLOD() const { return mLOD; }

        /// 设置 LOD 级别
        TPROPERTY(RTTRFuncName="LOD", RTTRFuncType="setter")
        void setLOD(uint32_t LOD) { mLOD = LOD; }

        /// 获取渲染队列（由 Queue Tag 解析得到的内置枚举值）
        uint32_t getRenderQueue() const { return mRenderQueue; }

        /// 获取 Tags 映射表
        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="getter")
        const ShaderLabTags &getTags() const { return mTags; }

        /// 获取渲染状态
        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="getter")
        RenderStatePtr getRenderState() const { return mRenderState; }

        /// 设置渲染状态
        TPROPERTY(RTTRFuncName="RenderState", RTTRFuncType="setter")
        void setRenderState(RenderStatePtr state) { mRenderState = state; }

        /// 获取 Pass 列表
        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="getter")
        const Passes &getPasses() const { return mPasses; }
        
    private:
        /// 默认构造，供 RTTR 反序列化使用
        Technique() : Technique("") {}

        /// 设置 Technique 名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        /// 设置 Tags 并重新解析全部内置 Tag 值（RTTR setter）
        TPROPERTY(RTTRFuncName="Tags", RTTRFuncType="setter")
        void setTags(const ShaderLabTags &tags)
        {
            mTags = tags;
            toTagValues();
        }

        /// 设置 Pass 列表（RTTR setter）
        TPROPERTY(RTTRFuncName="Passes", RTTRFuncType="setter")
        void setPasses(const Passes &passes) { mPasses = passes; }
        
    protected:
        /**
         * \brief 构造指定名称的 Technique
         * \param [in] name : Technique 名称
         */
        Technique(const String &name);

        /// 遍历全部 Tags 并逐个解析内置 Tag 值
        void toTagValues();

        /// 解析单个 Tag 键值对到对应内置字段（当前支持 Queue）
        void toTagValue(const String &key, const String &value);

        /// 移除 Tag 时将对应内置字段恢复为默认值
        void resetTagValue(const String &key);

        /// 反序列化完成后，为各 Pass 重新设置所属 Technique 指针
        void onPostLoad() override;
        
    protected:
        /// 所属 Shader
        Shader          *mShader {nullptr};
        /// Technique 名称
        String          mName {};
        /// LOD 级别
        uint32_t        mLOD {0};
        /// 渲染队列（由 Queue Tag 解析）
        uint32_t        mRenderQueue {ShaderLab::kBuiltinQueueGeometry};
        /// Tags
        ShaderLabTags   mTags {};
        /// 渲染状态
        RenderStatePtr  mRenderState {nullptr};
        /// Pass 列表
        Passes          mPasses {};
    };
}


#endif    /*__T3D_TECHNIQUE_H__*/
