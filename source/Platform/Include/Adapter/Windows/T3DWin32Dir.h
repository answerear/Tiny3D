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

#ifndef __T3D_WIN32_DIR_H__
#define __T3D_WIN32_DIR_H__


#include "Adapter/T3DDirInterface.h"
#include "T3DNoncopyable.h"
#include "Memory/T3DMemory.h"
#include <windows.h>
#include <filesystem>


// #define STD_FILESYSTEM

namespace Tiny3D
{
#if defined (STD_FILESYSTEM)
    class Win32Dir : public IDir, public Noncopyable
    {
    public:
        Win32Dir();
        virtual ~Win32Dir();

    protected:
        /**
         * @brief 开始枚举搜索指定目录下的文件.
         * @param [in] strPath : 指定目录文件格式字符串
         * @return 调用成功返回true，失败返回false.
         */
        virtual bool findFile(const String &strPath) override;

        /**
         * @brief 搜索下一个文件.
         * @return 枚举没结束返回true，否则返回false表示枚举结束.
         */
        virtual bool findNextFile() override;

        /**
         * @brief 结束当前枚举搜索.
         */
        virtual void close() override;

        /**
         * @brief 获取枚举目录的完整路径.
         * @return 返回枚举目录的路径，返回类型std::string.
         */
        virtual String getRoot() const override;

        /**
         * @brief 获取当前枚举到的完整文件名.
         * @return 返回当前枚举到的完整文件名，类型std::string.
         */
        virtual String getFileName() const override;

        /**
         * @brief 获取当前枚举到的文件的全路径名，包括完整文件名.
         * @return 返回当前枚举到的文件的全路径名，类型std::string.
         */
        virtual String getFilePath() const override;

        /**
         * @brief 获取当前枚举到的文件文件名，不包括扩展名.
         * @return 返回当前枚举到的文件文件名，类型std::string.
         */
        virtual String getFileTitle() const override;

        /**
         * @brief 获取当前枚举到的文件大小.
         * @return 返回当前枚举到的文件大小.
         */
        virtual uint32_t getLength() const override;

        /**
         * @brief 判断当前枚举到的文件是否点目录，包括'.'和'..' .
         * @return 点目录返回true，否则返回false.
         */
        virtual bool isDots() const override;

        /**
         * @brief 判断当前枚举到的文件是否文件夹.
         * @return 文件返回true，否则返回false.
         */
        virtual bool isDirectory() const override;

        /**
         * @brief 获取当前枚举到的文件的创建时间戳.
         * @return 返回文件创建时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getCreationTime() const override;

        /**
         * @brief 获取当前枚举到的文件的最后访问时间戳.
         * @return 返回文件最后访问时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getLastAccessTime() const override;

        /**
         * @brief 获取当前枚举到的文件的最后修改时间戳.
         * @return 返回文件最后修改时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getLastWriteTime() const override;

        /**
         * @brief 创建文件夹.
         * @note 该接口不能创建多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool makeDir(const String &strDir) override;

        /**
         * @brief 删除文件夹.
         * @note 该接口不能删除多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool removeDir(const String &strDir) override;

        /**
         * @brief 删除文件.
         * @param [in] strFileName : 需要删除文件的名称.
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool remove(const String &strFileName) override;

        /**
         * @brief 判断路径对应的文件是否存在.
         * @param [in] strPath : 完整路径名
         * @return 文件存在返回true，否则返回false.
         */
        virtual bool exists(const String &strPath) const override;

        /**
         * @brief 获取应用程序缓存数据存储路径，不同平台指定对应的路径
         * @return 返回应用程序缓存数据存储路径.
         */
        virtual String getCachePath() const override;

        /**
        * @brief 获取应用程序路径
        * @return 返回应用程序路径
        */
        virtual String getAppPath() const override;

        /**
         * @brief 获取程序当前工作路径
         * @return 返回程序当前工作路径
         */
        virtual String getCurrentPath() const override;

        /**
         * @brief 获取应用程序可写路径，不同平台指定对应的路径
         * @return 返回应用程序可写路径.
         */
        virtual String getWritablePath() const override;

        /**
         * @brief 获取动态库路径
         * @return 返回动态库路径
         * @remarks 本接口在android上才有效，其他平台跟getAppPath()相同
         */
        virtual String getLibraryPath() const override;

        virtual char getNativeSeparator() const override;

        bool extractRoot(const String &strFilePath, String &strRoot);

        // bool extractFileName(const String &strFilePath, 
        //     String &strName, String &strTitle) const;

    protected:
        std::filesystem::directory_iterator mDirItr;
        std::filesystem::directory_entry mEntry;
        String mRoot;
    };
#else
    class Win32Dir : public IDir, public Noncopyable
    {
    public:
        Win32Dir();
        virtual ~Win32Dir();

