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

#ifndef __T3D_SHADER_RESOURCE_PARAM_H__
#define __T3D_SHADER_RESOURCE_PARAM_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 着色器中的缓冲型资源绑定反射描述
     * \remarks 覆盖结构化缓冲、字节地址缓冲、typed buffer 与可读写纹理。
     *          与 ShaderConstantParam（cbuffer 布局）、ShaderSamplerParam
     *          （纹理 + 采样器）并列，是第三类反射产物。
     */
    TCLASS()
    class T3D_ENGINE_API ShaderResourceParam
        : public Object
        , public Noncopyable
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 资源在 shader 中的绑定形态
         */
        TENUM()
        enum class Kind : uint32_t
        {
            /// StructuredBuffer<T>
            kStructuredSRV = 0,
            /// ByteAddressBuffer
            kByteAddressSRV,
            /// Buffer<T>
            kTypedSRV,
            /// RWStructuredBuffer<T>
            kStructuredUAV,
            /// RWByteAddressBuffer
            kByteAddressUAV,
            /// RWBuffer<T>
            kTypedUAV,
            /// RWTexture1D / RWTexture2D / RWTexture3D
            kTextureUAV,
            /// AppendStructuredBuffer<T>
            kAppendUAV,
            /// ConsumeStructuredBuffer<T>
            kConsumeUAV,
        };

        /**
         * \brief 创建资源绑定参数对象
         * \param [in] name : shader 中的资源名称
         * \param [in] kind : 绑定形态
         * \param [in] binding : t# 或 u# 寄存器索引
         * \param [in] elementStride : 结构化缓冲的元素字节数，其余形态传 0
         * \param [in] hasCounter : 是否带隐藏计数器（Append/Consume 或用了 IncrementCounter）
         * \return 新建的 ShaderResourceParam 智能指针
         */
        static ShaderResourceParamPtr create(const String &name, Kind kind,
            uint32_t binding, uint32_t elementStride = 0, bool hasCounter = false);

        /// 析构函数
        ~ShaderResourceParam() override;

        /**
         * \brief 复制全部字段创建新对象
         * \return 新建的 ShaderResourceParam 副本
         */
        ShaderResourceParamPtr clone() const;

        /// 获取资源名称
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        /// 获取绑定形态
        TPROPERTY(RTTRFuncName="Kind", RTTRFuncType="getter")
        Kind getKind() const { return mKind; }

        /// 设置绑定形态
        TPROPERTY(RTTRFuncName="Kind", RTTRFuncType="setter")
        void setKind(Kind kind) { mKind = kind; }

        /// 获取 t# 或 u# 寄存器索引
        TPROPERTY(RTTRFuncName="Binding", RTTRFuncType="getter")
        uint32_t getBinding() const { return mBinding; }

        /// 设置寄存器索引
        TPROPERTY(RTTRFuncName="Binding", RTTRFuncType="setter")
        void setBinding(uint32_t binding) { mBinding = binding; }

        /// 获取结构化缓冲的元素字节数，其余形态为 0
        TPROPERTY(RTTRFuncName="ElementStride", RTTRFuncType="getter")
        uint32_t getElementStride() const { return mElementStride; }

        /// 设置元素字节数
        TPROPERTY(RTTRFuncName="ElementStride", RTTRFuncType="setter")
        void setElementStride(uint32_t stride) { mElementStride = stride; }

        /// 是否带隐藏计数器
        TPROPERTY(RTTRFuncName="HasCounter", RTTRFuncType="getter")
        bool hasCounter() const { return mHasCounter; }

        /// 设置是否带隐藏计数器
        TPROPERTY(RTTRFuncName="HasCounter", RTTRFuncType="setter")
        void setHasCounter(bool hasCounter) { mHasCounter = hasCounter; }

        /// 该绑定是否占用 UAV 槽位（u#），否则占用 SRV 槽位（t#）
        bool isUnorderedAccess() const
        {
            return mKind >= Kind::kStructuredUAV;
        }

    private:
        /// 默认构造，供 clone 与 RTTR 使用
        ShaderResourceParam() = default;

        /// 设置资源名称（RTTR setter）
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

    protected:
        /**
         * \brief 构造资源绑定参数对象
         * \param [in] name : shader 中的资源名称
         * \param [in] kind : 绑定形态
         * \param [in] binding : 寄存器索引
         * \param [in] elementStride : 结构化缓冲的元素字节数
         * \param [in] hasCounter : 是否带隐藏计数器
         */
        ShaderResourceParam(const String &name, Kind kind, uint32_t binding,
            uint32_t elementStride, bool hasCounter);

        /// 资源名称
        String      mName {};
        /// 绑定形态
        Kind        mKind {Kind::kStructuredSRV};
        /// t# 或 u# 寄存器索引
        uint32_t    mBinding {0};
        /// 结构化缓冲的元素字节数
        uint32_t    mElementStride {0};
        /// 是否带隐藏计数器
        bool        mHasCounter {false};
    };
}


#endif    /*__T3D_SHADER_RESOURCE_PARAM_H__*/
