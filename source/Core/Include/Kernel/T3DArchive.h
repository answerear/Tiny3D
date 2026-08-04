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


#ifndef __T3D_ARCHIVE_NEW_H__
#define __T3D_ARCHIVE_NEW_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 档案读回调：打开目标文件后由派生类传入 DataStream 供读取
     * \param DataStream & : 已定位到目标文件的数据流
     * \param const String & : 目标文件名 / 路径
     * \param void * : 调用方透传的用户数据
     * \return 回调处理成功应返回 T3D_OK
     */
    using ArchiveReadCallback = TFunction<TResult(DataStream &, const String &, void *)>;

    /**
     * \brief 档案写回调：打开目标文件后由派生类传入 DataStream 供写入
     * \param DataStream & : 已定位到目标文件的数据流
     * \param const String & : 目标文件名 / 路径
     * \param void * : 调用方透传的用户数据
     * \return 回调处理成功应返回 T3D_OK
     */
    using ArchiveWriteCallback = TFunction<TResult(DataStream &, const String &, void *)>;

    /**
     * \brief 档案抽象基类，隔离文件系统、Zip 等具体存储实现
     * \remarks 派生类通过 ArchiveManager 注册的创建器构造；基类仅保存名称与访问模式。
     */
    class T3D_ENGINE_API Archive : public Object, public Noncopyable
    {
    public:
        /**
         * \brief 档案访问模式（位标志，可组合）
         */
        enum class AccessMode : uint32_t
        {
            kNone = 0x0,                            ///< 未指定
            kRead = 0x1,                            ///< 可读
            kTruncate = 0x2,                        ///< 写时截断覆盖
            kAppend = 0x4,                          ///< 写时追加
            kReadTruncate = kRead | kTruncate,      ///< 读 + 截断写
            kReadAppend = kRead | kAppend,          ///< 读 + 追加写
            kText = 0x8,                            ///< 文本模式
            kReadTxtTruncate = kRead | kTruncate | kText,   ///< 读 + 截断写文本
            kReadTxtAppend = kRead | kAppend | kText,       ///< 读 + 追加写文本
            kReadOnly = kRead,                      ///< 只读二进制
            kReadTxtOnly = kRead | kText,           ///< 只读文本
        };

        ~Archive() override = default;

        /// 返回构造时指定的访问模式
        AccessMode getAccessMode() const { return mAccessMode; }

        /// 返回档案名称（通常为路径）
        String getName() const { return mName; }

        /**
         * \brief 返回档案类型字符串（如 "FileSystem"、"Zip"、"Composite"）
         * \return 类型名，供 ArchiveManager 作缓存 key 的一部分
         */
        virtual String getArchiveType() const = 0;

        /**
         * \brief 返回档案物理路径或等价定位信息
         * \return 路径字符串，具体含义由派生类定义
         */
        virtual String getPath() const = 0;

        /**
         * \brief 克隆出同类型、同配置的新档案对象
         * \return 新档案智能指针
         */
        virtual ArchivePtr clone() const = 0;

        /**
         * \brief 按文件名检查条目是否存在
         * \param [in] name : 文件名称 / 相对路径
         * \return 存在返回 true，否则返回 false
         */
        virtual bool exists(const String &name) const = 0;

        /**
         * \brief 按文件名打开条目并交给读回调处理
         * \param [in] name : 文件名称 / 相对路径
         * \param [in] callback : 读回调
         * \param [in] userData : 透传给回调的用户数据，可为 nullptr
         * \return 调用成功返回 T3D_OK；失败含义由派生类定义
         */
        virtual TResult read(const String &name, const ArchiveReadCallback &callback, void *userData) = 0;

        /**
         * \brief 按文件名打开条目并交给写回调处理
         * \param [in] name : 文件名称 / 相对路径
         * \param [in] callback : 写回调
         * \param [in] userData : 透传给回调的用户数据，可为 nullptr
         * \return 调用成功返回 T3D_OK；失败含义由派生类定义
         */
        virtual TResult write(const String &name, const ArchiveWriteCallback &callback, void *userData) = 0;

        /**
         * \brief 按 UUID 打开条目并交给读回调处理
         * \param [in] uuid : 资源 UUID
         * \param [in] callback : 读回调
         * \param [in] userData : 透传给回调的用户数据，可为 nullptr
         * \return 调用成功返回 T3D_OK；失败含义由派生类定义
         */
        virtual TResult read(const UUID &uuid, const ArchiveReadCallback &callback, void *userData) = 0;

        /**
         * \brief 按 UUID 打开条目并交给写回调处理
         * \param [in] uuid : 资源 UUID
         * \param [in] callback : 写回调
         * \param [in] userData : 透传给回调的用户数据，可为 nullptr
         * \return 调用成功返回 T3D_OK；失败含义由派生类定义
         */
        virtual TResult write(const UUID &uuid, const ArchiveWriteCallback &callback, void *userData) = 0;

    protected:
        /**
         * \brief 构造档案，仅保存名称与访问模式
         * \param [in] name : 档案名称，一般使用路径
         * \param [in] mode : 访问模式
         */
        Archive(const String &name, AccessMode mode);

    protected:
        String mName {};                                    ///< 档案名称
        AccessMode mAccessMode {AccessMode::kNone};         ///< 访问模式
    };
}


#endif  /*__T3D_ARCHIVE_NEW_H__*/
