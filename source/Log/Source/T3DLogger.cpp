
#include "T3DLogger.h"
#include "T3DLogItem.h"
#include "T3DLogTask.h"
#include <sstream>
#include <stdarg.h>


namespace Tiny3D
{
    T3D_INIT_SINGLETON(Logger);

    int32_t Logger::asyncWorkingProcedure(Logger *pThis)
    {
        return pThis->workingProcedure();
    }

    Logger::Logger()
    {

    }

    Logger::~Logger()
    {

    }

    void Logger::setLevel(Level eLevel)
    {
        if (eLevel != mStrategy.eLevel)
        {
            if (mIsRunning)
            {
                if (mStrategy.eLevel == E_LEVEL_OFF)
                {
                    mStrategy.eLevel = eLevel;
                    startup(mAppID, mTag, mIsForced);
                }
                else if (eLevel == E_LEVEL_OFF)
                {
                    shutdown();
                    mStrategy.eLevel = eLevel;
                }
                
                mStrategy.eLevel = eLevel;
            }
        }
    }

    void Logger::setMaxCacheSize(uint32_t unMaxCacheSize)
    {
        mStrategy.unMaxCacheSize = unMaxCacheSize;
    }

    void Logger::setExpired(uint32_t unExpired)
    {
        mStrategy.unExpired = unExpired;
    }

    void Logger::setMaxCacheTime(uint32_t unMaxCacheTime)
    {
        if (unMaxCacheTime != mStrategy.unMaxCacheTime)
        {
            mStrategy.unMaxCacheTime = unMaxCacheTime;

            stopFlushTimer();
            startFlushTimer();
        }
    }

    void Logger::setStrategy(const Strategy &strategy)
    {
        setMaxCacheSize(strategy.unMaxCacheSize);
        setMaxCacheTime(strategy.unMaxCacheTime);
        setExpired(strategy.unExpired);
        setLevel(strategy.eLevel);
    }

    bool Logger::startup(uint32_t appID, const String &tag, bool force /* = false */)
    {
        mAppID = appID;
        mTag = tag;
        mIsForced = force;

        if (mStrategy.eLevel != E_LEVEL_OFF)
        {
            /// 不是关闭日志输出，需要先打开日志
            if (openLogFile())
            {
                /// 启动定时器，定时提交写回异步任务
                stopFlushTimer();
                startFlushTimer();
            }
        }

        /// 不管是否要输出日志，都提交一个检查过期日志文件的异步任务
        commitCheckExpiredTask();

        return true;
    }

    void Logger::trace(Level level, const char *filename, int32_t line, const char *fmt, ...)
    {
        if (!mIsForced && level > mStrategy.eLevel)
            return;

        va_list args;
        char content[2048] = {0};

        va_start(args, fmt);
        vsnprintf(content, sizeof(content)-1, fmt, args);
        va_end(args);

        /// 截取路径，直接获取源码文件名
        String name = getFileName(filename);

        /// 生成一条日志项
        LogItem *item = new LogItem(level, filename, line, content);
        
        /// 输出到控制台
        item->outputConsole();

        mItemCache[mCacheIndex].push_back(item);

        if (mItemCache[mCacheIndex].size() >= mStrategy.unMaxCacheSize)
        {
            commitFlushCacheTask();
        }
    }

    void Logger::shutdown()
    {
        /// 停止缓存写回文件间隔定时器
        stopFlushTimer();

        /// 把所有缓存都同步写回文件中
        flushCache();

        /// 关闭文件
        closeLogFile();
    }

    void Logger::enterBackground()
    {

    }

    void Logger::enterForeground()
    {

    }

    String Logger::getLogPath() const
    {
        String cachePath = Dir::getCachePath();
        String path = cachePath + "Log" + Dir::NATIVE_SEPARATOR;
        return path;
    }

    String Logger::makeLogFileName(uint32_t appID, const String &tag, const DateTime &dt)
    {
        String logPath = getLogPath();

        std::stringstream ss;
        ss<<appID<<"_"<<tag<<"_"<<dt.dateToString(DateTime::E_YY_MM_DD)<<"_"<<dt.Hour()<<".log";

        String fullPath = logPath + "/" + ss.str();
        return fullPath;
    }

    bool Logger::openLogFile()
    {
        /// 先创建日志文件夹
        String logPath = getLogPath();
        Dir dir;
        dir.makeDir(logPath);

        DateTime dt = DateTime::currentDateTime();
        String filename = makeLogFileName(mAppID, mTag, dt);
        bool ret = false;
        if (ret = mFileStream.open(filename.c_str(), FileDataStream::E_MODE_APPEND|FileDataStream::E_MODE_TEXT))
        {
            mCurLogFileTime = dt;
        }

        return ret;
    }

    void Logger::writeLogFile(int32_t cacheIndex)
    {

    }

    void Logger::closeLogFile()
    {

    }

    void Logger::startFlushTimer()
    {
        mFlushCacheTimerID = T3D_MAIN_RUNLOOP.start(mStrategy.unMaxCacheTime, true, this);
    }

    void Logger::stopFlushTimer()
    {
        if (mFlushCacheTimerID != T3D_INVALID_LOOP_ID)
        {
            T3D_MAIN_RUNLOOP.stop(mFlushCacheTimerID);
            mFlushCacheTimerID = T3D_INVALID_LOOP_ID;
        }
    }

    void Logger::onExecute(uint32_t unLoopID, uint64_t dt)
    {

    }

    String Logger::getFileName(const String &path) const
    {
        int32_t pos = path.rfind(Dir::NATIVE_SEPARATOR) + 1;
        String name = path.substr(pos, path.length()- pos+1);
        return name;
    }

    void Logger::startAsyncTask()
    {
        if (!mWorkingThread.joinable())
        {
            /// 启动异步线程
            mWorkingThread = std::thread(Logger::asyncWorkingProcedure, this);
            mIsRunning = true;
        }
        else
        {
            // 异步线程已经启动了
            if (mIsSuspended)
            {
                /// 异步线程被挂起，则唤醒
                resumeAsyncProcedure();
            }
        }
    }
}