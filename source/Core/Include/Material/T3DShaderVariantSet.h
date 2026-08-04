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


#ifndef __T3D_SHADER_VARIANT_SET_H__
#define __T3D_SHADER_VARIANT_SET_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Material/T3DShaderVariant.h"


namespace Tiny3D
{
    /// 按目标着色语言索引的 shader 变体映射
    using ShaderVariantMap = TMap<SHADER_LANGUAGE, ShaderVariantPtr>;
    using ShaderVariantMapValue = ShaderVariantMap::value_type;

    /**
     * \brief 同一 (stage, keyword) 下、按目标语言索引的多份 shader 变体集合
     * \remarks 运行时按 active renderer 推导出的着色语言选取对应 ShaderVariant，
     *          是多后端变体打包进同一只读 bundle 的核心容器
     */
    TCLASS()
    class T3D_ENGINE_API ShaderVariantSet
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建空的 shader 变体集合
         * \return 新建的 ShaderVariantSet 智能指针
         */
        static ShaderVariantSetPtr create();

        /// 析构函数
        ~ShaderVariantSet() override;

        /**
         * \brief 获取指定语言的变体
         * \param [in] lang : 目标着色语言
         * \return 命中返回对应变体，否则返回 nullptr
         */
        ShaderVariantPtr getVariant(SHADER_LANGUAGE lang) const;

        /**
         * \brief 获取当前 active 渲染后端对应语言的变体
         * \return 命中返回对应变体；无 active renderer 或语言缺失时返回 nullptr
         */
        ShaderVariantPtr getActiveVariant() const;

        /**
         * \brief 放入或覆盖指定语言的变体
         * \param [in] lang : 目标着色语言
         * \param [in] variant : 变体对象
         */
        void putVariant(SHADER_LANGUAGE lang, ShaderVariantPtr variant);

        /**
         * \brief 移除指定语言的变体
         * \param [in] lang : 目标着色语言
         */
        void removeVariant(SHADER_LANGUAGE lang);

        /// 变体集合是否为空
        bool empty() const { return mVariants.empty(); }

        /// 变体数量
        size_t size() const { return mVariants.size(); }

        /// 获取全部语言变体映射
        TPROPERTY(RTTRFuncName="Variants", RTTRFuncType="getter")
        const ShaderVariantMap &getVariants() const { return mVariants; }

    protected:
        /// 默认构造
        ShaderVariantSet() = default;

        /// 设置全部语言变体映射（RTTR setter）
        TPROPERTY(RTTRFuncName="Variants", RTTRFuncType="setter")
        void setVariants(const ShaderVariantMap &variants) { mVariants = variants; }

    protected:
        /// 按目标语言索引的变体集合
        ShaderVariantMap    mVariants {};
    };
}


#endif  /*__T3D_SHADER_VARIANT_SET_H__*/
