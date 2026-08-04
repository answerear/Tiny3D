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

#ifndef __T3D_SERIALIZER_H__
#define __T3D_SERIALIZER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief (反)序列化器抽象接口：一种落盘格式对应一个实现。
     * \remarks 共享 RTTR 遍历语义由 RttrArchive 统一实现；BinSerializer /
     *          JsonSerializer 等派生类仅装配对应格式的归档读写后端。
     */
    class T3D_ENGINE_API Serializer : public Object, public Noncopyable
    {
        T3D_DECLARE_INTERFACE(Serializer);

    public:
        /**
         * \brief 把可反射对象序列化写入数据流。
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的可反射对象
         * \return 成功返回 T3D_OK；失败含义由 RttrArchive 与归档后端决定
         */
        virtual TResult serialize(DataStream &stream, const RTTRObject &obj) = 0;

        /**
         * \brief 从数据流反序列化出对象。
         * \param [in,out] stream : 输入数据流
         * \param [out] obj       : 还原出的对象变体，由调用方持有生命周期
         * \return 成功返回 T3D_OK；文件头校验失败返回对应错误码；
         *         派生实现还会在 obj 无效时返回 T3D_ERR_DESERIALIZE_OBJECT
         * \remarks 只提供出参形式：返回 RTTRObject(=rttr::instance) 会把非拥有
         *          视图的生命周期隐患留给调用方，见 SerializerManager::deserializeObject。
         */
        virtual TResult deserialize(DataStream &stream, RTTRVariant &obj) = 0;

        /**
         * \brief 回收无人接管的反序列化产物。
         * \param [in,out] var : 已还原出的对象，回收后置为无效变体
         * \param [in] wanted  : 调用方请求的类型，仅用于报错日志
         * \remarks var 无效时直接返回。as_object 策略的值类型由 variant 自行释放；
         *          as_raw_ptr 产出的裸指针 variant 析构时不释放所指对象，须通过
         *          ObjectPtr 接管并释放，否则整棵对象图泄漏。
         */
        static void discardUnclaimed(RTTRVariant &var, const rttr::type &wanted)
        {
            if (!var.is_valid())
            {
                return;
            }

            T3D_LOG_ERROR("Serializer",
                "Deserialized type [%s] mismatches requested type [%s] !",
                var.get_type().get_name().data(), wanted.get_name().data());

            // as_object 策略的值类型由 variant 自己持有并释放，无需接管；只有
            // as_raw_ptr 产出的裸指针才无人释放。
            if (var.get_type().is_pointer())
            {
                // 引用计数从 0 起，构造 acquire 到 1，出作用域 release 归零即
                // delete，正好一次释放。
                ObjectPtr unclaimed = RTTRObject(var).try_convert<Object>();
                if (unclaimed == nullptr)
                {
                    T3D_LOG_WARNING("Serializer",
                        "Type [%s] is not derived from Object, cannot reclaim it !",
                        var.get_type().get_name().data());
                }
            }

            var = RTTRVariant();
        }
    };
}


#endif    /*__T3D_SERIALIZER_H__*/
