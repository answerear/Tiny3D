

#ifndef __T3D_LOG_TASK_H__
#define __T3D_LOG_TASK_H__


#include "T3DLogPrerequisites.h"


namespace Tiny3D
{
    class LogItem;

    class LogTask
    {
    public:
        enum Type
        {
            E_TYPE_NONE = 0,        /// 没有类型
            E_TYPE_CHECK_EXPIRED,   /// 检查过期日志文件
            E_TYPE_FLUSH_CACHE,     /// 把缓存日志写回文件
            E_TYPE_MAX
        };

        T3D_DECLARE_INTERFACE(LogTask);

        virtual Type getType() const = 0;
    };

    class LogTaskCheckExpired : public LogTask
    {
    public:
        LogTaskCheckExpired(uint32_t unExpired);

        virtual Type getType() const override;

        uint32_t getExpired() const
        {
            return mExpired;
        }

    protected:
        uint32_t mExpired;
    };

    class LogTaskFlushCache : public LogTask
    {
    public:
        LogTaskFlushCache(size_t nCacheSize)
            : mItemCache(nCacheSize)
        {

        }

        virtual Type getType() const override;

        typedef std::vector<LogItem *>      ItemCache;
        typedef ItemCache::iterator         ItemCacheItr;
        typedef ItemCache::const_iterator   ItemCacheConstItr;

        ItemCache   mItemCache;
    };
}


#endif  /*__T3D_LOG_TASK_H__*/
