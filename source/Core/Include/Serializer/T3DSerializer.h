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
     * \brief (反)序列化器接口：一种落盘格式一个实现。
     * \remarks 语义只在共享遍历层 RttrArchive 实现一份，派生类仅负责把归档后端
     *          （IArchiveWriter / IArchiveReader）接上，因此各实现都只有薄封装。
     *          行为基线与格式版本见 T3DRttrArchive.h 与两个归档头的说明。
     */
    class T3D_ENGINE_API Serializer : public Object, public Noncopyable
    {
        T3D_DECLARE_INTERFACE(Serializer);

    public:
        virtual TResult serialize(DataStream &stream, const RTTRObject &obj) = 0;

        /**
         * \brief 从数据流反序列化出对象。
         * \remarks 只提供出参形式：见 SerializerManager::deserializeObject 说明，
         *          返回 RTTRObject(=rttr::instance) 的形式会把非拥有视图的生命周期
         *          隐患留给调用方。
         */
        virtual TResult deserialize(DataStream &stream, RTTRVariant &obj) = 0;

        /**
         * \brief 回收无人接管的反序列化产物。
         * \param [in,out] var : 已还原出的对象，回收后置为无效
         * \param [in] wanted  : 调用方请求的类型，仅用于报错
         * \remarks 供返回裸指针的 deserialize<T> 在类型不匹配时调用：还原本身成功，
         *          对象已在堆上建好，但既不会返回给调用方、也不会有人接管它。
         *          Object 派生类一律以 as_raw_ptr 策略构造（见 ReflectionPreprocessor），
         *          variant 里存的只是裸指针、析构时不释放所指对象，故必须就地销毁，
         *          否则连同其全部 SmartPtr 成员构成的整棵对象图一起泄漏。
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
