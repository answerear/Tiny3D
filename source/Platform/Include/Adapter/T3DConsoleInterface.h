
#ifndef __T3D_CONSOLE_INTERFACE_H__
#define __T3D_CONSOLE_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    /**
     * @class T3DConsoleAdapterInterface
     * @brief Interface class
     */
    class ConsoleInterface
    {
        T3D_DECLARE_INTERFACE(ConsoleInterface);

    public:
        /**
         * @brief Output some text in console.
         * @param [in] pText : Point to the text that need be output in console with ending character '\0'
         */
        virtual void print(const char *pText) = 0;
    };
}


#endif  /*__T3D_CONSOLE_INTERFACE_H__*/
