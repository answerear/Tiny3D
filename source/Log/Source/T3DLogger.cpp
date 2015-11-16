
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

    bool Logger::startup(uint32_t appID, const String &tag, bool truncate /* = true */, bool force /* = false */)
    {
        return true;
    }

    void Logger::trace(ELevel level, const char *filename, int32_t line, const char *fmt, ...)
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