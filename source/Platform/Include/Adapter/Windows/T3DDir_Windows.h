

#ifndef __T3D_DIR_WINDOWS_H__
#define __T3D_DIR_WINDOWS_H__


#include "T3DDirInterface.h"
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
        virtual bool findFile(const TString &strPath);
        virtual bool findNextFile();
        virtual void close();

        virtual TString getRoot() const;
        virtual TString getFileName() const;
        virtual TString getFilePath() const;
        virtual TString getFileTitle() const;

        virtual uint32_t getLength() const;

        virtual bool isDots() const;
        virtual bool isDirectory() const;

        virtual long_t getCreationTime() const;
        virtual long_t getLastAccessTime() const;
        virtual long_t getLastWriteTime() const;

        virtual bool makeDir(const TString &strDir);
        virtual bool removeDir(const TString &strDir);

        virtual bool remove(const TString &strFileName);
        virtual bool exists(const TString &strPath) const;

        virtual TString getCachePath() const;
        virtual TString getAppPath() const;
        virtual char getNativeSeparator() const;

        bool extractRoot(const TString &strFilePath, TString &strRoot);
        bool extractFileName(const TString &strFilePath, TString &strName, TString &strTitle) const;

    protected:
        HANDLE              m_hFindFile;
        WIN32_FIND_DATA     m_FindFileData;

        mutable bool        m_bExtractName;

        mutable TString     m_strRoot;
        mutable TString     m_strPath;
        mutable TString     m_strName;
        mutable TString     m_strTitle;
    };
}


#endif  /*__T3D_DIR_WINDOWS_H__*/
