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

#include "T3DLogger.h"
#include "T3DLogItem.h"
#include <sstream>
#include <stdarg.h>
#include <functional>
#include <chrono>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    class CheckExpiredFileJob : public IQueuedJob
    {
    public:
        CheckExpiredFileJob() = default;
        ~CheckExpiredFileJob() override = default;
    
        void setLogPath(const String &path)
        {
            mLogPath = path;
        }
    
        void setExpired(uint32_t expired)
        {
            mExpired = expired;
        }
        
    protected:
        void execute() override
        {
            uint64_t currentTime = DateTime::currentSecsSinceEpoch();
    
            Dir dir;
    
            String path = mLogPath + Dir::getNativeSeparator() + "*.log";
    
            bool working = dir.findFile(path);
            while (working)
            {
                String filename = dir.getFileName();
    
                const int32_t oneDay = 24 * 60 * 60;
                const int32_t maxOutdate = mExpired * oneDay;
                time_t lastTime = dir.getLastWriteTime();
                time_t dt = currentTime - lastTime;
                if (dt >= maxOutdate)
                {
                    Dir::remove(dir.getFilePath().c_str());
                }
            
                working = dir.findNextFile();
            }
    
            dir.close();
            delete this;
        }
        
        void abandon() override
        {
            delete this;
        }
    
    protected:
        String      mLogPath;
        uint32_t    mExpired = 0;
    };
    
    //--------------------------------------------------------------------------
    
    class FlushLogCacheJob : public IQueuedJob
    {
    public:
        FlushLogCacheJob(Logger *owner, uint32_t appID, const String &tag, const String &path, FileDataStream &fs)
            : mOwner(owner)
            , mAppID(appID)
            , mTag(tag)
            , mFileStream(fs)
            , mLogPath(path)
        {}
        
        ~FlushLogCacheJob() override
        {
            closeLogFile();
        }
        
        void setCacheItems(LogItem *item)
        {
            mItem = item;
        }
        
    protected:
        void execute() override
        {
            writeLogFile();
        }
    
        void abandon() override
        {
            
        }

        String makeLogFileName(uint32_t appID, const String &tag, const DateTime &dt)
        {
            String logPath = mLogPath;

            std::stringstream ss;
            ss << appID << "_" << tag << "_";
            ss << dt.dateToString(DateTime::DateFormat::YY_MM_DD);
            ss << "_" << dt.Hour() << ".log";

            String fullPath = logPath + Dir::getNativeSeparator() + ss.str();
            return fullPath;
        }

        bool openLogFile()
        {
            /// 先创建日志文件夹
            String logPath = mLogPath;
            Dir::makeDir(Dir::getCachePath());
            Dir::makeDir(logPath);

            DateTime dt = DateTime::currentDateTime();
            String filename = makeLogFileName(mAppID, mTag, dt);
            bool ret = mFileStream.open(filename.c_str(),
                FileDataStream::E_MODE_APPEND | FileDataStream::E_MODE_TEXT);
            if (ret)
            {
                mCurLogFileTime = dt;
            }

            return ret;
        }

        void writeLogFile()
        {
            LogItem *item = mItem;
            while (item != nullptr)
            {
                if (item->getHour() != mCurLogFileTime.Hour())
                {
                    // 跨越到下一个小时，重新写一个新文件
                    closeLogFile();
                    openLogFile();
                }

                item->outputFile(mFileStream);
                LogItem *next = item->Next;
                delete item;
                item = next;
            }
        }

        void closeLogFile()
        {
            if (mFileStream.isOpened())
            {
                mFileStream.flush();
                mFileStream.close();
            }
        }
        
    protected:
        Logger          *mOwner = nullptr;
        uint32_t        mAppID = 0;
        const String    &mTag;
        FileDataStream  &mFileStream;
        LogItem         *mItem = nullptr;
        String          mLogPath;
        DateTime        mCurLogFileTime;
    };
    
    //--------------------------------------------------------------------------

    const char * const Logger::LEVEL_OFF = "OFF";
    const char * const Logger::LEVEL_FATAL = "FATAL";
    const char * const Logger::LEVEL_CRITICAL = "CRITICAL";
    const char * const Logger::LEVEL_ERROR = "ERROR";
    const char * const Logger::LEVEL_WARNING = "WARNING";
    const char * const Logger::LEVEL_INFO = "INFO";
    const char * const Logger::LEVEL_DEBUG = "DEBUG";

    //--------------------------------------------------------------------------

    Logger::Logger()
    {
        mStrategy.eLevel = E_LEVEL_WARNING;
        mStrategy.unExpired = 7;
        mStrategy.unMaxCacheSize = 50;
        mStrategy.unMaxCacheTime = 1000 * 5;
        mQueuedJobPool = new QueuedJobPoolDefault();
    }

    //--------------------------------------------------------------------------

    Logger::~Logger()
    {
        T3D_SAFE_DELETE(mQueuedJobPool);

        while (!mFlushJobPool.empty())
        {
            FlushLogCacheJob *job = mFlushJobPool.front();
            mFlushJobPool.pop_front();
        }
    }

    //--------------------------------------------------------------------------

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
            }

            mStrategy.eLevel = eLevel;
        }
    }

    //--------------------------------------------------------------------------

    void Logger::setMaxCacheSize(uint32_t unMaxCacheSize)
    {
        mStrategy.unMaxCacheSize = unMaxCacheSize;
    }

    //--------------------------------------------------------------------------

    void Logger::setExpired(uint32_t unExpired)
    {
        mStrategy.unExpired = unExpired;
    }

    //--------------------------------------------------------------------------

    void Logger::setMaxCacheTime(uint32_t unMaxCacheTime)
    {
        if (unMaxCacheTime != mStrategy.unMaxCacheTime)
        {
            mStrategy.unMaxCacheTime = unMaxCacheTime;

            stopFlushTimer();
            startFlushTimer();
        }
    }

    //--------------------------------------------------------------------------

    void Logger::setStrategy(const Strategy &strategy)
    {
        setMaxCacheSize(strategy.unMaxCacheSize);
        setMaxCacheTime(strategy.unMaxCacheTime);
        setExpired(strategy.unExpired);
        setLevel(strategy.eLevel);
    }

    //--------------------------------------------------------------------------

    TResult Logger::startup(ID appID, const String &tag,
        bool force /* = false */, bool outputConsole /* = false */)
    {
        mAppID = appID;
        mTag = tag;
        mIsForced = force;
        mIsOutputConsole = outputConsole;

        if (mStrategy.eLevel != E_LEVEL_OFF || force)
        {
            /// 启动定时器，定时提交写回异步任务
            startFlushTimer();
        }

        mQueuedJobPool->create(1);

        mIsRunning = true;
        
        /// 不管是否要输出日志，都提交一个检查过期日志文件的异步任务
        commitCheckExpiredTask();

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Logger::trace(Level level, const char *filename, int32_t line,
        const char *tag, const char *fmt, ...)
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
        LogItem *item = new LogItem(toLevelString(level), name.c_str(), line, tag, content);

        /// 输出到控制台
        if (mIsOutputConsole)
        {
            item->outputConsole();
        }

        ScopeLock lock(&mCSItemCache);
        
        if (mFrontItem == nullptr)
        {
            mFrontItem = item;
        }
        else if (mBackItem != nullptr)
        {
            mBackItem->Next = item;
        }
        else
        {
            T3D_ASSERT(0, "Back item has not be set !");
        }
        
        mBackItem = item;

        if (++mCacheItemCount >= mStrategy.unMaxCacheSize)
        {
            commitFlushCacheTask();
        }
    }

    //--------------------------------------------------------------------------

    void Logger::shutdown()
    {        
        /// 停止缓存写回文件间隔定时器
        stopFlushTimer();

        /// 把所有缓存都同步写回文件中
        commitFlushCacheTask();

        mQueuedJobPool->destroy();
    }

    //--------------------------------------------------------------------------

    void Logger::enterBackground()
    {
        // 强制输出缓存中日志到文件中
        if (mStrategy.eLevel != E_LEVEL_OFF || mIsForced)
        {
            stopFlushTimer();
            commitFlushCacheTask();
        }
    }

    //--------------------------------------------------------------------------

    void Logger::enterForeground()
    {
        // 重启写日志定时器
        if (mStrategy.eLevel != E_LEVEL_OFF || mIsForced)
        {
            startFlushTimer();
        }
    }

    //--------------------------------------------------------------------------

    Logger::Level Logger::toLevelValue(const String &level)
    {
        Level eLevel = E_LEVEL_OFF;

        if (LEVEL_OFF == level)
        {
            eLevel = E_LEVEL_OFF;
        }
        else if (LEVEL_FATAL == level)
        {
            eLevel = E_LEVEL_FATAL;
        }
        else if (LEVEL_CRITICAL == level)
        {
            eLevel = E_LEVEL_CRITICAL;
        }
        else if (LEVEL_ERROR == level)
        {
            eLevel = E_LEVEL_ERROR;
        }
        else if (LEVEL_WARNING == level)
        {
            eLevel = E_LEVEL_WARNING;
        }
        else if (LEVEL_INFO == level)
        {
            eLevel = E_LEVEL_INFO;
        }
        else if (LEVEL_DEBUG == level)
        {
            eLevel = E_LEVEL_DEBUG;
        }

        return eLevel;
    }

    //--------------------------------------------------------------------------

    String Logger::toLevelString(Level eLevel)
    {
        String level[E_LEVEL_MAX] =
        {
            LEVEL_OFF, LEVEL_FATAL, LEVEL_CRITICAL, LEVEL_ERROR,
            LEVEL_WARNING, LEVEL_INFO, LEVEL_DEBUG
        };

        return level[eLevel];
    }

    //--------------------------------------------------------------------------

    String Logger::getLogPath() const
    {
        String cachePath = Dir::getCachePath();
        String path = cachePath + Dir::getNativeSeparator() + "Log";
        return path;
    }

    //--------------------------------------------------------------------------

    void Logger::startFlushTimer()
    {
        mFlushCacheTimerID = T3D_TIMER_MGR.startTimer(
            mStrategy.unMaxCacheTime,
            true,
            [this](ID timerID, uint32_t dt)
            {
                onTimer(timerID, dt);
            });
    }

    //--------------------------------------------------------------------------

    void Logger::stopFlushTimer()
    {
        if (mFlushCacheTimerID != T3D_INVALID_TIMER_ID)
        {
            T3D_TIMER_MGR.stopTimer(mFlushCacheTimerID);
            mFlushCacheTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    //--------------------------------------------------------------------------

    void Logger::onTimer(ID timerID, uint32_t dt)
    {
        if (timerID == mFlushCacheTimerID)
        {
            commitFlushCacheTask();
        }
    }

    //--------------------------------------------------------------------------

    String Logger::getFileName(const String &path) const
    {
        String name;
        size_t pos = path.rfind("\\");
        if (pos == String::npos)
        {
            pos = path.rfind("/");
        }
        if (pos != String::npos)
        {
            name = path.substr(pos+1);
        }

        return name;
    }

    //--------------------------------------------------------------------------

    void Logger::commitCheckExpiredTask()
    {
        mQueuedJobPool->addQueuedJob(new CheckExpiredFileJob());
    }

    //--------------------------------------------------------------------------

    void Logger::commitFlushCacheTask()
    {
        FlushLogCacheJob *job = acquireFlushJob();
        job->setCacheItems(mFrontItem);
        mFrontItem = mBackItem = nullptr;
        mQueuedJobPool->addQueuedJob(job);
        mCacheItemCount = 0;
    }

    //--------------------------------------------------------------------------

    FlushLogCacheJob *Logger::acquireFlushJob()
    {
        FlushLogCacheJob *job = nullptr;
        ScopeLock lock(&mCSFlushJobPool);
        if (mFlushJobPool.empty())
        {
            job = new FlushLogCacheJob(this, mAppID, mTag, getLogPath(), mFileStream);
        }
        else
        {
            job = mFlushJobPool.back();
            mFlushJobPool.pop_back();
        }
        return job;
    }

    //--------------------------------------------------------------------------

    void Logger::releaseFlushJob(FlushLogCacheJob *job)
    {
        ScopeLock lock(&mCSFlushJobPool);
        mFlushJobPool.push_back(job);
    }

    //--------------------------------------------------------------------------
}
