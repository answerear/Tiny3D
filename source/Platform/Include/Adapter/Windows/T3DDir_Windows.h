/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_DIR_WINDOWS_H__
#define __T3D_DIR_WINDOWS_H__


#include "Adapter/T3DDirInterface.h"
#include <windows.h>


namespace Tiny3D
{
    class Dir_Windows : public DirInterface
    {
        T3D_DISABLE_COPY(Dir_Windows);

    public:
        Dir_Windows();
        virtual ~Dir_Windows();

    protected:
        virtual bool findFile(const String &strPath);
        virtual bool findNextFile();
        virtual void close();

        virtual String getRoot() const;
        virtual String getFileName() const;
        virtual String getFilePath() const;
        virtual String getFileTitle() const;

        virtual uint32_t getLength() const;

        virtual bool isDots() const;
        virtual bool isDirectory() const;

        virtual long_t getCreationTime() const;
        virtual long_t getLastAccessTime() const;
        virtual long_t getLastWriteTime() const;

        virtual bool makeDir(const String &strDir);
        virtual bool removeDir(const String &strDir);

        virtual bool remove(const String &strFileName);
        virtual bool exists(const String &strPath) const;

        virtual String getCachePath() const;
        virtual String getAppPath() const;
        virtual char getNativeSeparator() const;

        bool extractRoot(const String &strFilePath, String &strRoot);
        bool extractFileName(const String &strFilePath, String &strName, String &strTitle) const;

    protected:
        HANDLE              m_hFindFile;
        WIN32_FIND_DATA     m_FindFileData;

        mutable bool        m_bExtractName;

        mutable String     m_strRoot;
        mutable String     m_strPath;
        mutable String     m_strName;
        mutable String     m_strTitle;
    };
}


#endif  /*__T3D_DIR_WINDOWS_H__*/
