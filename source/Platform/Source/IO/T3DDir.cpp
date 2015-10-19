

#include "IO/T3DDir.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DDirInterface.h"
#include "T3DSystem.h"
#include "T3DType.h"


namespace Tiny3D
{
    DirInterface *Dir::s_pDirAdapter = nullptr;

    char Dir::NATIVE_SEPARATOR = 0;

    Dir::Dir()
    {
        m_pDirAdpater = T3D_ADAPTER_FACTORY.createDirAdapter();
    }

    Dir::~Dir()
    {
        T3D_SAFE_DELETE(m_pDirAdpater);
    }

    bool Dir::findFile(const TString &strPath)
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->findFile(strPath);
        }

        return false;
    }

    bool Dir::findNextFile()
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->findNextFile();
        }

        return false;
    }

    void Dir::close()
    {
        if (m_pDirAdpater != nullptr)
        {
            m_pDirAdpater->close();
        }
    }

    TString Dir::getRoot() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getRoot();
        }

        return "";
    }

    TString Dir::getFileName() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getFileName();
        }

        return "";
    }

    TString Dir::getFilePath() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getFilePath();
        }

        return "";
    }

    TString Dir::getFileTitle() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getFileTitle();
        }

        return "";
    }

    uint32_t Dir::getFileSize() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getLength();
        }

        return 0;
    }

    bool Dir::isDots() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->isDots();
        }

        return false;
    }

    bool Dir::isDirectory() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->isDirectory();
        }

        return false;
    }

    long_t Dir::getCreationTime() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getCreationTime();
        }

        return 0;
    }

    long_t Dir::getLastAccessTime() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getLastAccessTime();
        }

        return 0;
    }

    long_t Dir::getLastWriteTime() const
    {
        if (m_pDirAdpater != nullptr)
        {
            return m_pDirAdpater->getLastWriteTime();
        }

        return 0;
    }

    bool Dir::makeDir(const TString &strDir)
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->makeDir(strDir);
        }

        return false;
    }

    bool Dir::removeDir(const TString &strDir)
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->removeDir(strDir);
        }

        return false;
    }

    bool Dir::remove(const TString &strFileName)
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->remove(strFileName);
        }

        return false;
    }

    bool Dir::exists(const TString &strPath)
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->exists(strPath);
        }

        return false;
    }

    TString Dir::getCachePath()
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->getCachePath();
        }

        return "";
    }

    TString Dir::getAppPath()
    {
        if (nullptr == s_pDirAdapter)
            s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

        if (s_pDirAdapter != nullptr)
        {
            return s_pDirAdapter->getAppPath();
        }

        return "";
    }

    char Dir::getNativeSeparator()
    {
        if (NATIVE_SEPARATOR == 0)
        {
            if (nullptr == s_pDirAdapter)
                s_pDirAdapter = T3D_ADAPTER_FACTORY.createDirAdapter();

            if (s_pDirAdapter != nullptr)
            {
                NATIVE_SEPARATOR = s_pDirAdapter->getNativeSeparator();
            }
        }

        return NATIVE_SEPARATOR;
    }
}
