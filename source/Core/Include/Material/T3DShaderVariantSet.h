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
     * \brief 同一 (stage, keyword) 下、按目标语言（图形 API）索引的多份 shader 变体集合。
     * \remarks 一个 ShaderVariantSet 对应一个关键字组合，其内部以 SHADER_LANGUAGE 为键，
     *          运行时按当前 renderer 推导出的着色语言选取对应的 ShaderVariant。
     *          这是多后端变体打包进同一只读 bundle 的核心容器。
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
         */
        static ShaderVariantSetPtr create();

        /**
         * \brief Destructor
         */
        ~ShaderVariantSet() override;

        /**
         * \brief 取指定语言的变体
         * \param [in] lang : 目标着色语言
         * \return 命中返回对应变体，否则返回 nullptr
         */
        ShaderVariantPtr getVariant(SHADER_LANGUAGE lang) const;

        /**
         * \brief 取当前 active 渲染后端对应语言的变体
         * \return 命中返回对应变体；当前后端语言缺失或无 active renderer 时返回 nullptr
         * \remarks 集中封装「active renderer → 着色语言 → 变体」的选取逻辑，
         *          运行时各处统一通过本方法取变体，避免重复散落选择代码。
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

        /**
         * \brief 是否为空
         */
        bool empty() const { return mVariants.empty(); }

        /**
         * \brief 变体数量
         */
        size_t size() const { return mVariants.size(); }

        TPROPERTY(RTTRFuncName="Variants", RTTRFuncType="getter")
        const ShaderVariantMap &getVariants() const { return mVariants; }

    protected:
        /**
         * \brief Constructor
         */
        ShaderVariantSet() = default;

        TPROPERTY(RTTRFuncName="Variants", RTTRFuncType="setter")
        void setVariants(const ShaderVariantMap &variants) { mVariants = variants; }

    protected:
        /// 按目标语言索引的变体集合
        ShaderVariantMap    mVariants {};
    };
}


#endif  /*__T3D_SHADER_VARIANT_SET_H__*/
