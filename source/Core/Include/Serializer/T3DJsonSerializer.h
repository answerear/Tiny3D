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

#ifndef __T3D_JSON_SERIALIZER_H__
#define __T3D_JSON_SERIALIZER_H__


#include "Serializer/T3DSerializer.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief JSON 文本格式的序列化器实现。
     * \remarks 装配 JsonArchiveWriter / JsonArchiveReader 与 RttrArchive，
     *          自身不含 RTTR 遍历逻辑。
     */
    class T3D_ENGINE_API JsonSerializer : public Serializer
    {
    public:
        /**
         * \brief 创建 JsonSerializer 实例。
         * \return 新建的 JsonSerializer 智能指针
         */
        static JsonSerializerPtr create();

        /**
         * \brief 把可反射对象序列化为 JSON 文本流。
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的可反射对象
         * \return 成功返回 T3D_OK
         */
        TResult serialize(DataStream &stream, const RTTRObject &obj) override;

        /**
         * \brief 从 JSON 文本流反序列化出对象。
         * \param [in,out] stream : 输入数据流
         * \param [out] obj       : 还原出的对象变体
         * \return 文件头或 RttrArchive 读入失败时返回对应错误码；
         *         读入成功但 obj 无效时返回 T3D_ERR_DESERIALIZE_OBJECT
         */
        TResult deserialize(DataStream& stream, RTTRVariant& obj) override;

        /**
         * \brief 序列化指定类型对象到数据流（模板便捷入口）。
         * \tparam [in] T : 可反射类型
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的对象
         * \return 成功返回 T3D_OK
         */
        template<typename T>
        TResult serializeObject(DataStream &stream, const T &obj)
        {
            return serialize(stream, obj);
        }

        /**
         * \brief 从数据流反序列化并返回裸指针（堆上新建，调用方须 SmartPtr 接管）。
         * \tparam [in] T : 目标类型
         * \param [in,out] stream : 输入数据流
         * \return 成功且类型匹配时返回堆上对象指针；失败或类型不匹配时返回 nullptr
         * \remarks 类型不匹配时调用 discardUnclaimed 回收已建对象；var 须活到
         *          try_convert 之后，不可拆分语句。
         */
        template<typename T>
        T *deserializeObject(DataStream &stream)
        {
            RTTRVariant var;
            if (T3D_FAILED(deserialize(stream, var)))
            {
                return nullptr;
            }

            // 所有权说明同 SerializerManager::deserialize<T>：instance 只做指针校正，
            // 对象所有权随裸指针交给调用方，且 var 必须活到取指针之后。
            T *obj = RTTRObject(var).try_convert<T>();

            if (obj == nullptr)
            {
                discardUnclaimed(var, rttr::type::get<T>());
            }

            return obj;
        }

        /**
         * \brief 从数据流反序列化到已有对象（值语义）。
         * \tparam [in] T : 目标类型
         * \param [in,out] stream : 输入数据流
         * \param [out] obj       : 接收还原结果的目标对象
         * \return deserialize 的返回值
         * \remarks 无论 deserialize 是否成功都会执行 get_value<T>，且不校验 variant
         *          能否转换为目标类型。
         */
        template<typename T>
        TResult deserializeObject(DataStream& stream, T& obj)
        {
            RTTRVariant var;
            TResult ret = deserialize(stream, var);
            obj = var.get_value<T>();
            return ret;
        }
    };
}


#endif    /*__T3D_JSON_SERIALIZER_H__*/
