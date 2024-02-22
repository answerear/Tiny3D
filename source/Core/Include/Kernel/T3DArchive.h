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


#ifndef __T3D_ARCHIVE_H__
#define __T3D_ARCHIVE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @class   Archive
     * @brief   档案类，用于档案文件的组织管理，隔离文件系统、zip压缩文件的实现细节
     */
    class T3D_ENGINE_API Archive : public Object, public Noncopyable
    {
    public:
        enum class AccessMode : uint32_t
        {
            /**< None */
            kNone = 0x0,
            /**< 只读 */
            kRead = 0x1,
            /**< 忽略原档案内容，直接覆盖 */
            kTruncate = 0x2,
            /**< 追加到原档案末尾 */
            kAppend = 0x4,
            /**< 读并覆盖写 */
            kReadTruncate = kRead | kTruncate,
            /**< 读并追加写 */
            kReadAppend = kRead | kAppend,
            /**< 文本文件 */
            kText = 0x8,
            /**< 读并覆盖写文本文件 */
            kReadTxtTruncate = kRead | kTruncate | kText,
            /**< 读并追加写文本文件 */
            kReadTxtAppend = kRead | kAppend | kText,
            /**< 只读二进制文件 */
            kReadOnly = kRead,
            /**< 只读文本文件 */
            kReadTxtOnly = kRead | kText,
        };

        /**
         * @fn  virtual Archive::~Archive();
         * @brief   析构函数
         */
        ~Archive() override = default;

        /**
         * @fn  AccessMode getAccessMode() const;
         * @brief   获取访问模式
         * @return  The access mode.
         */
        AccessMode getAccessMode() const { return mAccessMode; }

        /**
         * @fn virtual String getName() const = 0;
         * @brief   获取档案名称
         * @return  返回档案名称
         */
        String getName() const { return mName; }

        /**
         * @fn  virtual String Archive::getArchiveType() const = 0;
         * @brief   获取档案结构类型
         * @return  The archive type.
         */
        virtual String getArchiveType() const = 0;

        /**
         * @fn  virtual String Archive::getLocation() const = 0;
         * @brief   获取档案位置
         * @return  The location.
         */
        virtual String getLocation() const = 0;

        /**
         * @brief   克隆出一个新档案对象
         */
        virtual ArchivePtr clone() const = 0;

        /**
         * @fn  virtual bool Archive::exists(const String &name) const = 0;
         * @brief   指定文件是否在档案结构里
         * @param [in]  name    : 文件名称.
         * @return  存在就返回true，否则返回false。.
         */
        virtual bool exists(const String &name) const = 0;

        /**
         * @fn  virtual TResult Archive::read(const String &name, 
         *      MemoryDataStream &stream) = 0;
         * @brief   从档案读取指定文件到数据流中
         * @param [in]  name    : 文件名称.
         * @param [in]  stream  : 数据流.
         * @return  读成功返回T3D_OK.
         */
        virtual TResult read(const String &name, MemoryDataStream &stream) = 0;

        /**
         * @fn  virtual TResult Archive::write(const String &name, 
         *      const MemoryDataStream &stream) = 0;
         * @brief   写数据流到档案里的指定文件中
         * @param [in]  name    : 文件名称.
         * @param [in]  stream  : 数据流.
         * @return  写成功返回T3D_OK.
         */
        virtual TResult write(const String &name, const MemoryDataStream &stream) = 0;

    protected:
        /**
         * @fn  Archive::Archive(const String &name);
         * @brief   构造函数
         * @param [in]  name    : 资源名称，一般使用档案路径作为名称.
         */
        Archive(const String &name, AccessMode mode);

        String mName; /**< 档案名称 */
        AccessMode mAccessMode; /**< 访问模式 */
    };
}


#endif  /*__T3D_ARCHIVE_H__*/
