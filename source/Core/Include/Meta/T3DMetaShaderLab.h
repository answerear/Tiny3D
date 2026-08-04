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

#ifndef __T3D_META_SHADER_LAB_H__
#define __T3D_META_SHADER_LAB_H__


#include "T3DMeta.h"


namespace Tiny3D
{
#if defined(T3D_OS_DESKTOP)

    /**
     * \brief ShaderLab 源文件（.tshaderlab）的 .meta 元数据
     * \remarks 除自身 UUID 外还记录编译产物 Shader 资源的 UUID，供编译命令与路径查找使用
     */
    TCLASS()
    class T3D_ENGINE_API MetaShaderLab : public Meta
    {
        TRTTI_ENABLE(Meta)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 MetaShaderLab 实例
         * \param [in] uuid : ShaderLab 源文件 Meta 的 UUID
         * \param [in] shaderUUID : 关联 Shader 资源的 UUID
         * \return 新建的 MetaShaderLabPtr
         */
        static MetaShaderLabPtr create(const UUID &uuid, const UUID &shaderUUID)
        {
            return T3D_NEW MetaShaderLab(uuid, shaderUUID);
        }

        /**
         * \brief 返回 kShaderLab 类型标识
         * \return Meta::kShaderLab
         */
        Type getType() const override { return kShaderLab; }

        TPROPERTY(RTTRFuncName="ShaderUUID", RTTRFuncType="getter")
        /// 返回关联 Shader 资源的 UUID
        const UUID &getShaderUUID() const { return mShaderUUID; }

        TPROPERTY(RTTRFuncName="ShaderUUID", RTTRFuncType="setter")
        /**
         * \brief 设置关联 Shader 资源的 UUID
         * \param [in] uuid : Shader 资源的 UUID
         */
        void setShaderUUID(const UUID &uuid) { mShaderUUID = uuid; }

    protected:
        /// 默认构造
        MetaShaderLab() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : ShaderLab 源文件 Meta 的 UUID
         * \param [in] shaderUUID : 关联 Shader 资源的 UUID
         */
        MetaShaderLab(const UUID &uuid, const UUID &shaderUUID)
            : Meta(uuid)
            , mShaderUUID(shaderUUID)
        {
        }

        /// 关联 Shader 资源的 UUID，默认 UUID::INVALID
        UUID mShaderUUID {UUID::INVALID};
    };

#endif
}


#endif    /*__T3D_META_SHADER_LAB_H__*/
