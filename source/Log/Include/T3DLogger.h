

#ifndef __T3D_LOGGER_H__
#define __T3D_LOGGER_H__



#include "T3DSingleton.h"
#include "T3DLogPrerequisites.h"
#include <T3DPlatform.h>
#include <mutex>
#include <thread>


namespace Tiny3D
{
    class T3D_LOG_API Logger : public Singleton<Logger>
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

    public:
        Logger();
        virtual ~Logger();

        void setLevel(Level eLevel);

        bool startup(uint32_t appID, const String &tag, bool truncate = false, bool force = false);

        void trace(Level level, const char *filename, int32_t line, const char *fmt, ...);

        void shutdown();

        void enterBackground();

        void enterForeground();

    private:
        String makeLogFileName(uint32_t appID, const String &tag);

        bool openLogFile(const String &filename);
        void closeLogFile();

        static 

        class Item
        {
            friend class Logger;

        public:
            Item(Level level, const char *filename, int32_t line, const char *content)
                : mLevel(level)
                , mLine(line)
            {
                int32_t len = strlen(filename);
                len = (len > sizeof(mFilename) ? sizeof(mFilename) : len);
                memcpy(mFilename, filename, len);
                len = strlen(content);
                len = (len > sizeof(mContent) ? sizeof(mContent) : len);
                memcpy(mContent, content, len);
            }

        private:
            Level   mLevel;
            char    mFilename[64];
            int32_t mLine;
            char    mContent[2048];
        };

        typedef std::list<Item*>            ItemList;
        typedef ItemList::iterator          ItemListItr;
        typedef ItemList::const_iterator    ItemListConstItr;

        Level               mLevel;

        ItemList            mItemList[2];
        FileDataStream      mFileStream;

        std::mutex          mListMutex;
        std::thread         mWriteThread;
    };
}

#define T3D_LOGGER          Tiny3D::Logger::getInstance()


#endif  /*__T3D_LOGGER_H__*/
