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

#ifndef __T3D_SERIALIZER_MANAGER_H__
#define __T3D_SERIALIZER_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Serializer/T3DSerializer.h"


namespace Tiny3D
{
    /**
     * \brief 序列化管理器：按配置选择格式写出，按文件头 magic 自动选格式读入。
     */
    class T3D_ENGINE_API SerializerManager
        : public Object
        , public Singleton<SerializerManager>
    {
    public:
        /**
         * \brief 序列化落盘格式。
         */
        enum class FileMode : uint32_t
        {
            kBinary = 0,    ///< T3DB 二进制格式（写出时使用）
            kText,          ///< JSON 文本格式（写出时使用）
            kMax
        };

        /**
         * \brief 创建 SerializerManager 实例。
         * \return 新建的管理器智能指针
         */
        static SerializerManagerPtr create();

        /**
         * \brief 获取当前写出格式。
         * \return 当前 FileMode，默认 kText
         */
        FileMode getFileMode() const { return mFileMode; }

        /**
         * \brief 设置写出格式并重建对应序列化器。
         * \param [in] mode : 目标格式；kBinary 选 BinSerializer，kText 选 JsonSerializer
         */
        void setFileMode(FileMode mode)
        {
            mFileMode = mode;
            mSerializer = createSerializer();
        }

        /**
         * \brief 是否已在(反)序列化时触发对象生命周期回调。
         * \return 默认 true
         */
        bool isInvokeLifecycleCallbacks() const
        {
            return mInvokeLifecycleCallbacks;
        }

        /**
         * \brief 设置(反)序列化时是否触发对象生命周期回调。
         * \param [in] invoke : true 触发 onPreSave/onPostSave/onPostLoad 等
         * \remarks 反序列化时 onPostLoad 会重建运行时关系（如骨架/场景/预制件的
         *          父子层级），其中父子节点以智能指针互相持有形成引用环，仅由运行时
         *          卸载(onUnload)负责断开。BundleBuilder 等离线格式转换工具应关闭，
         *          以避免引用环泄漏及 onPreSave/onPostLoad 破坏字节级往返一致性。
         */
        void setInvokeLifecycleCallbacks(bool invoke)
        {
            mInvokeLifecycleCallbacks = invoke;
        }

        /**
         * \brief 序列化指定类型对象到数据流（模板便捷入口）。
         * \tparam [in] T : 可反射类型
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的对象
         * \return 成功返回 T3D_OK；mSerializer 为空时仍返回 T3D_OK 但不写出
         */
        template<typename T>
        TResult serialize(DataStream &stream, const T &obj)
        {
            return serializeObject(stream, obj);
        }

        /**
         * \brief 从数据流反序列化并返回裸指针（堆上新建，调用方须 SmartPtr 接管）。
         * \tparam [in] T : 目标类型
         * \param [in,out] stream : 输入数据流，按 magic 自动选反序列化器
         * \return 成功且类型匹配时返回堆上对象指针；失败或类型不匹配时返回 nullptr
         * \remarks 类型不匹配时调用 Serializer::discardUnclaimed；var 须活到
         *          try_convert 之后，不可拆分语句。
         */
        template<typename T>
        T *deserialize(DataStream &stream)
        {
            RTTRVariant var;
            if (T3D_FAILED(deserializeObject(stream, var)))
            {
                return nullptr;
            }

            // RTTRObject(instance) 在此仅用于做基类/派生类的指针校正，不涉及所有权：
            // 对象由 RTTR 的 as_raw_ptr 策略在堆上 new 出，随返回的裸指针把所有权
            // 交给调用方，调用方须立即用 SmartPtr 接管（各 ResourceManager 即如此）。
            // var 必须活到取指针之后，故不可把这两步拆到不同语句。
            T *obj = RTTRObject(var).try_convert<T>();

            if (obj == nullptr)
            {
                Serializer::discardUnclaimed(var, rttr::type::get<T>());
            }

            return obj;
        }

        /**
         * \brief 从数据流反序列化到已有对象（值语义）。
         * \tparam [in] T : 目标类型
         * \param [in,out] stream : 输入数据流
         * \param [out] obj       : 接收还原结果的目标对象
         * \return deserializeObject 失败或 variant 无效/无法转换为 T 时返回错误码
         */
        template<typename T>
        TResult deserialize(DataStream& stream, T& obj)
        {
            RTTRVariant var;
            TResult ret = deserializeObject(stream, var);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            if (!var.is_valid() || !var.can_convert<T>())
            {
                T3D_LOG_ERROR("Serializer",
                    "Deserialized variant is invalid or cannot convert to target type [%s]",
                    rttr::type::get<T>().get_name().data());
                return T3D_ERR_FAIL;
            }

            obj = var.get_value<T>();
            return ret;
        }

        /**
         * \brief 序列化可反射对象到数据流（使用当前 FileMode 对应的序列化器）。
         * \param [in,out] stream : 输出数据流
         * \param [in] obj        : 待序列化的可反射对象
         * \return mSerializer 非空时返回其 serialize 结果；为空时返回 T3D_OK
         */
        TResult serializeObject(DataStream &stream, const RTTRObject &obj);

        /**
         * \brief 从数据流反序列化出类型无关的对象（唯一的反序列化出口）。
         * \param [in,out] stream : 输入数据流，按文件头 magic 自动选反序列化器
         * \param [out] obj       : 还原出的对象变体，由调用方持有生命周期
         * \return 成功返回 T3D_OK
         * \remarks 不提供返回 RTTRObject(=rttr::instance) 的重载：instance 是非拥有
         *          视图，从局部 variant 构造再返回时正确性隐式依赖 as_raw_ptr 策略。
         */
        TResult deserializeObject(DataStream &stream, RTTRVariant &obj);

    protected:
        /**
         * \brief 按当前 FileMode 创建写出用序列化器。
         * \return kBinary 返回 BinSerializer，kText 返回 JsonSerializer；未知模式返回空
         */
        SerializerPtr createSerializer();

        /**
         * \brief 按文件头 magic 自动选择反序列化器。
         * \param [in,out] stream : 数据流（peek 头 4 字节后回退到原位置）
         * \return magic 为 T3DB 时返回 BinSerializer，否则返回 JsonSerializer；
         *         tell 失败时退回当前 mFileMode 对应的 mSerializer
         */
        SerializerPtr pickDeserializer(DataStream &stream);

        /// 当前写出格式，默认 kText。
        FileMode        mFileMode = FileMode::kText;
        /// 与 mFileMode 对应的写出用序列化器。
        SerializerPtr   mSerializer;
        /// (反)序列化时是否触发对象生命周期回调，默认 true。
        bool            mInvokeLifecycleCallbacks = true;
    };

    #define T3D_SERIALIZER_MGR      (SerializerManager::getInstance())

    T3D_EXTERN_SINGLETON(SerializerManager)
}


#endif    /*__T3D_SERIALIZER_MANAGER_H__*/