    protected:
        /**
         * @brief 开始枚举搜索指定目录下的文件.
         * @param [in] strPath : 指定目录文件格式字符串
         * @return 调用成功返回true，失败返回false.
         */
        virtual bool findFile(const String &strPath) override;

        /**
         * @brief 搜索下一个文件.
         * @return 枚举没结束返回true，否则返回false表示枚举结束.
         */
        virtual bool findNextFile() override;

        /**
         * @brief 结束当前枚举搜索.
         */
        virtual void close() override;

        /**
         * @brief 获取枚举目录的完整路径.
         * @return 返回枚举目录的路径，返回类型std::string.
         */
        virtual String getRoot() const override;

        /**
         * @brief 获取当前枚举到的完整文件名.
         * @return 返回当前枚举到的完整文件名，类型std::string.
         */
        virtual String getFileName() const override;

        /**
         * @brief 获取当前枚举到的文件的全路径名，包括完整文件名.
         * @return 返回当前枚举到的文件的全路径名，类型std::string.
         */
        virtual String getFilePath() const override;

        /**
         * @brief 获取当前枚举到的文件文件名，不包括扩展名.
         * @return 返回当前枚举到的文件文件名，类型std::string.
         */
        virtual String getFileTitle() const override;

        /**
         * @brief 获取当前枚举到的文件大小.
         * @return 返回当前枚举到的文件大小.
         */
        virtual uint32_t getLength() const override;

        /**
         * @brief 判断当前枚举到的文件是否点目录，包括'.'和'..' .
         * @return 点目录返回true，否则返回false.
         */
        virtual bool isDots() const override;

        /**
         * @brief 判断当前枚举到的文件是否文件夹.
         * @return 文件返回true，否则返回false.
         */
        virtual bool isDirectory() const override;

        /**
         * @brief 获取当前枚举到的文件的创建时间戳.
         * @return 返回文件创建时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getCreationTime() const override;

        /**
         * @brief 获取当前枚举到的文件的最后访问时间戳.
         * @return 返回文件最后访问时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getLastAccessTime() const override;

        /**
         * @brief 获取当前枚举到的文件的最后修改时间戳.
         * @return 返回文件最后修改时间戳，该时间是从1970年1月1日到当前的秒数
         */
        virtual long_t getLastWriteTime() const override;

        virtual long_t getCreationTime(const String &filename) const override;

        virtual long_t getLastAccessTime(const String &filename) const override;

        virtual long_t getLastWriteTime(const String &filename) const override;
        
        /**
         * @brief 创建文件夹.
         * @note 该接口不能创建多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool makeDir(const String &strDir) override;

        /**
         * @brief 删除文件夹.
         * @note 该接口不能删除多级文件夹.
         * @param [in] strDir : 文件夹路径名称
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool removeDir(const String &strDir) override;

        /**
         * @brief 删除文件.
         * @param [in] strFileName : 需要删除文件的名称.
         * @return 调用成功返回true，否则返回false.
         */
        virtual bool remove(const String &strFileName) override;

        /**
         * @brief 判断路径对应的文件是否存在.
         * @param [in] strPath : 完整路径名
         * @return 文件存在返回true，否则返回false.
         */
        virtual bool exists(const String &strPath) const override;

        /**
         * @brief 获取应用程序缓存数据存储路径，不同平台指定对应的路径
         * @return 返回应用程序缓存数据存储路径.
         */
        virtual String getCachePath() const override;

        /**
        * @brief 获取应用程序路径
        * @return 返回应用程序路径
        */
        virtual String getAppPath() const override;

        /**
         * @brief 获取程序当前工作路径
         * @return 返回程序当前工作路径
         */
        virtual String getCurrentPath() const override;

        /**
         * @brief 获取应用程序可写路径，不同平台指定对应的路径
         * @return 返回应用程序可写路径.
         */
        virtual String getWritablePath() const override;

        /**
         * @brief 获取动态库路径
         * @return 返回动态库路径
         * @remarks 本接口在android上才有效，其他平台跟getAppPath()相同
         */
        virtual String getLibraryPath() const override;

        virtual char getNativeSeparator() const override;

        bool extractRoot(const String &strFilePath, String &strRoot);

        bool extractFileName(const String &strFilePath, 
            String &strName, String &strTitle) const;

    protected:
        HANDLE              m_hFindFile;
        WIN32_FIND_DATA     m_FindFileData;
     
        mutable bool        m_bExtractName;

        mutable String     m_strRoot;
        mutable String     m_strPath;
        mutable String     m_strName;
        mutable String     m_strTitle;
        mutable String     m_strCachePath;
    };
#endif
}


#endif  /*__T3D_WIN32_DIR_H__*/
