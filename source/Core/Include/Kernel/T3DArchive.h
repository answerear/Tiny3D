/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_ARCHIVE_H__
#define __T3D_ARCHIVE_H__


#include "T3DPrerequisites.h"


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
            kNone = 0x0,
            /**< None */
            kRead = 0x1,
            /**< 只读 */
            kTruncate = 0x2,
            /**< 忽略原档案内容，直接覆盖 */
            kAppend = 0x4,
            /**< 追加到原档案末尾 */
            kReadTruncate = kRead | kTruncate,
            /**< */
            kReadAppend = kRead | kAppend
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
        virtual TResult read(const String &name, Meta *&meta, MemoryDataStream &stream) = 0;

        /**
         * @fn  virtual TResult Archive::write(const String &name, 
         *      const MemoryDataStream &stream) = 0;
         * @brief   写数据流到档案里的指定文件中
         * @param [in]  name    : 文件名称.
         * @param [in]  stream  : 数据流.
         * @return  写成功返回T3D_OK.
         */
        virtual TResult write(const String &name, Meta *meta, const MemoryDataStream &stream) = 0;

    protected:
        /**
         * @fn  Archive::Archive(const String &name);
         * @brief   构造函数
         * @param [in]  name    : 资源名称，一般使用档案路径作为名称.
         */
        Archive(const String &name);

        String mName; /**< 档案名称 */
        AccessMode mAccessMode; /**< 访问模式 */
    };
}


#endif  /*__T3D_ARCHIVE_H__*/
