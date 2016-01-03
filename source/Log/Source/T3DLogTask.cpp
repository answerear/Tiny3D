

#include "T3DLogTask.h"


namespace Tiny3D
{
    ////////////////////////////////////////////////////////////////////////////
    // LogTaskCheckExpired
    LogTaskCheckExpired::LogTaskCheckExpired(uint32_t unExpired)
        : mExpired(unExpired)
    {

    }

    LogTask::Type LogTaskCheckExpired::getType() const
    {
        return E_TYPE_CHECK_EXPIRED;
    }

    ////////////////////////////////////////////////////////////////////////////
    // LogTaskFlushCache
    LogTask::Type LogTaskFlushCache::getType() const
    {
        return E_TYPE_FLUSH_CACHE;
    }
}
