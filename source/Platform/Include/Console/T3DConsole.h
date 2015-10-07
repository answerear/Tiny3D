

#ifndef __T3D_CONSOLE_H__
#define __T3D_CONSOLE_H__


#include "T3DSingleton.h"
#include "T3DPlatformMacro.h"


namespace Tiny3D
{
    class ConsoleInterface;
    class FactoryInterface;

    class T3D_PLATFORM_API Console : public Singleton<Console>
    {
        T3D_DISABLE_COPY(Console);

    public:
        static const uint32_t MAX_CONTENT_SIZE;

        Console(FactoryInterface *pAdatperFactory);
        virtual ~Console();

        void print(const char *pText, ...);

    protected:
        ConsoleInterface *m_pconsoleAdapter;
    };


#define T3D_CONSOLE     Console::getInstance()
}


#endif  /*__T3D_CONSOLE_H__*/