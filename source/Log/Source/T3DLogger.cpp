
#include "T3DLogger.h"


T3D_INIT_SINGLETON(Tiny3D::Logger);


namespace Tiny3D
{
    Logger::Logger()
    {

    }

    Logger::~Logger()
    {

    }

    bool Logger::startup(uint32_t unAppID, const TString &strTag, bool bForceOutput /* = false */)
    {
        return true;
    }

    void Logger::trace(ELevel eLevel, const char *pszFileName, int32_t nLine, const char *pszFmt, ...)
    {

    }

    void Logger::shutdown()
    {

    }

    void Logger::enterBackground()
    {

    }

    void Logger::enterForeground()
    {

    }
}