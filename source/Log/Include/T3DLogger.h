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

#ifndef __T3D_LOGGER_H__
#define __T3D_LOGGER_H__


#include "T3DLogPrerequisites.h"


namespace Tiny3D
{
    class LogItem;
    class FlushLogCacheJob;
    
    class T3D_LOG_API Logger : public Singleton<Logger>
    {
        friend class FlushLogCacheJob;
        
    public:
        enum Level
        {
            E_LEVEL_OFF = 0,        /// None log
            E_LEVEL_FATAL,          /// Fatal error
            E_LEVEL_CRITICAL,       /// Critical error
            E_LEVEL_ERROR,          /// Normal error
            E_LEVEL_WARNING,        /// Warning
            E_LEVEL_INFO,           /// Information
            E_LEVEL_DEBUG,          /// Debug
            E_LEVEL_MAX,            /// Max number of log level
        };

        static const char * const LEVEL_OFF;
        static const char * const LEVEL_FATAL;
        static const char * const LEVEL_CRITICAL;
        static const char * const LEVEL_ERROR;
        static const char * const LEVEL_WARNING;
        static const char * const LEVEL_INFO;
        static const char * const LEVEL_DEBUG;

        struct Strategy
        {
            Level       eLevel;         /// 当前日志输出的级别，高于该级别的日志都不输出
            uint32_t    unMaxCacheSize; /// 日志内容最大缓存内存大小，超过该大小，马上提交异步线程写回文件
            uint32_t    unMaxCacheTime; /// 日志内容缓存最长事件，超过该时间间隔，马上提交异步线程写回文件
            uint32_t    unExpired;      /// 过期时间，从今天开始回溯过去unExpired天
        };

    public:
        /**
         * @brief Constructor
         */
        Logger();

        /**
         * @brief Destructor
         */
        virtual ~Logger();

        /**
         * @brief 设置日志策略
         */
        void setStrategy(const Strategy &strategy);

        /**
         * @brief 设置当前日志输出的最高级别
         * @remarks 高于该级别的日志被忽略不输出
         */
        void setLevel(Level eLevel);

        /**
         * @brief 设置最大缓存大小
         * @remarks 大于该大小的缓存日志会马上提交异步线程写回文件
         */
        void setMaxCacheSize(uint32_t unMaxCacheSize);

        /**
         * @brief 设置最大缓存时间间隔
         * @remarks 超过该时间间隔缓存日志会马上提交异步线程写回文件
         */
        void setMaxCacheTime(uint32_t unMaxCacheTime);

        /**
         * @brief 设置过期时间，从今天开始回溯过去unExpired天
         */
        void setExpired(uint32_t unExpired);

        /**
         * @brief 启动日志模块
         * @param [in] appID : 标识当前应用程序的ID，以便区分日志文件
         * @param [in] tag : 额外的应用程序标签，给日志文件名附加额外信息以作区别
         * @param [in] force : 是否忽略日志级别，全量日志输出
         * @return 调用成功返回T3D_OK
         * @note 调用该函数前，请先通过setStrategy()或者setLevel()、
         *      setMaxCacheSize()、setMaxCacheTime()设置好日志输出策略，
         *      否则会用默认策略
         */
        TResult startup(ID appID, const String &tag, bool force = false, 
            bool outputConsole = false);

        /**
         * @brief 输出日志
         * @param [in] level : 输出日志相应级别
         * @param [in] filename : 输出日志的源码文件
         * @param [in] line : 输出日志对应源码文件的行数
         * @param [in] tag : 打个特殊标签，用于区分不同日志
         * @param [in] fmt : 格式化字符串
         * @param [in] var_list : 可变参数
         * @return void
         */
        void trace(Level level, const char *filename, int32_t line, 
            const char *tag, const char *fmt, ...);

        /**
         * @brief 关闭日志模块
         */
        void shutdown();

        /**
         * @brief 程序进入后台调用
         */
        void enterBackground();

        /**
         * @brief 程序回到前台调用
         */
        void enterForeground();

        /**
         * @brief 根据日志文本获取日志级别枚举值
         */
        Level toLevelValue(const String &level);

        /**
         * @brief 根据日志级别枚举值获取文本
         */
        String toLevelString(Level eLevel);

    private:
        /// 获取日志文件存放路径
        String getLogPath() const;

        FlushLogCacheJob *acquireFlushJob();

        void releaseFlushJob(FlushLogCacheJob *job);

        /// 启动写回缓存时间间隔定时器
        void startFlushTimer();
        /// 停止写回缓存时间间隔定时器
        void stopFlushTimer();

        /// 定时器回调，继承自RunLoopObserver
        void onTimer(ID timerID, uint32_t dt);

        String getFileName(const String &path) const;

        /// 提交检查过期日志异步任务
        void commitCheckExpiredTask();
        /// 提交把缓存写回文件异步任务
        void commitFlushCacheTask();

    private:
        using FlushJobsObjectPool = TList<FlushLogCacheJob*>;

        CriticalSection     mCSItemCache {};
        CriticalSection     mCSFlushJobPool {};
        FlushJobsObjectPool mFlushJobPool {};
        
        QueuedJobPool       *mQueuedJobPool {nullptr};
        
        LogItem             *mFrontItem {nullptr};  /// 当前输出的日志项队列头 
        LogItem             *mBackItem {nullptr};   /// 当前输出的日志队列尾，方便新生成的日志项插入到末尾

        uint32_t            mCacheItemCount {0};    /// 当前缓存日志条数
        ID                  mFlushCacheTimerID {T3D_INVALID_TIMER_ID}; /// 写回定时器ID

        ID                  mAppID {0};         /// 应用程序标识
        String              mTag {"tag"};       /// 应用程序额外信息标签

        Strategy            mStrategy {};          /// 日志输出相关策略

        FileDataStream      mFileStream {};        /// 文件输出对象

        bool                mIsForced {false};          /// 是否强制输出
        bool                mIsOutputConsole {true};   /// 是否同步输出到控制台
        bool                mIsRunning {false};
    };
}

#define T3D_LOGGER          Tiny3D::Logger::getInstance()


#endif  /*__T3D_LOGGER_H__*/
