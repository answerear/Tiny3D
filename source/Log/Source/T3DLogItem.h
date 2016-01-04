
#ifndef __T3D_LOG_ITEM_H__
#define __T3D_LOG_ITEM_H__


#include "T3DLogPrerequisites.h"
#include "T3DLogger.h"
#include <sstream>


namespace Tiny3D
{
    class LogItem
    {
        friend class Logger;

    public:
        LogItem(Logger::Level level, const char *filename, int32_t line, const char *content)
        {
            DateTime dt = DateTime::currentDateTime();
            mHour = dt.Hour();
            formatContent(dt, filename, line, level, content);
        }

        void outputFile(FileDataStream &fs) const
        {
            // 日志格式
            // YYYY-MM-DD HH:MM:SS.XXX|Level|ThreadID|FineName|Line|Content
            fs.write((void *)mContent, mContentSize);
        }

        void outputConsole() const
        {
            T3D_CONSOLE.print(mContent);
        }

        uint32_t getContentSize() const    { return mContentSize; }

        int32_t getHour() const    { return mHour; }

    protected:
        uint32_t formatContent(const DateTime &dt, const char *filename, int32_t line, Logger::Level level, const char *content)
        {
            std::thread::id threadID = std::this_thread::get_id();
            String strDateTime = dt.toString();
            std::stringstream ss;
            ss<<strDateTime<<"|"<<level<<"|"<<threadID<<"|"<<filename<<"|"<<line<<"|"<<content<<"\n";
            mContentSize = ss.str().length();
            mContentSize = (mContentSize > sizeof(mContent) - 1 ? sizeof(mContent) - 1 : ss.str().length());
            memcpy(mContent, ss.str().c_str(), mContentSize);
            mContent[mContentSize] = 0;
            return mContentSize;
        }

    private:
        uint32_t        mContentSize;
        int32_t         mHour;
        char            mContent[2048];
    };
}


#endif  /*__T3D_LOG_ITEM_H__*/
