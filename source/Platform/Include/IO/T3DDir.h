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

#ifndef __T3D_DIR_H__
#define __T3D_DIR_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DNoncopyable.h"


namespace Tiny3D
{
    class IDir;

    class T3D_PLATFORM_API Dir : public Noncopyable
    {
    public:
        /**
         * @brief Constructor for T3DDir.
         */
        Dir();

        /**
         * @brief Destructor for T3DDir.
         */
        ~Dir();

        /**
         * @brief 开始枚举搜索指定目录下的文件.
         * @param [in] strPath : 指定目录文件格式字符串
         * @return 调用成功返回true，失败返回false.
         */
        bool findFile(const String &strPath);

        /**
         * @brief 搜索下一个文件.
         * @return 枚举没结束返回true，否则返回false表示枚举结束.
         */
        bool findNextFile();

        /**
         * @brief 结束当前枚举搜索.
         */
        void close();

        /**
         * @brief 获取枚举目录的完整路径.
         * @return 返回枚举目录的路径，返回类型std::string.
         */
        String getRoot() const;

        /**
         * @brief 获取当前枚举到的完整文件名.
         * @return 返回当前枚举到的完整文件名，类型std::string.
         */
        String getFileName() const;

        /**
         * @brief 获取当前枚举到的文件的全路径名，包括完整文件名.
         * @return 返回当前枚举到的文件的全路径名，类型std::string.
         */
        String getFilePath() const;

        /**
         * @brief 获取当前枚举到的文件文件名，不包括扩展名.
         * @return 返回当前枚举到的文件文件名，类型std::string.
         */
        String getFileTitle() const;

        /**
         * @brief 获取当前枚举到的文件大小.
         * @return 返回当前枚举到的文件大小.
         */
        uint32_t getFileSize() const;

        /**
         * @brief 判断当前枚举到的文件是否点目录，包括'.'和'..' .
         * @return 点目录返回true，否则返回false.
         */
        bool isDots() const;

        /**
         * @brief 判断当前枚举到的文件是否文件夹.
         * @return 文件返回true，否则返回false.
         */
        bool isDirectory() const;

        /**
         * @brief 获取当前枚举到的文件的创建时间戳.
         * @return 返回文件创建时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getCreationTime() const;

        /**
         * @brief 获取当前枚举到的文件的最后访问时间戳.
         * @return 返回文件最后访问时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getLastAccessTime() const;

        /**
         * @brief 获取当前枚举到的文件的最后修改时间戳.
         * @return 返回文件最后修改时间戳，该时间是从1970年1月1日到当前的秒数
         */
        long_t getLastWriteTime() const;

        /**
         * @brief 创建文件夹.
         * @note 该接口不能创建多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        static bool makeDir(const String &strDir);

        /**
         * @brief 删除文件夹.
         * @note 该接口不能删除多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @param [in] force : 是否删除非空文件夹
         * @return 调用成功返回true，否则返回false.
         */
        static bool removeDir(const String &strDir, bool force = false);

        /**
         * @brief 删除文件.
         * @param [in] strFileName : 需要删除文件的名称.
         * @return 调用成功返回true，否则返回false.
         */
        static bool remove(const String &strFileName);

        /**
         * @brief 判断路径对应的文件是否存在.
         * @param [in] strPath : 完整路径名
         * @return 文件存在返回true，否则返回false.
         */
        static bool exists(const String &strPath);

        /**
         * @brief 获取应用程序缓存数据存储路径，不同平台指定对应的路径，以对上层透明处理.
         * @return 返回应用程序缓存数据存储路径.
         */
        static const String &getCachePath();

        /**
         * @brief 获取应用程序路径.
         * @return 返回应用程序路径.
         */
        static const String &getAppPath();

        /**
         * @brief 获取程序当前工作路径
         * @return 返回程序当前工作路径
         */
        static String getCurrentPath();

        /**
         * @brief 获取应用程序可写路径.
         * @return 返回应用程序可写路径.
         */
        static const String &getWritablePath();

        /**
         * @brief 获取动态库路径
         * @return 返回动态库路径
         * @remarks 本接口在android上才有效，其他平台跟getAppPath()相同
         */
        static const String &getLibraryPath();

        /**
         * @brief 获取系统相关的路径分隔符
         */
        static char getNativeSeparator();

        /**
         * @brief 从一个文件完整路径里解析出文件夹、文件名、文件扩展名
         * @param [in] path : 带完整路径的文件名
         * @param [out] dir : 文件目录
         * @param [out] title : 不带扩展名的文件名
         * @param [out] ext : 扩展名
         * @return 调用成功返回 true
         */
        static bool parsePath(const String &path, String &dir, String &title, String &ext);

        /**
         * @brief 从一个文件完整路径里解析出文件夹、带扩展名的文件名
         * @param [in] path : 带完整路径的文件名
         * @param [out] dir : 文件目录
         * @param [out] name : 带扩展名的文件名
         * @return 调用成功返回 true
         */
        static bool parsePath(const String &path, String &dir, String &name);

        /**
         * @brief 从一个文件完整路径里解析出带扩展名的文件名
         * @param [in] path : 带完整路径的文件名
         * @param [out] name : 带扩展名的文件名
         * @return 调用成功返回 true
         */
        static bool parsePath(const String &path, String &name);

        /**
         * @brief 格式化路径字符串，把 斜杠和反斜杠统一成系统相关的，并且把其中的 . 和 .. 转换成绝对路径
         * @param [in] path : 需要格式化的路径字符串，格式化完后作为返回值
         * @return 调用成功返回格式化后的路径字符串
         */
        static String formatPath(const String &path);

    protected:
        IDir    *mDir;

        static String  sCachePath;
        static String  sAppPath;
        static String  sWritablePath;
        static String  sLibPath;

        static IDir *sDir;
        static char NATIVE_SEPARATOR;
    };
}


#endif  /*__T3D_DIR_H__*/
