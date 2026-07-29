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
     * \brief 序列化管理类，负责序列化对象和反序列化对象
     */
    class T3D_ENGINE_API SerializerManager
        : public Object
        , public Singleton<SerializerManager>
    {
    public:
        enum class FileMode : uint32_t
        {
            kBinary = 0,
            kText,
            kMax
        };

        static SerializerManagerPtr create();

        FileMode getFileMode() const { return mFileMode; }
        
        void setFileMode(FileMode mode)
        {
            mFileMode = mode;
            mSerializer = createSerializer();
        }

        /**
         * \brief 是否在(反)序列化时触发对象生命周期回调
         *        （onPostLoad / onAddComponentForLoadingResource 等）。
         */
        bool isInvokeLifecycleCallbacks() const
        {
            return mInvokeLifecycleCallbacks;
        }

        /**
         * \brief 设置(反)序列化时是否触发对象生命周期回调。
         * \remarks 反序列化时序列化器会调用 onPostLoad 重建运行时关系（如骨架/场景/
         *          预制件的父子层级），其中父子节点以智能指针互相持有形成引用环，
         *          仅由运行时卸载(onUnload)负责断开。像 BundleBuilder 这类只做格式
         *          转换、不经资源管理器加载/卸载生命周期的离线工具，一旦触发这些回调
         *          就会因引用环无法释放而泄漏，且 onPreSave/onPostLoad 对扁平层级表的
         *          重采集还可能破坏字节级往返一致性。这些场景应关闭此开关，做纯数据
         *          直通的读写。默认开启以保持运行时加载行为不变。
         */
        void setInvokeLifecycleCallbacks(bool invoke)
        {
            mInvokeLifecycleCallbacks = invoke;
        }

        /**
         * @brief 序列化对象到数据流对象中
         * @param [in,out] stream   : 数据流对象
         * @param [in] obj          : 要序列化的可反射对象
         * @return 序列化成功返回 T3D_OK .
         * @remarks 序列化的时候把数据和类型一起序列化保存起来，方便反序列化生成对象
         */
        template<typename T>
        TResult serialize(DataStream &stream, const T &obj)
        {
            return serializeObject(stream, obj);
        }

        /**
         * @brief 从数据流中反序列化生成对象. 
         *          该接口借助序列化时候保存了类型，能自动还原所有类型和数据.
         * @param [in,out] stream   : 数据流对象
         * @param [in,out] obj      : 返回的可序列化对象
         * @return 反序列化成功返回 T3D_OK .
         * @remarks 由于序列化的时候保存了类型信息，所以反序列化的时候能把类型
         *          和数据还原。
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

        TResult serializeObject(DataStream &stream, const RTTRObject &obj);

        /**
         * @brief 从数据流反序列化出类型无关的对象。
         * @param [in,out] stream : 数据流对象，按文件头 magic 自动选反序列化器
         * @param [out] obj       : 还原出的对象
         * @return 成功返回 T3D_OK .
         * @remarks 唯一的反序列化出口。**不提供**返回 RTTRObject(=rttr::instance)
         *          的重载：instance 只是非拥有视图，从函数局部 variant 构造再返回
         *          时，其正确性隐式依赖"所有可序列化类型都注册为 as_raw_ptr"
         *          （视图里存的是裸指针而非局部存储地址），换成 as_object 策略即
         *          悬垂。改为出参后 variant 由调用方持有，生命周期显式可控。
         */
        TResult deserializeObject(DataStream &stream, RTTRVariant &obj);
        
    protected:
        SerializerPtr createSerializer();

        /**
         * @brief 按文件头 magic 自动选择反序列化器。
         * @param [in,out] stream : 数据流（会 peek 头部若干字节后回退到原位置）
         * @return 'T3DB' 头返回二进制反序列化器，否则返回 JSON 反序列化器。
         * @remarks 使运行时能透明读取二进制(T3DB) bundle 产物，编辑器 JSON 源
         *          资产照常读取，且兼容同一 bundle 内混合格式，无需调用方预设
         *          FileMode。若流不支持 peek/回退（tell/seek 失败），回退到当前
         *          mFileMode 对应的 mSerializer。
         */
        SerializerPtr pickDeserializer(DataStream &stream);

    protected:
        FileMode        mFileMode = FileMode::kText;
        SerializerPtr   mSerializer;
        /// (反)序列化时是否触发对象生命周期回调，默认开启（见 setter 说明）
        bool            mInvokeLifecycleCallbacks = true;
    };

    #define T3D_SERIALIZER_MGR      (SerializerManager::getInstance())

    T3D_EXTERN_SINGLETON(SerializerManager)
}


#endif    /*__T3D_SERIALIZER_MANAGER_H__*/
