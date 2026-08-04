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

#ifndef __T3D_META_PREFAB_H__
#define __T3D_META_PREFAB_H__


#include "T3DMeta.h"


namespace Tiny3D
{
#if defined(T3D_OS_DESKTOP)

    /**
     * \brief Prefab 资源（.tprefab）的 .meta 元数据
     * \remarks UUID 与对应 Prefab 资源的 UUID 一致
     */
    TCLASS()
    class T3D_ENGINE_API MetaPrefab : public Meta
    {
        TRTTI_ENABLE(Meta)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 MetaPrefab 实例
         * \param [in] uuid : 与 Prefab 资源相同的 UUID
         * \return 新建的 MetaPrefabPtr
         */
        static MetaPrefabPtr create(const UUID &uuid)
        {
            return T3D_NEW MetaPrefab(uuid);
        }

        /**
         * \brief 返回 kPrefab 类型标识
         * \return Meta::kPrefab
         */
        Type getType() const override { return kPrefab; }

    protected:
        /// 默认构造
        MetaPrefab() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 与 Prefab 资源相同的 UUID
         */
        MetaPrefab(const UUID &uuid)
            : Meta(uuid)
        {
        }
    };

#endif
}


#endif    /*__T3D_META_PREFAB_H__*/
