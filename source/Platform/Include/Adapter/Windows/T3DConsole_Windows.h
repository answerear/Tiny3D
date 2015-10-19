
#ifndef __T3D_CONSOLE_ADAPTER_WINDOWS_H__
#define __T3D_CONSOLE_ADAPTER_WINDOWS_H__


#include "Adapter/T3DConsoleInterface.h"


namespace Tiny3D
{
    class Console_Windows : public ConsoleInterface
    {
        T3D_DISABLE_COPY(Console_Windows);

    public:
        Console_Windows();

    protected:
        virtual void print(const char *pText);
    };
}


#endif  /*__T3D_CONSOLE_ADAPTER_WINDOWS_H__*/
