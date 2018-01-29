/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_LOGGER_H__
#define __T3D_LOGGER_H__


#include "T3DLogPrerequisites.h"


namespace Tiny3D
{
    class LogItem;
    class LogTask;

    class T3D_LOG_API Logger 
        : public Singleton<Logger>
        , public ITimerListener
    {
        T3D_DISABLE_COPY(Logger);

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
            Level       eLevel;         /// ��ǰ��־����ļ��𣬸��ڸü������־�������
            uint32_t    unMaxCacheSize; /// ��־������󻺴��ڴ��С�������ô�С�������ύ�첽�߳�д���ļ�
            uint32_t    unMaxCacheTime; /// ��־���ݻ�����¼���������ʱ�����������ύ�첽�߳�д���ļ�
            uint32_t    unExpired;      /// ����ʱ�䣬�ӽ��쿪ʼ���ݹ�ȥunExpired��
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
         * @brief ������־����
         */
        void setStrategy(const Strategy &strategy);

        /**
         * @brief ���õ�ǰ��־�������߼��𣬸��ڸü������־�����Բ����
         */
        void setLevel(Level eLevel);

        /**
         * @brief ������󻺴��С�����ڸô�С�Ļ�����־�������ύ�첽�߳�д���ļ�
         */
        void setMaxCacheSize(uint32_t unMaxCacheSize);

        /**
         * @brief ������󻺴�ʱ������������ʱ����������־�������ύ�첽�߳�д���ļ�
         */
        void setMaxCacheTime(uint32_t unMaxCacheTime);

        /**
         * @brief ���ù���ʱ�䣬�ӽ��쿪ʼ���ݹ�ȥunExpired��
         */
        void setExpired(uint32_t unExpired);

        /**
         * @brief ������־ģ��
         * @param [in] appID : ��ʶ��ǰӦ�ó����ID���Ա�������־�ļ�
         * @param [in] tag : �����Ӧ�ó����ǩ������־�ļ������Ӷ�����Ϣ��������
         * @param [in] force : �Ƿ������־����ȫ����־���
         * @return ���óɹ�����true
         * @note ���øú���ǰ������ͨ��setStrategy()����setLevel()��setMaxCacheSize()��setMaxCacheTime()���ú���־������ԣ�
         *      �������Ĭ�ϲ���
         */
        bool startup(uint32_t appID, const String &tag, bool force = false, bool outputConsole = false);

        /**
         * @brief �����־
         * @param [in] level : �����־��Ӧ����
         * @param [in] filename : �����־��Դ���ļ�
         * @param [in] line : �����־��ӦԴ���ļ�������
         * @param [in] fmt : ��ʽ���ַ���
         * @param [in] var_list : �ɱ����
         * @return void
         */
        void trace(Level level, const char *filename, int32_t line, const char *fmt, ...);

        /**
         * @brief �ر���־ģ��
         */
        void shutdown();

        /**
         * @brief ��������̨����
         */
        void enterBackground();

        /**
         * @brief ����ص�ǰ̨����
         */
        void enterForeground();

        /**
         * @brief ������־�ı���ȡ��־����ö��ֵ
         */
        Level toLevelValue(const String &level);

        /**
         * @brief ������־����ö��ֵ��ȡ�ı�
         */
        String toLevelString(Level eLevel);

    private:
        /// ��ȡ��־�ļ����·��
        String getLogPath() const;

        /// ������־�ļ���
        String makeLogFileName(uint32_t appID, const String &tag, const DateTime &dt);

        /// ����־�ļ�
        bool openLogFile();
        /// ���ݻ��滺��������д�ļ�
        void writeLogFile(std::vector<LogItem*> &cache);
        /// �ر���־�ļ�
        void closeLogFile();

        /// �����л��涼д���ļ�
        void flushCache();

        /// ����д�ػ���ʱ������ʱ��
        void startFlushTimer();
        /// ֹͣд�ػ���ʱ������ʱ��
        void stopFlushTimer();

        /// ��ʱ���ص����̳���RunLoopObserver
        virtual void onTimer(uint32_t timerID, int32_t dt) override;

        String getFileName(const String &path) const;

        /// �첽�̵߳��õĹ�������
        //static int32_t asyncWorkingProcedure(Logger *pThis);
        void workingProcedure();

        /// �����첽��������첽�̲߳������򴴽�������̱߳���������
        void startAsyncTask();
        /// ֹͣ�첽����
        void stopAsyncTask();
        /// �����첽�����߳�
        void suspendAsyncTask();
        /// �����첽�����߳�
        void wakeAsyncTask();

        /// �ύ��������־�첽����
        void commitCheckExpiredTask();
        /// �ύ�ѻ���д���ļ��첽����
        void commitFlushCacheTask();

        /// �����������־�첽����
        int32_t processCheckExpiredTask(LogTask *task);
        /// ����ѻ���д���ļ��첽����
        int32_t processFlushCacheTask(LogTask *task);

    private:
        typedef std::list<LogItem*>         ItemCache;
        typedef ItemCache::iterator         ItemCacheItr;
        typedef ItemCache::const_iterator   ItemCacheConstItr;

        typedef std::list<LogTask*>         TaskQueue;
        typedef TaskQueue::iterator         TaskQueueItr;
        typedef TaskQueue::const_iterator   TaskQueueConstItr;

        uint32_t            mFlushCacheTimerID; /// д�ض�ʱ��ID

        uint32_t            mAppID;             /// Ӧ�ó����ʶ
        String              mTag;               /// Ӧ�ó��������Ϣ��ǩ

        Strategy            mStrategy;          /// ��־�����ز���

        DateTime            mCurLogFileTime;    /// ��ǰ��־�ļ���ʱ�䣬���ڿ�Сʱ�л���־�ļ�

        ItemCache           mItemCache;         /// ������־��¼������һ����������ʱ��ʱ�ύ�첽д�ش���
        TaskQueue           mTaskQueue;         /// �첽�������

        FileDataStream      mFileStream;        /// �ļ��������

        std::thread         mWorkingThread;     /// �첽�����̣߳��������������־�ļ���д����־�ļ����첽����

        std::mutex              mWaitMutex;     /// ���ڹ����̻߳�����
        std::condition_variable mWaitCond;      /// �첽�߳���������

        std::mutex          mTaskMutex;         /// �첽���񻥳���

        int32_t             mTaskType;          /// ��ǰ��������

        bool                mIsForced;          /// �Ƿ�ǿ�����
        bool                mIsOutputConsole;   /// �Ƿ�ͬ�����������̨
        bool                mIsRunning;         /// ��־ϵͳ�Ƿ�������
        bool                mIsTerminated;      /// �첽�߳��Ƿ���ֹ
        bool                mIsSuspended;       /// �Ƿ񱻹���
    };
}

#define T3D_LOGGER          Tiny3D::Logger::getInstance()


#endif  /*__T3D_LOGGER_H__*/
