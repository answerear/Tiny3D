
#ifndef __MCONV_COMMAND_H__
#define __MCONV_COMMAND_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    class Settings;

    class Command
    {
    public:
        bool parse(int argc, char *argv[], Settings &settings);

    protected:
        void printCommand();
        void printHelp();
    };
}


#endif  /*__MCONV_COMMAND_H__*/
