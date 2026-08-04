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

#ifndef __T3D_BIN_SERIALIZER_H__
#define __T3D_BIN_SERIALIZER_H__


#include "T3DTypedef.h"
#include "Serializer/T3DSerializer.h"


namespace Tiny3D
{
    /**
     * \brief T3DB 二进制格式的序列化器实现。
     * \remarks 装配 BinArchiveWriter / BinArchiveReader 与 RttrArchive，
     *          自身不含 RTTR 遍历逻辑。
     */
    class T3D_ENGINE_API BinSerializer : public Serializer
    {
    public:
        /**
         * \brief 创建 BinSerializer 实例。
         * \return 新建的 BinSerializer 智能指针
         */
        static BinSerializerPtr create();

        /**
         * \brief 把可反射对象序列化为 T3DB 二进制流。
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的可反射对象
         * \return 成功返回 T3D_OK
         */
        TResult serialize(DataStream &stream, const RTTRObject &obj) override;

        /**
         * \brief 从 T3DB 二进制流反序列化出对象。
         * \param [in,out] stream : 输入数据流
         * \param [out] obj       : 还原出的对象变体
         * \return 文件头或 RttrArchive 读入失败时返回对应错误码；
         *         读入成功但 obj 无效时返回 T3D_ERR_DESERIALIZE_OBJECT
         */
        TResult deserialize(DataStream& stream, RTTRVariant& obj) override;

    protected:
        /// 默认构造，供 create() 使用。
        BinSerializer() = default;
    };
}


#endif    /*__T3D_BIN_SERIALIZER_H__*/
