
#include "T3DLogger.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Logger);

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