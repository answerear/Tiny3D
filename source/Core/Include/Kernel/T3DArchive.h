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
    /// 档案系统读回调
    using ArchiveReadCallback = TFunction<TResult(DataStream &)>;

    // 档案系统写回调
    using ArchiveWriteCallback = TFunction<TResult(DataStream &)>;
    
    /**
     * @brief 档案类，用于档案文件的组织管理，用于隔离文件系统、zip压缩文件的实现细节
     */
    class T3D_ENGINE_API Archive : public Object, public Noncopyable
    {
    public:
        /**
         * 访问档案的模式
         */
        enum class AccessMode : uint32_t
        {
            /// None
            kNone = 0x0,
            /// 只读
            kRead = 0x1,
            /// 忽略原档案内容，直接覆盖
            kTruncate = 0x2,
            /// 追加到原档案末尾
            kAppend = 0x4,
            /// 读并覆盖写
            kReadTruncate = kRead | kTruncate,
            /// 读并追加写
            kReadAppend = kRead | kAppend,
            /// 文本文件
            kText = 0x8,
            /// 读并覆盖写文本文件
            kReadTxtTruncate = kRead | kTruncate | kText,
            /// 读并追加写文本文件
            kReadTxtAppend = kRead | kAppend | kText,
            /// 只读二进制文件
            kReadOnly = kRead,
            /// 只读文本文件
            kReadTxtOnly = kRead | kText,
        };

        /**
         * 析构函数
         */
        ~Archive() override = default;

        /**
         * @brief 获取访问模式
         * @return 返回档案访问模式.
         */
        AccessMode getAccessMode() const { return mAccessMode; }

        /**
         * @brief 获取档案名称
         * @return 返回档案名称
         */
        String getName() const { return mName; }

        /**
         * @brief 获取档案结构类型字符串
         * @return 返回档案结构类型字符串
         */
        virtual String getArchiveType() const = 0;

        /**
         * @brief 获取档案位置
         * @return 返回档案路径
         */
        virtual String getPath() const = 0;

        /**
         * @brief 克隆出一个新档案对象
         */
        virtual ArchivePtr clone() const = 0;

        /**
         * @brief 查找指定文件是否在档案结构里
         * @param [in] name : 文件名称.
         * @return 存在就返回true，否则返回false
         */
        virtual bool exists(const String &name) const = 0;

        /**
         * @brief 从档案读取指定文件到数据流中
         * @param [in] name : 文件名称.
         * @param [in] callback : 读回调.
         * @return 读成功返回T3D_OK.
         */
        virtual TResult read(const String &name, const ArchiveReadCallback &callback) = 0;

        /**
         * @brief 写数据流到档案里的指定文件中
         * @param [in] name : 文件名称.
         * @param [in] callback : 写回调.
         * @return 写成功返回T3D_OK.
         */
        virtual TResult write(const String &name, const ArchiveWriteCallback &callback) = 0;

        /**
         * @brief 从档案读取指定UUID的文件到数据流中
         * @param [in] uuid : 文件的 UUID
         * @param [in] callback : 读回调 
         * @return 读成功返回 T3D_OK
         */
        virtual TResult read(const UUID &uuid, const ArchiveReadCallback &callback) = 0;

        /**
         * @brief 写数据流到档案里的指定UUID文件中
         * @param [in] uuid : 文件的 UUID
         * @param [in] callback : 写回调
         * @return 写成功返回 T3D_OK
         */
        virtual TResult write(const UUID &uuid, const ArchiveWriteCallback &callback) = 0;

    protected:
        /**
         * @brief 构造函数
         * @param [in] name : 资源名称，一般使用档案路径作为名称.
         * @param [in] mode : 访问模式.
         */
        Archive(const String &name, AccessMode mode);

    protected:
        /// 档案名称
        String mName {};
        /// 访问模式
        AccessMode mAccessMode {AccessMode::kNone};
    };
}


#endif  /*__T3D_ARCHIVE_NEW_H__*/
