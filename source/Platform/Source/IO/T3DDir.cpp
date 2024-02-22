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

#include "IO/T3DDir.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DDirInterface.h"
#include "T3DPlatform.h"
#include "T3DType.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    IDir *Dir::sDir = nullptr;

    String Dir::sCachePath;
    String Dir::sAppPath;
    String Dir::sWritablePath;
    String Dir::sLibPath;

    char Dir::NATIVE_SEPARATOR = 0;

    //--------------------------------------------------------------------------

    Dir::Dir()
    {
        mDir = T3D_PLATFORM_FACTORY.createPlatformDir();
    }

    //--------------------------------------------------------------------------

    Dir::~Dir()
    {
        T3D_SAFE_DELETE(mDir);
    }

    //--------------------------------------------------------------------------

    bool Dir::findFile(const String &strPath)
    {
        if (mDir != nullptr)
        {
            return mDir->findFile(strPath);
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Dir::findNextFile()
    {
        if (mDir != nullptr)
        {
            return mDir->findNextFile();
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void Dir::close()
    {
        if (mDir != nullptr)
        {
            mDir->close();
        }
    }

    //--------------------------------------------------------------------------

    String Dir::getRoot() const
    {
        if (mDir != nullptr)
        {
            return mDir->getRoot();
        }

        return "";
    }

    //--------------------------------------------------------------------------

    String Dir::getFileName() const
    {
        if (mDir != nullptr)
        {
            return mDir->getFileName();
        }

        return "";
    }

    //--------------------------------------------------------------------------

    String Dir::getFilePath() const
    {
        if (mDir != nullptr)
        {
            return mDir->getFilePath();
        }

        return "";
    }

    //--------------------------------------------------------------------------

    String Dir::getFileTitle() const
    {
        if (mDir != nullptr)
        {
            return mDir->getFileTitle();
        }

        return "";
    }

    //--------------------------------------------------------------------------

    uint32_t Dir::getFileSize() const
    {
        if (mDir != nullptr)
        {
            return mDir->getLength();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    bool Dir::isDots() const
    {
        if (mDir != nullptr)
        {
            return mDir->isDots();
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Dir::isDirectory() const
    {
        if (mDir != nullptr)
        {
            return mDir->isDirectory();
        }

        return false;
    }

    //--------------------------------------------------------------------------

    long_t Dir::getCreationTime() const
    {
        if (mDir != nullptr)
        {
            return mDir->getCreationTime();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    long_t Dir::getLastAccessTime() const
    {
        if (mDir != nullptr)
        {
            return mDir->getLastAccessTime();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    long_t Dir::getLastWriteTime() const
    {
        if (mDir != nullptr)
        {
            return mDir->getLastWriteTime();
        }

        return 0;
    }

    //--------------------------------------------------------------------------

    long_t Dir::getCreationTime(const String &filename)
    {
        if (nullptr == sDir)
        {
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();
        }

        if (sDir != nullptr)
        {
            return sDir->getCreationTime(filename);
        }

        return 0;
    }

    //--------------------------------------------------------------------------
    
    long_t Dir::getLastAccessTime(const String &filename)
    {
        if (nullptr == sDir)
        {
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();
        }

        if (sDir != nullptr)
        {
            return sDir->getLastAccessTime(filename);
        }

        return 0;
    }
    
    //--------------------------------------------------------------------------

    long_t Dir::getLastWriteTime(const String &filename)
    {
        if (nullptr == sDir)
        {
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();
        }

        if (sDir != nullptr)
        {
            return sDir->getLastWriteTime(filename);
        }

        return 0;
    }
    
    //--------------------------------------------------------------------------

    bool Dir::makeDir(const String &strDir)
    {
        if (nullptr == sDir)
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

        if (sDir != nullptr)
        {
            return sDir->makeDir(strDir);
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Dir::removeDir(const String &strDir, bool force/* = false */)
    {
        if (nullptr == sDir)
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

        bool ret = false;
        
        if (sDir != nullptr)
        {
            if (force)
            {
                Dir dir;
                bool working = dir.findFile(strDir + getNativeSeparator() + "*.*");

                while (working)
                {
                    working = dir.findNextFile();

                    if (dir.isDots())
                    {
                        // . or ..
                        continue;
                    }
                    
                    if (dir.isDirectory())
                    {
                        // directory
                        const String filePath = dir.getFilePath();
                        removeDir(filePath, true);
                    }
                    else
                    {
                        // file
                        const String filePath = dir.getFilePath();
                        remove(filePath);
                    }
                }
                
                sDir->removeDir(strDir);

                dir.close();
            }
            else
            {
                ret = sDir->removeDir(strDir);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool Dir::remove(const String &strFileName)
    {
        if (nullptr == sDir)
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

        if (sDir != nullptr)
        {
            return sDir->remove(strFileName);
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool Dir::exists(const String &strPath)
    {
        if (nullptr == sDir)
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

        if (sDir != nullptr)
        {
            return sDir->exists(strPath);
        }

        return false;
    }

    //--------------------------------------------------------------------------

    const String &Dir::getCachePath()
    {
        if (sCachePath.empty())
        {
            if (nullptr == sDir)
                sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

            if (sDir != nullptr)
            {
                sCachePath = sDir->getCachePath();
            }
        }

        return sCachePath;
    }

    //--------------------------------------------------------------------------

    const String &Dir::getAppPath()
    {
        if (sAppPath.empty())
        {
            if (nullptr == sDir)
                sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

            if (sDir != nullptr)
            {
                sAppPath = sDir->getAppPath();
            }
        }

        return sAppPath;
    }

    //--------------------------------------------------------------------------

    String Dir::getCurrentPath()
    {
        String path;
        
        if (nullptr == sDir)
            sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

        if (sDir != nullptr)
        {
            path = sDir->getCurrentPath();
        }
        
        return path;
    }

    //--------------------------------------------------------------------------

    const String &Dir::getWritablePath()
    {
        if (sWritablePath.empty())
        {
            if (nullptr == sDir)
                sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

            if (sDir != nullptr)
            {
                sWritablePath = sDir->getWritablePath();
            }
        }

        return sWritablePath;
    }

    //--------------------------------------------------------------------------

    const String &Dir::getLibraryPath()
    {
        if (sLibPath.empty())
        {
            if (nullptr == sDir)
                sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

            if (sDir != nullptr)
            {
                sLibPath = sDir->getLibraryPath();
            }
        }

        return sLibPath;
    }

    //--------------------------------------------------------------------------

    char Dir::getNativeSeparator()
    {
        if (NATIVE_SEPARATOR == 0)
        {
            if (nullptr == sDir)
                sDir = T3D_PLATFORM_FACTORY.createPlatformDir();

            if (sDir != nullptr)
            {
                NATIVE_SEPARATOR = sDir->getNativeSeparator();
            }
        }

        return NATIVE_SEPARATOR;
    }

    //--------------------------------------------------------------------------

    bool Dir::parsePath(const String& path, String& dir, String& title, String& ext)
    {
        String name;

        bool ret = parsePath(path, dir, name);
        if (ret)
        {
            String::size_type pos = name.find_last_of('.');

            if (pos == String::npos)
            {
                title = name;
                ext = "";
            }
            else
            {
                title = name.substr(0, pos);
                ext = name.substr(pos + 1);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool Dir::parsePath(const String& path, String& dir, String& name)
    {
        String::size_type pos = path.find_last_of('/');

        if (pos == String::npos)
        {
            pos = path.find_last_of('\\');
        }

        if (pos == String::npos)
        {
            name = path;
            dir = "";
        }
        else
        {
            dir = path.substr(0, pos);
            name = path.substr(pos + 1);
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool Dir::parsePath(const String& path, String& name)
    {
        String dir;
        return parsePath(path, dir, name);
    }

    //--------------------------------------------------------------------------

    String Dir::formatPath(const String& path)
    {
        String fullpath;
        
        if (path[0] == '.')
        {
            // 相对路径
            fullpath = Dir::getCurrentPath() + Dir::getNativeSeparator() + path;
        }
        else
        {
            fullpath = path;
        }
        
        TList<String> strings;
        String s;
        s.reserve(fullpath.size());
        size_t jumpCount = 0;
        int32_t i = (int32_t)fullpath.length() - 1;
        
        while (i >= 0)
        {
            if (fullpath[i] == '\\' || fullpath[i] == '/')
            {
                if (!s.empty() && jumpCount == 0)
                {
                    strings.push_front(s);
                    s.clear();
                }
                else if (jumpCount > 0)
                {
                    // 退掉一级目录
                    jumpCount--;
                }
            }
            else
            {
                if (fullpath[i] == '.' && fullpath[i-1] == '.')
                {
                    // 要退到上一级目录，也就是删掉上一级目录
                    jumpCount++;
                    i-=2;
                }
                else if (fullpath[i] == '.' && fullpath[i-1] != '.')
                {
                    // 当前目录，只需要删掉 . 即可
                }
                else if (jumpCount == 0)
                {
                    // 没有跳到上一级目录，存起来
                    s.insert(s.begin(), fullpath[i]);
                }
            }
            
            i--;
        }

        if (!s.empty())
        {
            strings.push_front(s);
        }

        String ret;
        ret.reserve(fullpath.size());
        
        // 重新用系统路径分隔符组合路径
        i = 0;
        for (const auto &str : strings)
        {
            ret += str;
            if (i != strings.size() - 1)
                ret += getNativeSeparator();
            i++;
        }        
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
