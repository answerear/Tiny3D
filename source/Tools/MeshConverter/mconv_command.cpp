

#include "mconv_command.h"


namespace mconv
{
    bool Command::parse(int argc, char *argv[], Settings &settings)
    {
        printCommand(argc, argv);

        if (argc < 3)
        {
            printHelp();
        }

        return true;
    }

    void Command::printCommand(int argc, char *argv[]) const
    {
        int i = 0;

        for (i = 1; i < argc; ++i)
        {
            if (i > 1)
                printf(" ");
            printf(argv[i]);
        }

        printf("\n");
    }

    void Command::printHelp() const
    {
        printf("Usage: mesh-conv.exe [options] <input> [<output>]\n");
        printf("\n");
        printf("Options:\n");
        printf("-?       : Display this help information.\n");
#ifdef ALLOW_INPUT_TYPE
        printf("-i <type>: Set the type of the input file to <type>\n");
#endif
        printf("-o <type>: Set the type of the output file to <type>\n");
        printf("-f       : Flip the V texture coordinates.\n");
        printf("-p       : Pack vertex colors to one float.\n");
        printf("-m <size>: The maximum amount of vertices or indices a mesh may contain (default: 32k)\n");
        printf("-b <size>: The maximum amount of bones a nodepart can contain (default: 12)\n");
        printf("-w <size>: The maximum amount of bone weights per vertex (default: 4)\n");
        printf("-v       : Verbose: print additional progress information\n");
        printf("\n");
        printf("<input>  : The filename of the file to convert.\n");
        printf("<output> : The filename of the converted file.\n");
        printf("\n");
        printf("<type>   : FBX, G3DJ (json) or G3DB (binary).\n");
    }
}
