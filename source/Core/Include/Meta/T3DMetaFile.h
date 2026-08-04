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

#ifndef __T3D_META_FILE_H__
#define __T3D_META_FILE_H__


#include "T3DMeta.h"


namespace Tiny3D
{
#if defined(T3D_OS_DESKTOP)

    /**
     * \brief 通用文件的 .meta 元数据
     * \remarks 扩展名无法归入其他专用 Meta 类型时使用；UUID 由调用方生成
     */
    TCLASS()
    class T3D_ENGINE_API MetaFile : public Meta
    {
        TRTTI_ENABLE(Meta)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建 MetaFile 实例
         * \param [in] uuid : Meta 的 UUID
         * \return 新建的 MetaFilePtr
         */
        static MetaFilePtr create(const UUID &uuid)
        {
            return T3D_NEW MetaFile(uuid);
        }

        /**
         * \brief 返回 kFile 类型标识
         * \return Meta::kFile
         */
        Type getType() const override { return kFile; }

    protected:
        /// 默认构造
        MetaFile() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : Meta 的 UUID
         */
        MetaFile(const UUID &uuid)
            : Meta(uuid)
        {
        }
    };

#endif
}


#endif    /*__T3D_META_FILE_H__*/
